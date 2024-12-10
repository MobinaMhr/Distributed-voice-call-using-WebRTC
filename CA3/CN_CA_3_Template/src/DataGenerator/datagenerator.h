#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <QObject>
#include <QVector>
#include <random>

class DataGenerator : public QObject {
    Q_OBJECT

public:
    explicit DataGenerator(QObject* parent = nullptr);
    void setParameters(int cycleCount, int packetCount, int pcCount, double lambda, float threshold);
    QVector<int> generatePacketDistribution();

Q_SIGNALS:
    void distributionGenerated(const QVector<int>& distribution);

private:
    int cycleCount;
    int packetCount;
    int pcCount;
    int devidingStep;
    double lambda;

    std::random_device rd;
    std::mt19937 generator;

    float getUniformRandom();
    int poissonInverseCDF(float value);
    double poissonPMF(int x);
    int calculate_deviding_step(float tr);
    int getRandomCycle();
};

#endif // DATAGENERATOR_H
