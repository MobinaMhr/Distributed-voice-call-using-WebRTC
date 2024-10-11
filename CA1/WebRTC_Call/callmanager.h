#ifndef CALLMANAGER_H
#define CALLMANAGER_H

#include <QObject>

class CallManager : public QObject
{
    Q_OBJECT
public:
    explicit CallManager(QObject *parent = nullptr);

signals:
};

#endif // CALLMANAGER_H
