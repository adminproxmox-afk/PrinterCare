#include <QApplication>
#include <QFont>
#include <QIODevice>
#include <QLocalServer>
#include <QLocalSocket>
#include <QSharedMemory>
#include <QIcon>
#include <QPoint>
#include <QRect>
#include <QScreen>
#include <QThread>
#include <QWidget>

#include <optional>

#include "core/AppInfo.h"
#include "localization/AppTranslator.h"
#include "managers/ConfigManager.h"
#include "ui/MainWindow.h"

namespace
{
constexpr char kSingleInstanceServerName[] = "PrinterCare.SingleInstance";
constexpr char kSingleInstanceMemoryKey[] = "PrinterCare.SingleInstance.Memory";

void centerWindowOnPrimaryScreen(QWidget& window)
{
    const QScreen* const screen = QApplication::primaryScreen();
    if (screen == nullptr)
    {
        return;
    }

    const QRect availableGeometry = screen->availableGeometry();
    const QPoint topLeft(
        availableGeometry.center().x() - window.width() / 2,
        availableGeometry.center().y() - window.height() / 2);

    window.move(topLeft);
}

bool activateRunningInstance()
{
    for (int attempt = 0; attempt < 20; ++attempt)
    {
        QLocalSocket socket;
        socket.connectToServer(QString::fromLatin1(kSingleInstanceServerName), QIODevice::WriteOnly);
        if (socket.waitForConnected(100))
        {
            socket.write("show");
            socket.flush();
            socket.waitForBytesWritten(100);
            socket.disconnectFromServer();
            return true;
        }

        QThread::msleep(50);
    }

    return false;
}
} // namespace

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);
    QApplication::setApplicationName(QString::fromLatin1(printercare::app::kApplicationName));
    QApplication::setOrganizationName(QString::fromLatin1(printercare::app::kOrganizationName));
    QApplication::setOrganizationDomain(QStringLiteral("printercare.local"));
    QApplication::setApplicationVersion(QString::fromLatin1(printercare::app::kApplicationVersion));
    QApplication::setStyle(QStringLiteral("Fusion"));
    QApplication::setFont(QFont(QStringLiteral("Segoe UI Variable Text")));
    QApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/app.svg")));
    QApplication::setQuitOnLastWindowClosed(false);

    const printercare::managers::ConfigManager bootstrapConfigManager;
    printercare::models::AppSettings bootstrapSettings{};
    if (const std::optional<printercare::models::AppSettings> loadedSettings = bootstrapConfigManager.load(); loadedSettings.has_value())
    {
        bootstrapSettings = *loadedSettings;
    }

    printercare::localization::AppTranslator translator(bootstrapSettings.languageMode);
    application.installTranslator(&translator);

    QSharedMemory instanceGuard(QString::fromLatin1(kSingleInstanceMemoryKey));
    if (!instanceGuard.create(1))
    {
        if (instanceGuard.error() == QSharedMemory::AlreadyExists)
        {
            if (activateRunningInstance())
            {
                return 0;
            }

            return 0;
        }
    }

    QLocalServer instanceServer;

    MainWindow mainWindow;
    mainWindow.resize(printercare::app::kWindowWidth, printercare::app::kWindowHeight);
    centerWindowOnPrimaryScreen(mainWindow);

    QObject::connect(&instanceServer, &QLocalServer::newConnection, &mainWindow, [&instanceServer, &mainWindow]()
    {
        while (instanceServer.hasPendingConnections())
        {
            if (QLocalSocket* const socket = instanceServer.nextPendingConnection(); socket != nullptr)
            {
                socket->readAll();
                socket->disconnectFromServer();
                socket->deleteLater();
            }
        }

        QMetaObject::invokeMethod(&mainWindow, "showFromTray", Qt::QueuedConnection);
    });

    if (!instanceServer.listen(QString::fromLatin1(kSingleInstanceServerName)))
    {
        QLocalServer::removeServer(QString::fromLatin1(kSingleInstanceServerName));
        instanceServer.listen(QString::fromLatin1(kSingleInstanceServerName));
    }

    mainWindow.show();
    mainWindow.raise();
    mainWindow.activateWindow();

    return application.exec();
}
