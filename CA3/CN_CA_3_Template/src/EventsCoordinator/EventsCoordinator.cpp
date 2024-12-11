#include "EventsCoordinator.h"

EventsCoordinator::EventsCoordinator(double lambda, int cycleCount, int packetCount, int pcCount, int cycleLength, QThread* parent)
    : QThread(parent), m_cycleCount(cycleCount), m_cycleLength(cycleLength) {
    m_dataGenerator = new DataGenerator(this);
    m_dataGenerator->setParameters(cycleCount, packetCount, pcCount, lambda, 0.99999);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &EventsCoordinator::handleTimeout);
}

EventsCoordinator::~EventsCoordinator() {
    delete m_timer;
    delete m_dataGenerator;
}

EventsCoordinator* EventsCoordinator::instance(double lambda, int cycleCount, int packetCount, int pcCount, int cycleLength, QThread* parent) {
    if (!m_self) {
        m_self = new EventsCoordinator(lambda, cycleCount, packetCount, pcCount, cycleLength, parent);
    }
    return m_self;
}

void EventsCoordinator::release() {
    if (m_self) {
        delete m_self;
        m_self = nullptr;
    }
}

void EventsCoordinator::run() {
    m_timer->start(m_cycleLength);
    m_distribution = m_dataGenerator->generatePacketDistribution();
    exec();
}

void EventsCoordinator::handleTimeout() {
    if (m_currentCycle >= m_cycleCount) {
        if (m_timer->isActive()) {
            m_timer->stop();
        }
        return;
    }

    int packetsCount = m_distribution[m_currentCycle];
    handleCurrentCyclePackets(packetsCount);

    m_currentCycle++;
}

void EventsCoordinator::handleCurrentCyclePackets(const int &packetsCount) {
    std::vector<UT::PacketDetails> packets;
    std::set<int> usedSenders;

    for (int idx = 0; idx < packetsCount; ++idx) {
        int senderID;

        do {
            senderID = QRandomGenerator::global()->bounded(m_pcCount);
        } while (usedSenders.find(senderID) != usedSenders.end());

        int receiverID;

        do {
            receiverID = QRandomGenerator::global()->bounded(m_pcCount);
        } while (receiverID == senderID);

        int packetType = 0;

        packets.emplace_back(senderID, receiverID, packetType);
        usedSenders.insert(senderID);

        if (usedSenders.size() == m_pcCount) {
            break;
        }
    }

    Q_EMIT packetsReady(packets);
}
