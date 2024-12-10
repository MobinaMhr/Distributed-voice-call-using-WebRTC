#ifndef EVENTSCOORDINATOR_H
#define EVENTSCOORDINATOR_H

#include <chrono>

#include <QObject>
#include <QThread>

#include "../DataGenerator/datagenerator.h"

class EventsCoordinator : public QThread
{
    Q_OBJECT

    typedef std::chrono::milliseconds Millis;

    explicit EventsCoordinator(double lambda, int cycleCount, int packetCount, int pcCount, QThread *parent = nullptr);

public:    // constructors
    ~EventsCoordinator() override = default;
    static EventsCoordinator *instance(double lambda, int cycleCount, int packetCount, int pcCount, QThread *parent = nullptr);
    static void               release();

public:    // methods

public:    // getter and setters



public Q_SLOTS:

private Q_SLOTS:

private:    // members
    inline static EventsCoordinator *m_self = nullptr;
    DataGenerator *m_dataGenerator;
};

#endif    // EVENTSCOORDINATOR_H
