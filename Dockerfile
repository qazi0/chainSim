# Stage 1: Build Qt Backend
FROM ubuntu:22.04 AS backend-builder

# Install minimal build dependencies
RUN apt-get -y update && DEBIAN_FRONTEND=noninteractive && apt-get install -y --no-install-recommends \
    build-essential \
    binutils \
    cmake \
    git \
    ca-certificates \
    python3.10 \
    python3-pip \
    python3-setuptools \
    libdbus-1-3 \
    libpulse-mainloop-glib0 \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install --upgrade pip && pip install aqtinstall

ARG QT=6.8.0
ARG QT_MODULES="qthttpserver qtnetworkauth qtwebsockets"
ARG QT_HOST=linux
ARG QT_TARGET=desktop
ARG QT_ARCH=linux_gcc_64
RUN aqt install --outputdir /opt/qt ${QT} ${QT_HOST} ${QT_TARGET} ${QT_ARCH} -m ${QT_MODULES}

ENV PATH=/opt/qt/${QT}/gcc_64/bin:$PATH
ENV LD_LIBRARY_PATH=/opt/qt/${QT}/gcc_64/lib:$LD_LIBRARY_PATH
ENV QT_PLUGIN_PATH=/opt/qt/${QT}/gcc_64/plugins/
ENV QML_IMPORT_PATH=/opt/qt/${QT}/gcc_64/qml/
ENV QML2_IMPORT_PATH=/opt/qt/${QT}/gcc_64/qml/

# Copy backend source code
WORKDIR /app/backend
COPY . .

# Build backend
RUN mkdir -p build && cd build \
    && cmake .. \
    && make -j$(nproc)

# Stage 2: Build Next.js Frontend
FROM node:18-alpine AS frontend-builder

# Copy frontend source code
WORKDIR /app/frontend
COPY chainsim-ui/ .

# Install dependencies and build
RUN npm ci
RUN npm run build

# Stage 3: Final image
FROM ubuntu:22.04

# Install Node.js 18.x and other dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    python3.10 \
    python3-pip \
    binutils \
    libdbus-1-3 \
    libpulse-mainloop-glib0 \
    curl \
    net-tools \
    && curl -fsSL https://deb.nodesource.com/setup_18.x | bash - \
    && apt-get install -y nodejs \
    && rm -rf /var/lib/apt/lists/*

# Install npm explicitly
RUN npm install -g npm@latest
RUN pip3 install --upgrade pip && pip install aqtinstall

ARG QT=6.8.0
ARG QT_MODULES="qthttpserver qtnetworkauth qtwebsockets"
ARG QT_HOST=linux
ARG QT_TARGET=desktop
ARG QT_ARCH=linux_gcc_64
RUN aqt install --outputdir /opt/qt ${QT} ${QT_HOST} ${QT_TARGET} ${QT_ARCH} -m ${QT_MODULES}

ENV PATH /opt/qt/${QT}/gcc_64/bin:$PATH
ENV LD_LIBRARY_PATH=/opt/qt/${QT}/gcc_64/lib:$LD_LIBRARY_PATH
ENV QT_PLUGIN_PATH /opt/qt/${QT}/gcc_64/plugins/
ENV QML_IMPORT_PATH /opt/qt/${QT}/gcc_64/qml/
ENV QML2_IMPORT_PATH /opt/qt/${QT}/gcc_64/qml/

# Add build arguments for configuration
ARG CORS_DOMAIN=https://default-domain.run.app
ARG API_URL=http://0.0.0.0:47761

# Set environment variables
ENV ALLOWED_ORIGIN=${CORS_DOMAIN}
ENV NEXT_PUBLIC_API_URL=${API_URL}
ENV ALLOWED_ORIGINS="http://localhost:3000,http://localhost:47761,${CORS_DOMAIN}"

# Copy backend binary and frontend build
WORKDIR /app
COPY --from=backend-builder /app/backend/build/ChainSimQServe /app/
COPY --from=frontend-builder /app/frontend/.next /app/frontend/.next
COPY --from=frontend-builder /app/frontend/public /app/frontend/public
COPY --from=frontend-builder /app/frontend/package*.json /app/frontend/
COPY --from=frontend-builder /app/frontend/node_modules /app/frontend/node_modules

RUN echo "LD_LIBRARY_PATH: ${LD_LIBRARY_PATH}"
RUN echo "Qt path: ${PATH}"
RUN strip --remove-section=.note.ABI-tag /opt/qt/${QT}/gcc_64/lib/libQt6Core.so.6

# Create and copy startup script with better logging
RUN echo '#!/bin/bash\n\
    set -e\n\
    \n\
    # Start the Qt backend server with detailed logging\n\
    echo "LD_LIBRARY_PATH: ${LD_LIBRARY_PATH}"\n\
    echo "Qt path: ${PATH}"\n\
    echo "Starting Qt backend server..."\n\
    ./ChainSimQServe --server --log_level 2 &\n\
    SERVER_PID=$!\n\
    \n\
    # Wait briefly to ensure server starts\n\
    sleep 10\n\
    \n\
    # Check if server is running and listening\n\
    echo "Checking network status:"\n\
    netstat -tulpn | grep 47761\n\
    \n\
    # Check if server is running\n\
    if ! kill -0 $SERVER_PID 2>/dev/null; then\n\
    echo "Failed to start Qt backend server"\n\
    exit 1\n\
    fi\n\
    \n\
    echo "Qt backend server started successfully on port 47761"\n\
    \n\
    # Start the Next.js frontend\n\
    echo "Starting Next.js frontend..."\n\
    cd frontend && NODE_ENV=production npm start\n\
    ' > /app/start.sh

RUN chmod +x /app/start.sh

# Expose ports
EXPOSE 3000 47761

# Start both services
CMD ["/app/start.sh"]