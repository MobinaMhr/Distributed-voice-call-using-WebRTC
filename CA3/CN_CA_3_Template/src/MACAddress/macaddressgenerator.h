#ifndef MACADDRESSGENERATOR_H
#define MACADDRESSGENERATOR_H

#include <QObject>
#include <QSet>
#include <random>
#include "macaddress.h"

class MacAddressGenerator : public QObject
{
    Q_OBJECT

public:
    explicit MacAddressGenerator(QObject *parent = nullptr);
    MacAddress generateMacAddress();

private:
    QSet<uint64_t> generatedAddresses;
    std::mt19937_64 rng;
    uint64_t generateUniqueAddress();

Q_SIGNALS:
};

#endif    // MACADDRESSGENERATOR_H
