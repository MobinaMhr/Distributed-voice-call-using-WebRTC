#ifndef TOPOLOGYCONTROLLER_H
#define TOPOLOGYCONTROLLER_H

#include <QObject>

class TopologyController : public QObject
{
    Q_OBJECT

public:
    explicit TopologyController(QObject *parent = nullptr);

Q_SIGNALS:
};

#endif    // TOPOLOGYCONTROLLER_H
