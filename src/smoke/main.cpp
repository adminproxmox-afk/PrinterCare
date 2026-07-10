#include <QApplication>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QEventLoop>
#include <QFileInfo>
#include <QIcon>
#include <QSystemTrayIcon>
#include <QTemporaryDir>
#include <QTime>
#include <QTimer>

#include "core/AppInfo.h"
#include "managers/ConfigManager.h"
#include "managers/HistoryManager.h"
#include "managers/ReminderManager.h"
#include "managers/StartupManager.h"
#include "managers/TrayManager.h"

#include <iostream>

namespace
{
struct SmokeContext
{
    int failures = 0;

    void pass(const QString& message)
    {
        const QByteArray utf8 = message.toUtf8();
        std::cout << "[PASS] " << utf8.constData() << std::endl;
    }

    void fail(const QString& message)
    {
        const QByteArray utf8 = message.toUtf8();
        std::cerr << "[FAIL] " << utf8.constData() << std::endl;
        ++failures;
    }
};

bool expectTrue(SmokeContext& context, bool value, const QString& message)
{
    if (value)
    {
        context.pass(message);
        return true;
    }

    context.fail(message);
    return false;
}

template <typename T>
bool expectEqual(SmokeContext& context, const T& actual, const T& expected, const QString& message)
{
    if (actual == expected)
    {
        context.pass(message);
        return true;
    }

    context.fail(message);
    return false;
}
} // namespace

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);
    QApplication::setApplicationName(QStringLiteral("Printer Care Smoke Test"));
    QApplication::setOrganizationName(QString::fromLatin1(printercare::app::kOrganizationName));
    QApplication::setOrganizationDomain(QStringLiteral("printercare.local"));
    QApplication::setStyle(QStringLiteral("Fusion"));
    QApplication::setQuitOnLastWindowClosed(false);

    SmokeContext context;

    QTemporaryDir tempDir;
    if (!expectTrue(context, tempDir.isValid(), QStringLiteral("Temporary workspace created")))
    {
        return 1;
    }

    const QString configPath = tempDir.filePath(QStringLiteral("settings.json"));
    const QString historyPath = tempDir.filePath(QStringLiteral("history.json"));

    printercare::models::AppSettings settings;
    settings.themeMode = printercare::models::ThemeMode::Dark;
    settings.launchAtStartup = true;
    settings.startMinimizedToTray = true;
    settings.showNotifications = true;
    settings.testPrintRule.enabled = true;
    settings.testPrintRule.preferredDay = printercare::models::WeekDay::Friday;
    settings.testPrintRule.intervalDays = 5;
    settings.headCleaningRule.enabled = false;

    printercare::managers::ConfigManager configManager(configPath);
    QString errorMessage;
    expectTrue(context, configManager.save(settings, &errorMessage), QStringLiteral("Config saved"));
    if (!errorMessage.isEmpty())
    {
        context.fail(errorMessage);
    }

    const auto loadedSettings = configManager.load(&errorMessage);
    expectTrue(context, loadedSettings.has_value(), QStringLiteral("Config loaded"));
    if (loadedSettings.has_value())
    {
        expectEqual(context, loadedSettings->themeMode, settings.themeMode, QStringLiteral("Theme mode round-trip"));
        expectEqual(context, loadedSettings->launchAtStartup, settings.launchAtStartup, QStringLiteral("Autostart flag round-trip"));
        expectEqual(context, loadedSettings->startMinimizedToTray, settings.startMinimizedToTray, QStringLiteral("Tray flag round-trip"));
        expectEqual(context, loadedSettings->showNotifications, settings.showNotifications, QStringLiteral("Notification flag round-trip"));
    }
    if (!errorMessage.isEmpty())
    {
        context.fail(errorMessage);
    }

    printercare::managers::HistoryManager historyManager(historyPath);
    const QDateTime firstService(QDate(2026, 7, 1), QTime(9, 30), Qt::LocalTime);
    const QDateTime secondService(QDate(2026, 7, 4), QTime(13, 15), Qt::LocalTime);
    expectTrue(
        context,
        historyManager.record(printercare::managers::HistoryManager::ServiceType::TestPrint, firstService, QStringLiteral("Smoke test")),
        QStringLiteral("First history record saved"));
    expectTrue(
        context,
        historyManager.record(printercare::managers::HistoryManager::ServiceType::HeadCleaning, secondService, QStringLiteral("Smoke test")),
        QStringLiteral("Second history record saved"));

    printercare::managers::HistoryManager reloadedHistory(historyPath);
    expectEqual(context, reloadedHistory.totalRecords(), 2, QStringLiteral("History record count"));
    const auto testPrintDate = reloadedHistory.lastServiceDate(printercare::managers::HistoryManager::ServiceType::TestPrint);
    const auto headCleaningDate = reloadedHistory.lastServiceDate(printercare::managers::HistoryManager::ServiceType::HeadCleaning);
    expectTrue(context, testPrintDate.has_value(), QStringLiteral("Test print date available"));
    expectTrue(context, headCleaningDate.has_value(), QStringLiteral("Head cleaning date available"));
    if (testPrintDate.has_value())
    {
        expectEqual(context, testPrintDate->toString(Qt::ISODate), firstService.date().toString(Qt::ISODate), QStringLiteral("Test print history date"));
    }
    if (headCleaningDate.has_value())
    {
        expectEqual(context, headCleaningDate->toString(Qt::ISODate), secondService.date().toString(Qt::ISODate), QStringLiteral("Head cleaning history date"));
    }

    const auto reminderEvaluation = printercare::managers::ReminderManager::evaluate(
        QDate(2026, 7, 6),
        testPrintDate,
        settings.testPrintRule);
    expectTrue(context, reminderEvaluation.isEnabled, QStringLiteral("Reminder enabled"));
    expectTrue(context, reminderEvaluation.isDue, QStringLiteral("Reminder due state"));

    if (printercare::managers::StartupManager::isSupported())
    {
        const QString smokeAppName = QStringLiteral("Printer Care Smoke Test");
        const QString executablePath = QCoreApplication::applicationFilePath();
        const bool restored = printercare::managers::StartupManager::setLaunchAtStartupEnabled(
            false,
            executablePath,
            smokeAppName,
            &errorMessage);
        expectTrue(context, restored, QStringLiteral("Startup entry cleared before smoke run"));

        const bool enabled = printercare::managers::StartupManager::setLaunchAtStartupEnabled(
            true,
            executablePath,
            smokeAppName,
            &errorMessage);
        expectTrue(context, enabled, QStringLiteral("Startup entry created"));
        if (enabled)
        {
            expectTrue(
                context,
                printercare::managers::StartupManager::isLaunchAtStartupEnabled(smokeAppName),
                QStringLiteral("Startup entry visible in registry"));
        }

        const bool disabled = printercare::managers::StartupManager::setLaunchAtStartupEnabled(
            false,
            executablePath,
            smokeAppName,
            &errorMessage);
        expectTrue(context, disabled, QStringLiteral("Startup entry removed"));
        if (!errorMessage.isEmpty())
        {
            context.fail(errorMessage);
        }
    }
    else
    {
        context.pass(QStringLiteral("Startup integration skipped on this platform"));
    }

    printercare::managers::TrayManager trayManager;
    const bool traySupported = trayManager.isSupported();
    expectTrue(context, traySupported, QStringLiteral("System tray available"));
    if (traySupported)
    {
        const bool trayInitialized = trayManager.initialize(
            QIcon(QStringLiteral(":/icons/app.svg")),
            QStringLiteral("Printer Care Smoke Test"));
        expectTrue(context, trayInitialized, QStringLiteral("Tray manager initialized"));
        if (trayInitialized)
        {
            trayManager.showMessage(
                QStringLiteral("Printer Care Smoke Test"),
                QStringLiteral("Notification path smoke test"),
                QSystemTrayIcon::Information,
                2000);
            QEventLoop loop;
            QTimer::singleShot(250, &loop, &QEventLoop::quit);
            loop.exec();
            context.pass(QStringLiteral("Tray notification dispatched"));
        }
    }

    if (!errorMessage.isEmpty())
    {
        context.fail(errorMessage);
    }

    if (context.failures == 0)
    {
        context.pass(QStringLiteral("Smoke run completed successfully"));
        return 0;
    }

    std::cerr << "Smoke run failed with " << context.failures << " error(s)." << std::endl;
    return 1;
}
