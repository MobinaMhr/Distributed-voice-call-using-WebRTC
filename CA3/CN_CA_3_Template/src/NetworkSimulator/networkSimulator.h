#ifndef NETWORKSIMULATOR_H
#define NETWORKSIMULATOR_H

#include <QObject>
#include <QVector>
#include <QSharedPointer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "../AutonomousSystem/autonomoussystem.h"
#include "../EventsCoordinator/EventsCoordinator.h"
#include <QFile>
#include <QTextStream>

class NetworkSimulator : public QObject {
    Q_OBJECT

public:
    explicit NetworkSimulator(const QString &configFilePath, QObject *parent = nullptr);
    ~NetworkSimulator() override;

    void initializeNetwork();
    void startPhaseOne(const QString &configFilePath);
    void startPhaseTwo();
    void startPhaseThree();
    void startPhaseFour();
    // void resetNetwork();
    // void cleanLogs();

    // void printRoutingTable(int nodeId);
    void isConfigLoaded();

private:
    QVector<QSharedPointer<AutonomousSystem>>   m_autonomousSystems;
    EventsCoordinator*                          m_eventCoordinator;
    QJsonObject                                 m_config;
    int                                         m_routerCount;
    int                                         m_routingCompletionCount;
    int                                         m_totalRouters;
    QFile                                       m_logFile;

    void log(const QString &message);
    void loadConfig(const QString &filePath);
    void connectAS();
    std::pair<int, int> calculateOffsets();
    void monitorRoutingCompletion();
    void analyzeResults();
    UT::TopologyType returnTopologyType(QString topologyType);
    void generateEventCoordinator(double lambda, int cycleCount, int packetCount, int pcCount, int cycleLength);
};

#endif // NETWORKSIMULATOR_H
