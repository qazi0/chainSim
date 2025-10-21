#ifndef CHAINSIM_CHAINLOGGER_HPP
#define CHAINSIM_CHAINLOGGER_HPP

#include <QDebug>
#include <QString>

class ChainLogger
{
    quint32 m_logging_level{};

public:
    explicit ChainLogger(quint32 loggingLevel = 1) : m_logging_level{loggingLevel} {}

    void info(const QString &message) const
    {
        if (m_logging_level < 1)
            return;
        qInfo() << "[INFO > ]" << message;
    }

    void warn(const QString &message) const
    {
        if (m_logging_level < 1)
            return;
        qWarning() << "[WARN ! ]" << message;
    }

    void error(const QString &message) const
    {
        if (m_logging_level < 1)
            return;
        qCritical() << "[ERROR !!]" << message;
    }

    void debug(const QString &message)
    {
        if (m_logging_level < 1)
            return;
        qDebug() << "[DEBG > ]" << message;
    }
};

#endif // CHAINSIM_CHAINLOGGER_HPP
