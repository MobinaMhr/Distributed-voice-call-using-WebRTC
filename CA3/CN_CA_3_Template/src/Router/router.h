#ifndef ROUTER_H
#define ROUTER_H

#include <QObject>

class Router : public QObject
{
    Q_OBJECT

public:
    explicit Router(QObject *parent = nullptr);

Q_SIGNALS:
};

#endif    // ROUTER_H
