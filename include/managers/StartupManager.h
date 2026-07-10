#pragma once

#include <QString>

namespace printercare::managers
{
class StartupManager final
{
public:
    [[nodiscard]] static bool isSupported() noexcept;

    [[nodiscard]] static bool isLaunchAtStartupEnabled(
        const QString& applicationName = QString());

    [[nodiscard]] static bool setLaunchAtStartupEnabled(
        bool enabled,
        const QString& executablePath = QString(),
        const QString& applicationName = QString(),
        QString* errorMessage = nullptr);

private:
    static QString startupValueName(const QString& applicationName);
    static QString startupCommand(const QString& executablePath);
};
} // namespace printercare::managers
