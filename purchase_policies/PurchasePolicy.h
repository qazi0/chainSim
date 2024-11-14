#ifndef CHAINSIM_PURCHASEPOLICY_H
#define CHAINSIM_PURCHASEPOLICY_H

#include <QString>
#include <QMap>
#include <QVector>
#include <cmath>
#include <QTextStream>
#include <QObject>

class PurchasePolicy : public QObject
{
    Q_OBJECT

public:
    using simulation_records_t = QMap<QString, QVector<qint64>>;

    PurchasePolicy(QObject *parent = nullptr) {}

    [[nodiscard]] virtual qint64
    get_purchase(const simulation_records_t &pastRecords, quint32 current_day) const = 0;

    [[nodiscard]] virtual QString name() const = 0;

    [[nodiscard]] virtual QString get_calculation_details(
        const simulation_records_t &pastRecords,
        quint32 current_day) const = 0;
};

#endif // CHAINSIM_PURCHASEPOLICY_H
