#ifndef TOPOLOGYBUILDER_H
#define TOPOLOGYBUILDER_H

#include <QObject>

class TopologyBuilder : public QObject
{
    Q_OBJECT

public:
    explicit TopologyBuilder(QObject *parent = nullptr);

Q_SIGNALS:
};

#endif    // TOPOLOGYBUILDER_H
