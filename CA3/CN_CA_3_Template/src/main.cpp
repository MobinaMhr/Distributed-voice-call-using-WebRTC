#include <QCoreApplication>
#include <QDebug>
int
main(int argv, char* argc[])
{
    QCoreApplication app(argv, argc);
    qDebug() << "shit";
    return app.exec();
}
