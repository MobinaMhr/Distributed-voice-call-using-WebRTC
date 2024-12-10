#include "DataGenerator.h"

DataGenerator::DataGenerator(QObject* parent)
    : QObject(parent), cycleCount(0), packetCount(0), pcCount(0),
    generator(rd()) {}

void DataGenerator::setParameters(int cycleCount, int packetCount, int pcCount, double lambda, float threshold) {
    this->cycleCount = cycleCount;
    this->packetCount = packetCount;
    this->pcCount = pcCount;
    this->lambda = lambda;

    this->devidingStep = calculate_deviding_step(threshold);
}

float DataGenerator::getUniformRandom() {
    std::uniform_real_distribution<float> uniformDist(0.0, 1.0);
    return uniformDist(generator);
}

double DataGenerator::poissonPMF(int x) {
    return qExp(-this->lambda) * qPow(this->lambda, x) / tgamma(x + 1);
}

int DataGenerator::poissonInverseCDF(float p) {
    // poissonInverseCDF
    double cumulative = 0.0;
    int x = 0;

    while (cumulative < p) {
        cumulative += poissonPMF(x);
        if (cumulative >= p) {
            return x;
        }
        x++;
    }
    return x;
}

QVector<int> DataGenerator::generatePacketDistribution() {
    QVector<int> distribution(cycleCount, 0);

    for (int i = 0; i < packetCount; ++i) {
        bool valid = false;
        while (!valid) {
            int cycle = getRandomCycle();

            if (distribution[cycle] < pcCount) {
                ++distribution[cycle];
                valid = true;
            }
        }
    }

    Q_EMIT distributionGenerated(distribution);
    return distribution;
}


int DataGenerator::calculate_deviding_step(float tr) {
    int border = poissonInverseCDF(tr);
    return border / this->cycleCount;
}

int DataGenerator::getRandomCycle() {
    int temp = poissonInverseCDF(getUniformRandom());
    int cycle = -1;
    for (int i = 0; i < this->cycleCount - 1; ++i) {
        if (temp >= i * this->devidingStep && temp < (i + 1) * this->devidingStep) {
            cycle = i;
            break;
        }
    }
    if (cycle == -1)
        cycle = this->cycleCount - 1;
    return cycle;
}
