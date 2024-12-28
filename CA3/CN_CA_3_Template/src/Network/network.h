#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QVector>
#include <QSharedPointer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "../AutonomousSystem/autonomoussystem.h"
#include "../EventsCoordinator/EventsCoordinator.h"
#include "../Globals/globals.h"

class Network : public QObject {
    Q_OBJECT

public:
    explicit Network(const QString &configFilePath, QObject *parent = nullptr);
    ~Network() override;

    void initializeNetwork();
    // void startPhaseOne();
    // void startPhaseTwo();
    // void startPhaseThree();
    // void startPhaseFour();

    // void analyzeResults();
    // void resetNetwork();
    // void cleanLogs();

    // void printRoutingTable(int nodeId);
    void isConfigLoaded();

private:
    QVector<QSharedPointer<AutonomousSystem>> m_autonomousSystems;
    EventsCoordinator* m_eventCoordinator;
    QJsonObject m_config;
    int m_routerCount;

    void loadConfig(const QString &filePath);
    void connectAS();
    std::pair<int, int> calculateOffsets();
    // void monitorRoutingCompletion();

    int m_routingCompletionCount;
    int m_totalRouters;
};

#endif // NETWORK_H
