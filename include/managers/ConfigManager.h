#pragma once

#include "models/AppSettings.h"

#include <QString>

#include <optional>

namespace printercare::managers
{
class ConfigManager final
{
public:
    explicit ConfigManager(QString configFilePath = {});

    [[nodiscard]] const QString& configFilePath() const noexcept;
    void setConfigFilePath(QString configFilePath);

    [[nodiscard]] std::optional<printercare::models::AppSettings> load(QString* errorMessage = nullptr) const;
    [[nodiscard]] bool save(
        const printercare::models::AppSettings& settings,
        QString* errorMessage = nullptr) const;

    [[nodiscard]] static QString defaultConfigFilePath();

private:
    QString configFilePath_;
};
} // namespace printercare::managers
