#include "ChainSimServer.h"
#include "ChainSimBuilder.h"
#include <QFile>
#include <QUrlQuery>
#include <QHttpServerResponse>
#include <QHttpServerRequest>
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
                           try
                           {
                               // Log request if log level is 2
                               if (query.hasQueryItem("log_level") && query.queryItemValue("log_level").toUInt() >= 2)
                               {
                                   printRequestDetails(query);
                               }

                               QJsonObject result = runSimulation(query);
                               m_logger.info("Simulation finished successfully");
                               return QHttpServerResponse(result);
                           }
                           catch (const std::exception &e)
                           {
                               QJsonObject error{
                                   {"error", e.what()}};
                               m_logger.error(QString("Simulation failed: %1").arg(e.what()));
                               return QHttpServerResponse(error, QHttpServerResponse::StatusCode::BadRequest);
                           }
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

        // Extract parameters
        auto log_level = params.queryItemValue("log_level").toUInt();
        auto simulation_length = params.queryItemValue("simulation_length").toULongLong();
        auto lead_time = params.queryItemValue("average_lead_time").toULongLong();
        auto demand = params.queryItemValue("average_demand").toDouble();
        auto starting_inventory = params.queryItemValue("starting_inventory").toULongLong();
        auto output_file = params.queryItemValue("output_file");

        // Create policy
        auto policy = createPolicy(params);

        // Create and configure simulation
        auto chainSimulator = ChainSimBuilder()
                                  .setSimulationName("ChainSim")
                                  .setSimulationLength(simulation_length)
                                  .setLeadTime(lead_time)
                                  .setAverageDemand(demand)
                                  .setStartingInventory(starting_inventory)
                                  .setLoggingLevel(log_level)
                                  .create();

        chainSimulator->initialize_simulation();

        // Run simulation
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
    }

} // namespace qz
