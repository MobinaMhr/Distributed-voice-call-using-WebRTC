#ifndef OSPF_H
#define OSPF_H

#include <QObject>
#include <QMap>
#include <QSet>
#include <QTimer>
#include <QDebug>
#include "../Router/router.h"

class OSPF : public QObject
{
    Q_OBJECT

public:
    explicit OSPF(Router *router, QObject *parent = nullptr);
    ~OSPF();
};

#endif // OSPF_H
