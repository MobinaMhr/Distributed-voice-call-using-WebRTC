#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "./src/callmanager.h"
#include "./network/socket.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    CallManager callManager;

    const QUrl url0(QStringLiteral("ws://localhost:3000"));
    Socket socket(url0);

    socket.connectToServer();

    QString message = QString("Incoming offer..............");
    socket.sendMessage(message);


    engine.rootContext()->setContextProperty("callManager", &callManager);

    const QUrl url(QStringLiteral("qrc:/WebRTC_Call/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
