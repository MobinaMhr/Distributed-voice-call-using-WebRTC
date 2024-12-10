#include "EventsCoordinator.h"

EventsCoordinator::EventsCoordinator(double lambda, int cycleCount, int packetCount, int pcCount, QThread *parent) :
    QThread {parent}
{
    m_dataGenerator->setParameters(cycleCount, packetCount, pcCount, lambda, 0.99999);
}

EventsCoordinator *
EventsCoordinator::instance(double lambda, int cycleCount, int packetCount, int pcCount, QThread *parent)
{
    if(!m_self)
    {
        m_self = new EventsCoordinator(lambda, cycleCount, packetCount, pcCount, parent);
    }

    return m_self;
}

void
EventsCoordinator::release()
{
    if(m_self)
    {
        delete m_self;
        m_self = nullptr;
    }
}

