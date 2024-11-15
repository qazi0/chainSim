#ifndef CHAINSIM_SERVER_H
#define CHAINSIM_SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QHttpServer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <memory>
#include "ChainSim.h"
#include "utils/ChainLogger.hpp"

namespace qz
{

    class ChainSimServer : public QObject
    {
        Q_OBJECT

    public:
        explicit ChainSimServer(QObject *parent = nullptr);
        bool start(quint16 port = 47761);

    private:
        QHttpServer m_server;
        std::unique_ptr<QTcpServer> m_tcpServer;
        ChainLogger m_logger;

        // Helper methods
        QJsonObject runSimulation(const QUrlQuery &params);
        QJsonObject simulationRecordsToJson(const ChainSim::simulation_records_t &records);
        std::unique_ptr<PurchasePolicy> createPolicy(const QUrlQuery &params);
        void validateParameters(const QUrlQuery &params);
        void printRequestDetails(const QUrlQuery &params);
        bool isAllowedOrigin(const QString &origin);
    };

} // namespace qz

#endif // CHAINSIM_SERVER_H
