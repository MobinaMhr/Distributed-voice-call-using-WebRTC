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
}
