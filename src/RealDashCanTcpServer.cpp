#include "RealDashCanTcpServer.h"
#include <QtNetwork>

static const char* canFrameMarker   = "\x44\x33\x22\x11";
static const quint32 canFrameId     = 3200;

RealDashCanTcpServer::RealDashCanTcpServer(quint16 port, bool debug, QObject *parent) :
    QDBusAbstractAdaptor(parent),
    m_port(port),
    m_debug(debug),
    m_tcpServer(new QTcpServer(this)),
    m_timer(new QTimer(this))
{
}

RealDashCanTcpServer::~RealDashCanTcpServer()
{
    stopServer();
    delete m_timer;
    delete m_tcpServer;
}

void RealDashCanTcpServer::startServer()
{
    resetData();
    if (!m_tcpServer->isListening() && m_tcpServer->listen(QHostAddress::AnyIPv4, m_port))
    {
        if (m_debug)
            qDebug() << "RealDash CAN server listening on port " << m_port;
        connect(m_tcpServer, &QTcpServer::newConnection,
                this, &RealDashCanTcpServer::onNewConnection);
        connect(m_timer, &QTimer::timeout,
                this, &RealDashCanTcpServer::onTimer);
    }
}

void RealDashCanTcpServer::stopServer()
{
    if (m_tcpServer->isListening())
    {
        m_timer->stop();
        // Disconnect all connected clints
        disconnectAllClients();
        disconnect(m_tcpServer, &QTcpServer::newConnection,
                   this, &RealDashCanTcpServer::onNewConnection);
        disconnect(m_timer, &QTimer::timeout,
                   this, &RealDashCanTcpServer::onTimer);
        resetData();
        m_tcpServer->close();
        if (m_debug)
            qDebug() << "RealDash CAN server stopped listening on port " << m_port;
    }
}

void RealDashCanTcpServer::resetData()
{
    if (m_debug)
        qDebug() << "Reset metrics data to zero";
    m_revs = 0;
    m_speedMph = 0;
    m_fuelPercent = 0;
    m_gear = 0;
}

void RealDashCanTcpServer::setRevs(ushort revs)
{
    m_revs = revs;
    qDebugMetricUnsignedShort("REVS", m_revs);
    sendCanFramesToClients();
}

void RealDashCanTcpServer::setSpeed(ushort mph)
{
    m_speedMph = mph;
    qDebugMetricUnsignedShort("SPEED", m_speedMph);
    sendCanFramesToClients();
}

void RealDashCanTcpServer::setFuelLevel(ushort fuelPercent)
{
    m_fuelPercent = fuelPercent;
    qDebugMetricUnsignedShort("FUEL", m_fuelPercent);
    sendCanFramesToClients();
}

void RealDashCanTcpServer::setGear(ushort gear)
{
    m_gear = gear;
    qDebugMetricUnsignedShort("GEAR", m_gear);
    sendCanFramesToClients();
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

void RealDashCanTcpServer::disconnectAllClients()
{
    QList<QTcpSocket *>::iterator it;
    for (it = m_clients.begin(); it != m_clients.end(); ++it)
    {
        QTcpSocket *pClient = *it;
        if (m_debug)
            qDebug() << "Disconnect: " << pClient->peerAddress();
        disconnect(pClient, &QTcpSocket::disconnected, this, &RealDashCanTcpServer::onSocketDisconnected);
        pClient->close();
        pClient->deleteLater();
    }
    m_clients.clear();
}

void RealDashCanTcpServer::sendCanFramesToClients()
{
    // Build CAN frame
    
    // Frame marker
    QByteArray canFrameData(canFrameMarker, 4);
    // Frame id
    canFrameData.append(char(canFrameId & 0x000000ff));
    canFrameData.append(char((canFrameId & 0x0000ff00) >> 8));
    canFrameData.append(char((canFrameId & 0x00ff0000) >> 16));
    canFrameData.append(char((canFrameId & 0xff000000) >> 24));
    // RPM
    canFrameData.append(char(m_revs & 0x00ff));
    canFrameData.append(char((m_revs & 0xff00) >> 8));
    // Speed - MPH
    canFrameData.append(char(m_speedMph & 0x00ff));
    canFrameData.append(char((m_speedMph & 0xff00) >> 8));
    // Fuel - %
    canFrameData.append(char(m_fuelPercent & 0x00ff));
    canFrameData.append(char((m_fuelPercent & 0xff00) >> 8));
    // Gear
    canFrameData.append(char(m_gear & 0x00ff));
    canFrameData.append(char((m_gear & 0xff00) >> 8));

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

void RealDashCanTcpServer::qDebugMetricUnsignedShort(const char *name, ushort value)
{
    if (m_debug)
        qDebug() << "DBus client set" << name << "to" << value << "(" << hex << showbase << value << ")";
}