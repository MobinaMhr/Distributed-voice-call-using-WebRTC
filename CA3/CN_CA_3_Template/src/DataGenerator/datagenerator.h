#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <QObject>
#include <QVector>
#include <random>

class DataGenerator : public QObject {
    Q_OBJECT

public:
    explicit DataGenerator(QObject* parent = nullptr);
    void setParameters(int cycleCount, int packetCount, int pcCount);
    QVector<int> generatePacketDistribution();

Q_SIGNALS:
    void distributionGenerated(const QVector<int>& distribution);

private:
    int cycleCount;
    int packetCount;
    int pcCount;

    std::random_device rd;
    std::mt19937 generator;
    std::poisson_distribution<int> poissonDist;

    float getUniformRandom();
    int getCycleFromValue(float value);
};

#endif // DATAGENERATOR_H
