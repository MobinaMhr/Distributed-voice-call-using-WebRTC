#ifndef EVENTSCOORDINATOR_H
#define EVENTSCOORDINATOR_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QVector>
#include <QRandomGenerator>
#include <set>
#include "../DataGenerator/datagenerator.h"
#include "../Globals/Globals.h"

class EventsCoordinator : public QThread
{
    Q_OBJECT

    explicit EventsCoordinator(double lambda, int cycleCount, int packetCount, int pcCount, int cycleLength, QThread *parent = nullptr);

public:
    ~EventsCoordinator() override;
    static EventsCoordinator *instance(double lambda, int cycleCount, int packetCount, int pcCount, int cycleLength, QThread *parent = nullptr);
    static void release();

protected:
    void run() override;
    void handleCurrentCyclePackets(const int &packetsCount);

public Q_SLOTS:

private Q_SLOTS:
    void handleTimeout();

Q_SIGNALS:
    void packetsReady(std::vector<UT::PacketDetails> packets);
    void nextTick(int currentCycle);
    void distributePackets(const QVector<int>& distribution);

private:
    inline static EventsCoordinator *m_self = nullptr;
    DataGenerator*  m_dataGenerator;
    QTimer* m_timer = nullptr;
    QVector<int>    m_distribution;
    int             m_currentCycle = 0;
    int             m_cycleCount;
    int             m_pcCount;
    int             m_cycleLength;
};

#endif    // EVENTSCOORDINATOR_H
