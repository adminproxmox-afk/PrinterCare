#include "managers/StartupManager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSettings>

namespace
{
QString effectiveApplicationName(const QString& applicationName)
{
    if (!applicationName.isEmpty())
    {
        return applicationName;
    }

    const QString coreName = QCoreApplication::applicationName();
    if (!coreName.isEmpty())
    {
        return coreName;
    }

    return QStringLiteral("Printer Care");
}

QString effectiveExecutablePath(const QString& executablePath)
{
    if (!executablePath.isEmpty())
    {
        return executablePath;
    }

    return QCoreApplication::applicationFilePath();
}
} // namespace

namespace printercare::managers
{
bool StartupManager::isSupported() noexcept
{
#ifdef Q_OS_WIN
    return true;
#else
    return false;
#endif
}

QString StartupManager::startupValueName(const QString& applicationName)
{
    return effectiveApplicationName(applicationName);
}

QString StartupManager::startupCommand(const QString& executablePath)
{
    const QString resolvedExecutablePath = effectiveExecutablePath(executablePath);
    if (resolvedExecutablePath.isEmpty())
    {
        return {};
    }

    return QStringLiteral("\"%1\"").arg(QDir::toNativeSeparators(resolvedExecutablePath));
}

bool StartupManager::isLaunchAtStartupEnabled(const QString& applicationName)
{
#ifdef Q_OS_WIN
    const QSettings settings(
        QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"),
        QSettings::NativeFormat);

    return settings.value(startupValueName(applicationName)).toString().trimmed().isEmpty() == false;
#else
    Q_UNUSED(applicationName);
    return false;
#endif
}

bool StartupManager::setLaunchAtStartupEnabled(
    bool enabled,
    const QString& executablePath,
    const QString& applicationName,
    QString* errorMessage)
{
#ifdef Q_OS_WIN
    const QString keyName = startupValueName(applicationName);
    const QString command = startupCommand(executablePath);
    if (command.isEmpty())
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QObject::tr("Unable to determine the application executable path.");
        }
        return false;
    }

    QSettings settings(
        QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"),
        QSettings::NativeFormat);

    if (enabled)
    {
        settings.setValue(keyName, command);
    }
    else
    {
        settings.remove(keyName);
    }

    settings.sync();
    if (settings.status() != QSettings::NoError)
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QObject::tr("Unable to update Windows startup settings.");
        }
        return false;
    }

    return true;
#else
    Q_UNUSED(enabled);
    Q_UNUSED(executablePath);
    Q_UNUSED(applicationName);
    if (errorMessage != nullptr)
    {
        *errorMessage = QObject::tr("Windows startup integration is only available on Windows.");
    }
    return false;
#endif
}
} // namespace printercare::managers
