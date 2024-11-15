#include "ChainSimServer.h"
#include "ChainSimBuilder.h"
#include <QFile>
#include <QUrlQuery>
#include <QHttpServerResponse>
#include <QHttpServerRequest>
#include <QHttpHeaders>
#include "purchase_policies/PurchaseROP.h"
#include "purchase_policies/PurchaseEOQ.h"
#include "purchase_policies/PurchaseTPOP.h"

namespace qz
{

    ChainSimServer::ChainSimServer(QObject *parent)
        : QObject(parent), m_logger(2)
    {
    }

    bool ChainSimServer::start(quint16 port)
    {
        // Create TCP server
        m_tcpServer = std::make_unique<QTcpServer>();

        // Setup routes before binding
        m_server.route("/simulate", QHttpServerRequest::Method::Post,
                       [this](const QHttpServerRequest &request)
                       {
                           QUrlQuery query(request.url().query());
                           QString origin;
                           try
                           {
                               // Get the Origin header from the request
                               origin = request.value("Origin");
                               if (origin.isEmpty())
                               {
                                   origin = "*"; // Fallback to allow all if no Origin header
                               }

                               // Log request if log level is 2
                               if (query.hasQueryItem("log_level") && query.queryItemValue("log_level").toUInt() >= 2)
                               {
                                   printRequestDetails(query);
                               }

                               QJsonObject result = runSimulation(query);
                               m_logger.info("Simulation finished successfully");

                               // Create response with CORS headers
                               auto response = QHttpServerResponse(result);
                               QHttpHeaders headers = response.headers();
                               if (isAllowedOrigin(origin))
                                   headers.append("Access-Control-Allow-Origin", origin);

                               headers.append("Access-Control-Allow-Methods", "POST, OPTIONS");
                               headers.append("Access-Control-Allow-Headers", "Content-Type, Authorization");
                               response.setHeaders(headers);
                               return response;
                           }
                           catch (const std::exception &e)
                           {
                               QJsonObject error{
                                   {"error", e.what()}};
                               m_logger.error(QString("Simulation failed: %1").arg(e.what()));

                               // Create error response with CORS headers
                               auto response = QHttpServerResponse(error, QHttpServerResponse::StatusCode::BadRequest);
                               QHttpHeaders headers = response.headers();
                               if (isAllowedOrigin(origin))
                                   headers.append("Access-Control-Allow-Origin", origin);

                               headers.append("Access-Control-Allow-Methods", "POST, OPTIONS");
                               headers.append("Access-Control-Allow-Headers", "Content-Type, Authorization");
                               response.setHeaders(headers);
                               return response;
                           }
                       });

        // Add OPTIONS route for CORS preflight
        m_server.route("/simulate", QHttpServerRequest::Method::Options,
                       [this](const QHttpServerRequest &request)
                       {
                           QString origin = request.value("Origin");
                           if (origin.isEmpty())
                               origin = "*";

                           auto response = QHttpServerResponse(QHttpServerResponse::StatusCode::NoContent);
                           QHttpHeaders headers = response.headers();
                           if (this->isAllowedOrigin(origin))
                               headers.append("Access-Control-Allow-Origin", origin);

                           headers.append("Access-Control-Allow-Methods", "POST, OPTIONS");
                           headers.append("Access-Control-Allow-Headers", "Content-Type, Authorization");
                           response.setHeaders(headers);
                           return response;
                       });

        // Start listening on localhost with specified port
        if (!m_tcpServer->listen(QHostAddress::LocalHost, port))
        {
            m_logger.error(QString("Failed to start TCP server on port %1: %2")
                               .arg(port)
                               .arg(m_tcpServer->errorString()));
            return false;
        }

        // Bind HTTP server to TCP server
        if (!m_server.bind(m_tcpServer.get()))
        {
            m_logger.error("Failed to bind HTTP server to TCP socket");
            m_tcpServer->close();
            return false;
        }

        quint16 actualPort = m_tcpServer->serverPort();
        m_logger.info(QString("Server running on http://127.0.0.1:%1/").arg(actualPort));
        m_logger.info("Use endpoint /simulate with POST method and query parameters for simulation requests");

        // TCP server is now owned by HTTP server
        m_tcpServer.release();

        return true;
    }

