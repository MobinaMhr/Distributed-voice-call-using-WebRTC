#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "callmanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    CallManager callManager;

    engine.rootContext()->setContextProperty("callManager", &callManager);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("WebRTC_Call", "Main");

    return app.exec();
}
