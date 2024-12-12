#ifndef IPHEADER_H
#define IPHEADER_H

#include <QObject>

class IPHeader : public QObject
{
    Q_OBJECT

public:
    explicit IPHeader(QObject *parent = nullptr);

Q_SIGNALS:
};

#endif    // IPHEADER_H