    void ChainSimServer::printRequestDetails(const QUrlQuery &params)
    {
        QString separator(80, '=');
        QString subseparator(80, '-');

        m_logger.info(separator);
        m_logger.info("Received Simulation Request");
        m_logger.info(subseparator);

        // Print all parameters
        QStringList paramNames = {
            "simulation_length",
            "average_lead_time",
            "average_demand",
            "policy",
            "starting_inventory",
            "log_level"};

        // Policy-specific parameters
        QString policy = params.queryItemValue("policy");
        if (policy == "EOQ")
        {
            paramNames << "ordering_cost" << "holding_cost";
        }
        else if (policy == "TPOP")
        {
            paramNames << "purchase_period";
        }

        for (const auto &param : paramNames)
        {
            if (params.hasQueryItem(param))
            {
                m_logger.info(QString("%1: %2")
                                  .arg(param, -25)
                                  .arg(params.queryItemValue(param)));
            }
        }

        m_logger.info(separator);
    }

    QJsonObject ChainSimServer::runSimulation(const QUrlQuery &params)
    {
        validateParameters(params);

        // Extract common parameters
        auto log_level = params.queryItemValue("log_level").toUInt();
        auto simulation_length = params.queryItemValue("simulation_length").toULongLong();
        auto lead_time = params.queryItemValue("average_lead_time").toULongLong();
        auto starting_inventory = params.queryItemValue("starting_inventory").toULongLong();
        auto seed = params.queryItemValue("seed").toUInt();
        bool deterministic = params.hasQueryItem("deterministic");
        QString output_file = params.queryItemValue("output_file");

        // Get demand distribution and its parameters
        QString distribution = params.queryItemValue("demand_distribution");

        // Create builder and maintain reference
        ChainSimBuilder &builder = *(new ChainSimBuilder());
        builder.setSimulationName("ChainSim")
            .setSimulationLength(simulation_length)
            .setLeadTime(lead_time)
            .setDemandDistribution(distribution)
            .setDeterministic(deterministic)
            .setSeed(seed)
            .setStartingInventory(starting_inventory)
            .setLoggingLevel(log_level);

        // Configure distribution-specific parameters
        if (distribution == "normal")
        {
            auto demand = params.queryItemValue("average_demand").toDouble();
            auto std_demand = params.queryItemValue("std_demand").toDouble();
            builder.setAverageDemand(demand)
                .setDemandStdDev(std_demand);
        }
        else if (distribution == "gamma")
        {
            auto shape = params.queryItemValue("gamma_shape").toDouble();
            auto scale = params.queryItemValue("gamma_scale").toDouble();
            builder.setGammaParameters(shape, scale);
        }
        else if (distribution == "poisson")
        {
            auto demand = params.queryItemValue("average_demand").toDouble();
            builder.setAverageDemand(demand);
        }
        else if (distribution == "uniform")
        {
            auto min = params.queryItemValue("uniform_min").toDouble();
            auto max = params.queryItemValue("uniform_max").toDouble();
            builder.setUniformParameters(min, max);
        }
        else if (distribution == "fixed")
        {
            auto demand = params.queryItemValue("average_demand").toDouble();
            builder.setAverageDemand(demand)
                .setDeterministic(true);
        }

        // Create and run simulation
        auto chainSimulator = builder.create();
        delete &builder; // Clean up the builder

        chainSimulator->initialize_simulation();

        // Create policy before simulation
        auto policy = createPolicy(params);

        // Run simulation with policy
        chainSimulator->simulate(*policy);

        // Get results
        auto simulation_records = chainSimulator->get_simulation_records();

        // Optionally save to file if specified
        if (!output_file.isEmpty())
        {
            QFile file(output_file);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&file);
                // Write CSV header
                out << "Day,inventory_quantity,demand_quantity,procurement_quantity,"
                    << "purchase_quantity,sale_quantity,lost_sale_quantity\n";

                // Write data
                for (int i = 0; i < simulation_records.begin().value().size(); ++i)
                {
                    out << i << ","
                        << simulation_records["inventory_quantity"][i] << ","
                        << simulation_records["demand_quantity"][i] << ","
                        << simulation_records["procurement_quantity"][i] << ","
                        << simulation_records["purchase_quantity"][i] << ","
                        << simulation_records["sale_quantity"][i] << ","
                        << simulation_records["lost_sale_quantity"][i] << "\n";
                }
                file.close();
            }
        }

        // Convert results to JSON
        return simulationRecordsToJson(simulation_records);
    }

    QJsonObject ChainSimServer::simulationRecordsToJson(const ChainSim::simulation_records_t &records)
    {
        QJsonObject result;

        for (auto it = records.begin(); it != records.end(); ++it)
        {
            QJsonArray values;
            for (const auto &value : it.value())
            {
                values.append(QJsonValue(value));
            }
            result[it.key()] = values;
        }

        return result;
    }

    std::unique_ptr<PurchasePolicy> ChainSimServer::createPolicy(const QUrlQuery &params)
    {
        QString policy_name = params.queryItemValue("policy");
        auto lead_time = params.queryItemValue("average_lead_time").toUInt();
        auto demand = params.queryItemValue("average_demand").toDouble();

        if (policy_name == "ROP")
        {
            return std::make_unique<PurchaseROP>(lead_time, demand);
        }
        else if (policy_name == "EOQ")
        {
            double ordering_cost = params.queryItemValue("ordering_cost").toDouble();
            double holding_cost = params.queryItemValue("holding_cost").toDouble();
            return std::make_unique<PurchaseEOQ>(lead_time, demand, ordering_cost, holding_cost);
        }
        else if (policy_name == "TPOP")
        {
            unsigned review_period = params.queryItemValue("purchase_period").toUInt();
            return std::make_unique<PurchaseTPOP>(lead_time, demand, review_period);
        }

        throw std::invalid_argument("Unsupported policy: " + policy_name.toStdString());
    }

    void ChainSimServer::validateParameters(const QUrlQuery &params)
    {
        // Required parameters
        QStringList required = {
            "simulation_length",
            "average_lead_time",
            "average_demand",
            "std_demand",
            "policy"};

        for (const auto &param : required)
        {
            if (!params.hasQueryItem(param))
            {
                throw std::invalid_argument("Missing required parameter: " + param.toStdString());
            }
        }

        // Validate policy-specific parameters
        QString policy = params.queryItemValue("policy");
        if (policy == "EOQ")
        {
            if (!params.hasQueryItem("ordering_cost") || !params.hasQueryItem("holding_cost"))
            {
                throw std::invalid_argument("EOQ policy requires ordering_cost and holding_cost parameters");
            }
        }
        else if (policy == "TPOP")
        {
            if (!params.hasQueryItem("purchase_period"))
            {
                throw std::invalid_argument("TPOP policy requires purchase_period parameter");
            }
        }

        // Add validation for demand distribution parameters
        QString distribution = params.queryItemValue("demand_distribution");
        if (distribution.isEmpty())
        {
            throw std::invalid_argument("Missing demand_distribution parameter");
        }

        if (distribution == "normal")
        {
            if (!params.hasQueryItem("average_demand") || !params.hasQueryItem("std_demand"))
            {
                throw std::invalid_argument("Normal distribution requires average_demand and std_demand parameters");
            }
        }
        else if (distribution == "gamma")
        {
            if (!params.hasQueryItem("gamma_shape") || !params.hasQueryItem("gamma_scale"))
            {
                throw std::invalid_argument("Gamma distribution requires shape and scale parameters");
            }
        }
        else if (distribution == "uniform")
        {
            if (!params.hasQueryItem("uniform_min") || !params.hasQueryItem("uniform_max"))
            {
                throw std::invalid_argument("Uniform distribution requires min and max parameters");
            }
        }
        else if (distribution == "poisson" || distribution == "fixed")
        {
            if (!params.hasQueryItem("average_demand"))
            {
                throw std::invalid_argument("Distribution requires average_demand parameter");
            }
        }
    }

    bool ChainSimServer::isAllowedOrigin(const QString &origin)
    {
        QByteArray allowedOriginsEnv = qgetenv("ALLOWED_ORIGINS");
        if (allowedOriginsEnv.isEmpty())
        {
            // Default allowed origins
            static const QStringList defaultOrigins = {
                "http://localhost:3000",
                "http://localhost:47761"};
            return defaultOrigins.contains(origin);
        }

        // Parse comma-separated origins from environment
        QStringList allowedOrigins = QString::fromUtf8(allowedOriginsEnv).split(',');
        return allowedOrigins.contains(origin);
    }

} // namespace qz
