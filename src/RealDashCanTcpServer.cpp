#include "RealDashCanTcpServer.h"
#include <QtNetwork>

static const char* canFrameMarker   = "\x44\x33\x22\x11";
static const char* canFrameId       = "\x80\x0c\x00\x00";

RealDashCanTcpServer::RealDashCanTcpServer(quint16 port, bool debug, QObject *parent) :
    QObject(parent),
    m_debug(debug),
    m_tcpServer(new QTcpServer(this)),
    m_timer(new QTimer(this))
{
    if (m_tcpServer->listen(QHostAddress::Any, port)) {
        if (m_debug)
            qDebug() << "RealDash CAN server listening on port " << port;
        connect(m_tcpServer, &QTcpServer::newConnection,
                this, &RealDashCanTcpServer::onNewConnection);
        connect(m_timer, &QTimer::timeout,
                this, &RealDashCanTcpServer::onTimer);
    }
}

RealDashCanTcpServer::~RealDashCanTcpServer()
{
    m_timer->stop();
    m_tcpServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
    delete m_timer;
    delete m_tcpServer;
}

void RealDashCanTcpServer::onNewConnection()
{
    QTcpSocket *pSocket = m_tcpServer->nextPendingConnection();
    if (m_debug)
        qDebug() << "New connection: " << pSocket->peerAddress();
   connect(pSocket, &QTcpSocket::disconnected, this, &RealDashCanTcpServer::onSocketDisconnected);

    m_clients << pSocket;

    if (m_clients.count() > 0 && !m_timer->isActive())
    {
        if (m_debug)
            qDebug() << "Start timer ";
        m_timer->start(500);
    }
}

void RealDashCanTcpServer::onSocketDisconnected()
{
    QTcpSocket *pClient = qobject_cast<QTcpSocket *>(sender());
    if (m_debug)
        qDebug() << "Disconnection: " << pClient->peerAddress();
    if (pClient) {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
    if (m_clients.count() < 1 && m_timer->isActive())
    {
        if (m_debug)
            qDebug() << "Stop timer ";
        m_timer->stop();
    }
}

void RealDashCanTcpServer::onTimer()
{
    sendCanFramesToClients();
}

void RealDashCanTcpServer::sendCanFramesToClients()
{
    // Consts for now
    quint16 rpm = 3500;
    quint16 speedMph = 120;
    quint16 fuelPercent = 25;
    char gear = 0;

    // Build CAN frame
    QByteArray canFrameData(canFrameMarker, 4);
    canFrameData.append(canFrameId, 4);
    // RPM
    canFrameData.append(char((rpm & 0xff00) >> 8));
    canFrameData.append(char(rpm & 0x00ff));
    // Speed - MPH
    canFrameData.append(char((speedMph & 0xff00) >> 8));
    canFrameData.append(char(speedMph & 0x00ff));
    // Fuel - %
    canFrameData.append(char((fuelPercent & 0xff00) >> 8));
    canFrameData.append(char(fuelPercent & 0x00ff));
    // Gear
    canFrameData.append(gear);

    // Send CAN frame to all connected clints
    QList<QTcpSocket *>::iterator it;
    for (it = m_clients.begin(); it != m_clients.end(); ++it)
    {
        QTcpSocket *pClient = *it;
        if (m_debug)
            qDebug() << "Send CAN data to: " << pClient->peerAddress();
        pClient->write(canFrameData);
    }
}
