# EventCoordinator

The EventsCoordinator class is designed to manage and coordinate events in a network simulation. 
It handles the generation and distribution of packets over a specified number of cycles, using a given distribution from DataGenerator. 
The class inherits from QThread, allowing it to run in a separate thread, and utilizes QTimer for timing events.

Its constructor sets some private attributes, connects the `handleTimeout` signal to the instance of `QTimer` object.
It also sets input parameters to the created instance of DataGenerator. 

```cpp
EventsCoordinator::EventsCoordinator(double lambda, int cycleCount, int packetCount, int pcCount, int cycleLength, QThread* parent)
    : QThread(parent), m_cycleCount(cycleCount), m_cycleLength(cycleLength) {
    m_dataGenerator = new DataGenerator(this);
    m_dataGenerator->setParameters(cycleCount, packetCount, pcCount, lambda, 0.99999);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &EventsCoordinator::handleTimeout);
}
```

It has a destructor to delete created pointers:
```cpp
EventsCoordinator::~EventsCoordinator() {
    delete m_timer;
    delete m_dataGenerator;
}
```

The `instance()` method provides a singleton instance of the this class.
```cpp
EventsCoordinator* EventsCoordinator::instance(double lambda, int cycleCount, int packetCount, int pcCount, int cycleLength, QThread* parent) {
    if (!m_self) {
        m_self = new EventsCoordinator(lambda, cycleCount, packetCount, pcCount, cycleLength, parent);
    }
    return m_self;
}
```

The `release()` method releases the singleton instance of the class.
```cpp
void EventsCoordinator::release() {
    if (m_self) {
        delete m_self;
        m_self = nullptr;
    }
}
```

The `run()` method starts the timer and generates the packet distribution, then enters the event loop and starts execution.
```cpp
void EventsCoordinator::run() {
    m_timer->start(m_cycleLength);
    m_distribution = m_dataGenerator->generatePacketDistribution();
    exec();
}
```

The `handleTimeout()` method handles the timeout event, processes packets for the current cycle, and increments the cycle counter.
It stops the timer instance of the current cycle count exceeds the cycle count.
```cpp
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
```

The `handleCurrentCyclePackets()` method chooses sender and receiver IDs randomly using `QRandomGenerator`.
These data are saved in a PacketDetails datatype declared in `Globals` file.
Note that the senders cannot send packet to themselves and they cannot send more thant one packet in each cycle, so these are implemented in do-while loops.
The `usedSenders` is a set to push senderIDs in order to avoid senders send more thant one packet in this cycle.
The packet type is temporarily set to 0. 
It saves all packet details in a packets vector, then it emits a signal with the generated packets.
```cpp
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
```