#ifndef REALDASHCANTCPSERVER_H
#define REALDASHCANTCPSERVER_H

#include <QObject>

QT_FORWARD_DECLARE_CLASS(QTcpServer)
QT_FORWARD_DECLARE_CLASS(QTcpSocket)
QT_FORWARD_DECLARE_CLASS(QTimer)

class RealDashCanTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit RealDashCanTcpServer(quint16 port, bool debug = false, QObject *parent = nullptr);
    ~RealDashCanTcpServer();

signals:

private slots:
    void onNewConnection();
    void onSocketDisconnected();
    void onTimer();

private:
    void sendCanFramesToClients();

private:
    bool m_debug;
    QTcpServer *m_tcpServer = nullptr;
    QTimer *m_timer;
    QList<QTcpSocket *> m_clients;
};

#endif // REALDASHCANTCPSERVER_H
