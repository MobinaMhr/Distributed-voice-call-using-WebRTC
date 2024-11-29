#include "EventsCoordinator.h"

EventsCoordinator::EventsCoordinator(QThread *parent) :
    QThread {parent}
{}

EventsCoordinator *
EventsCoordinator::instance(QThread *parent)
{
    if(!m_self)
    {
        m_self = new EventsCoordinator(parent);
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
