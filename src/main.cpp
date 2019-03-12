#include <QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>

#include "RealDashCanTcpServer.h"

static const char *DEFAULT_PORT = "35000";

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("RealDash CAN Server");
    parser.addHelpOption();

    QCommandLineOption dbgOption(QStringList() << "d" << "debug",
            QCoreApplication::translate("main", "Debug output [default: off]."));
    parser.addOption(dbgOption);
    QCommandLineOption portOption(QStringList() << "p" << "port",
            QCoreApplication::translate("main", "Port to listen on [default: 35000]."),
            QCoreApplication::translate("main", "port"), QLatin1Literal(DEFAULT_PORT));
    parser.addOption(portOption);
    parser.process(a);
    bool debug = parser.isSet(dbgOption);
    uint port = parser.value(portOption).toUInt();

    new RealDashCanTcpServer(quint16(port), debug);

    return a.exec();
}
