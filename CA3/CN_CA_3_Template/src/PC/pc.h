#ifndef PC_H
#define PC_H

#include <QObject>

class PC : public QObject
{
    Q_OBJECT

public:
    explicit PC(QObject *parent = nullptr);

Q_SIGNALS:
};

#endif    // PC_H
