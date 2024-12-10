#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <QObject>
#include <QVector>
#include <random>

class DataGenerator : public QObject {
    Q_OBJECT

public:
    explicit DataGenerator(QObject* parent = nullptr);
    void setParameters(int cycleCount, int packetCount, int pcCount, double lambda);
    QVector<int> generatePacketDistribution();

Q_SIGNALS:
    void distributionGenerated(const QVector<int>& distribution);

private:
    int cycleCount;
    int packetCount;
    int pcCount;
    double lambda;

    std::random_device rd;
    std::mt19937 generator;

    float getUniformRandom();
    int getCycle(float value);
    double poissonPMF(int x);
};

#endif // DATAGENERATOR_H
