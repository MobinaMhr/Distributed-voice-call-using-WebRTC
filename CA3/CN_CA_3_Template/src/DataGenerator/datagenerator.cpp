#include "DataGenerator.h"

DataGenerator::DataGenerator(QObject* parent)
    : QObject(parent), cycleCount(0), packetCount(0), pcCount(0),
    generator(rd()), poissonDist(1) {}

void DataGenerator::setParameters(int cycleCount, int packetCount, int pcCount) {
    this->cycleCount = cycleCount;
    this->packetCount = packetCount;
    this->pcCount = pcCount;
    poissonDist = std::poisson_distribution<int>(packetCount);
}

float DataGenerator::getUniformRandom() {
    std::uniform_real_distribution<float> uniformDist(0.0, 1.0);
    return uniformDist(generator);
}

int DataGenerator::getCycleFromValue(float value) {
}

QVector<int> DataGenerator::generatePacketDistribution() {
    QVector<int> distribution(cycleCount, 0);

    for (int i = 0; i < packetCount; ++i) {
        bool valid = false;
        while (!valid) {
            float randomValue = getUniformRandom();
            int cycle = getCycleFromValue(randomValue);

            if (distribution[cycle] < pcCount) {
                ++distribution[cycle];
                valid = true;
            }
        }
    }

    Q_EMIT distributionGenerated(distribution);
    return distribution;
}
