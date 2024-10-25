#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include <QObject>

class AudioOutput : public QObject
{
    Q_OBJECT
public:
    explicit AudioOutput(QObject *parent = nullptr);

signals:
};

#endif // AUDIOOUTPUT_H
