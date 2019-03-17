#ifndef REALDASHCANTCPSERVER_H
#define REALDASHCANTCPSERVER_H

#include <QObject>
#include <QtDBus/QDBusAbstractAdaptor>

QT_FORWARD_DECLARE_CLASS(QTcpServer)
QT_FORWARD_DECLARE_CLASS(QTcpSocket)
QT_FORWARD_DECLARE_CLASS(QTimer)

#define CAN_SERVER_SERVICE_NAME "nd.realdash.canserver"
#define CAN_SERVER_INTERFACE    "nd.realdash.canserver.DashBoard"
#define CAN_SERVER_OBJECT_PATH  "/nd/realdash/canserver"

/**
 * @brief The RealDashCanTcpServer class implements a RealDash CAN Server for RealDash clients. The data inputs are exposed over DBus
 */
class RealDashCanTcpServer : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", CAN_SERVER_INTERFACE)
public:
    explicit RealDashCanTcpServer(quint16 port, bool debug = false, QObject *parent = nullptr);
    ~RealDashCanTcpServer();

// Public slots are exposed over DBus
public slots:
    void startServer();
    void stopServer();
    void resetData();
    Q_NOREPLY void setRevs(ushort revs);
    Q_NOREPLY void setSpeed(ushort mph);
    Q_NOREPLY void setFuelLevel(ushort fuelPercent);
    Q_NOREPLY void setGear(uchar gear);

signals:

// Private slots for internal use, not exposed over DBus
private slots:
    void onNewConnection();
    void onSocketDisconnected();
    void onTimer();

// Private internal methodss
private:
    void disconnectAllClients();
    void sendCanFramesToClients();

private:
    quint16 m_port;
    bool m_debug;
    QTcpServer *m_tcpServer = nullptr;
    QTimer *m_timer;
    QList<QTcpSocket *> m_clients;

    // Dash data
    quint16 m_revs;
    quint16 m_speedMph;
    quint16 m_fuelPercent;
    quint8 m_gear;
};

#endif // REALDASHCANTCPSERVER_H
