#ifndef AUTONOMOUSSYSTEM_H
#define AUTONOMOUSSYSTEM_H

#include <QObject>

class AutonomousSystem : public QObject
{
    Q_OBJECT

public:
    explicit AutonomousSystem(QObject *parent = nullptr);

Q_SIGNALS:
};

#endif    // AUTONOMOUSSYSTEM_H
