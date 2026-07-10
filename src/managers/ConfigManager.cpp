#include "managers/ConfigManager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QStandardPaths>

#include <utility>

namespace
{
constexpr int kSchemaVersion = 1;
constexpr int kDefaultIntervalDays = 7;
constexpr int kMinimumIntervalDays = 1;

constexpr char kSchemaVersionKey[] = "schemaVersion";
constexpr char kUiKey[] = "ui";
constexpr char kStartupKey[] = "startup";
constexpr char kRemindersKey[] = "reminders";
constexpr char kThemeModeKey[] = "themeMode";
constexpr char kLanguageModeKey[] = "languageMode";
constexpr char kLaunchAtStartupKey[] = "launchAtStartup";
constexpr char kStartMinimizedToTrayKey[] = "startMinimizedToTray";
constexpr char kShowNotificationsKey[] = "showNotifications";
constexpr char kTestPrintKey[] = "testPrint";
constexpr char kHeadCleaningKey[] = "headCleaning";
constexpr char kEnabledKey[] = "enabled";
constexpr char kPreferredDayKey[] = "preferredDay";
constexpr char kIntervalDaysKey[] = "intervalDays";

QString themeModeToString(printercare::models::ThemeMode mode)
{
    using printercare::models::ThemeMode;

    switch (mode)
    {
    case ThemeMode::Light:
        return QStringLiteral("light");
    case ThemeMode::Dark:
        return QStringLiteral("dark");
    case ThemeMode::System:
    default:
        return QStringLiteral("system");
    }
}

printercare::models::ThemeMode themeModeFromString(const QString& value)
{
    using printercare::models::ThemeMode;

    if (value.compare(QStringLiteral("light"), Qt::CaseInsensitive) == 0)
    {
        return ThemeMode::Light;
    }

    if (value.compare(QStringLiteral("dark"), Qt::CaseInsensitive) == 0)
    {
        return ThemeMode::Dark;
    }

    return ThemeMode::System;
}

QString languageModeToString(printercare::models::LanguageMode mode)
{
    using printercare::models::LanguageMode;

    switch (mode)
    {
    case LanguageMode::English:
        return QStringLiteral("en");
    case LanguageMode::Ukrainian:
        return QStringLiteral("uk");
    case LanguageMode::Russian:
        return QStringLiteral("ru");
    case LanguageMode::System:
    default:
        return QStringLiteral("system");
    }
}

printercare::models::LanguageMode languageModeFromString(const QString& value)
{
    using printercare::models::LanguageMode;

    if (value.compare(QStringLiteral("en"), Qt::CaseInsensitive) == 0)
    {
        return LanguageMode::English;
    }

    if (value.compare(QStringLiteral("uk"), Qt::CaseInsensitive) == 0)
    {
        return LanguageMode::Ukrainian;
    }

    if (value.compare(QStringLiteral("ru"), Qt::CaseInsensitive) == 0)
    {
        return LanguageMode::Russian;
    }

    return LanguageMode::System;
}

int weekDayToInt(printercare::models::WeekDay day)
{
    return static_cast<int>(day);
}

printercare::models::WeekDay weekDayFromInt(int value)
{
    using printercare::models::WeekDay;

    switch (value)
    {
    case 1:
        return WeekDay::Monday;
    case 2:
        return WeekDay::Tuesday;
    case 3:
        return WeekDay::Wednesday;
    case 4:
        return WeekDay::Thursday;
    case 5:
        return WeekDay::Friday;
    case 6:
        return WeekDay::Saturday;
    case 7:
    default:
        return WeekDay::Sunday;
    }
}

QJsonObject reminderRuleToJson(const printercare::models::ReminderRuleSettings& rule)
{
    QJsonObject object;
    object.insert(kEnabledKey, rule.enabled);
    object.insert(kPreferredDayKey, weekDayToInt(rule.preferredDay));
    object.insert(kIntervalDaysKey, qMax(kMinimumIntervalDays, rule.intervalDays));
    return object;
}

printercare::models::ReminderRuleSettings reminderRuleFromJson(
    const QJsonObject& object,
    const printercare::models::ReminderRuleSettings& defaults)
{
    using printercare::models::ReminderRuleSettings;

    ReminderRuleSettings rule = defaults;
    rule.enabled = object.value(QLatin1StringView(kEnabledKey)).toBool(defaults.enabled);
    rule.preferredDay = weekDayFromInt(
        object.value(QLatin1StringView(kPreferredDayKey)).toInt(weekDayToInt(defaults.preferredDay)));
    rule.intervalDays = qMax(
        kMinimumIntervalDays,
        object.value(QLatin1StringView(kIntervalDaysKey)).toInt(defaults.intervalDays));
    return rule;
}

QJsonObject settingsToJson(const printercare::models::AppSettings& settings)
{
    QJsonObject root;
    root.insert(kSchemaVersionKey, kSchemaVersion);

    QJsonObject uiObject;
    uiObject.insert(kThemeModeKey, themeModeToString(settings.themeMode));
    uiObject.insert(kLanguageModeKey, languageModeToString(settings.languageMode));
    root.insert(kUiKey, uiObject);

    QJsonObject startupObject;
    startupObject.insert(kLaunchAtStartupKey, settings.launchAtStartup);
    startupObject.insert(kStartMinimizedToTrayKey, settings.startMinimizedToTray);
    startupObject.insert(kShowNotificationsKey, settings.showNotifications);
    root.insert(kStartupKey, startupObject);

    QJsonObject remindersObject;
    remindersObject.insert(kTestPrintKey, reminderRuleToJson(settings.testPrintRule));
    remindersObject.insert(kHeadCleaningKey, reminderRuleToJson(settings.headCleaningRule));
    root.insert(kRemindersKey, remindersObject);

    return root;
}

std::optional<printercare::models::AppSettings> settingsFromJson(
    const QJsonObject& root,
    QString* errorMessage)
{
    using printercare::models::AppSettings;

    const int schemaVersion = root.value(QLatin1StringView(kSchemaVersionKey)).toInt(kSchemaVersion);
    if (schemaVersion > kSchemaVersion)
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QObject::tr("Unsupported config schema version: %1").arg(schemaVersion);
        }
        return std::nullopt;
    }

    AppSettings settings;

    const QJsonObject uiObject = root.value(QLatin1StringView(kUiKey)).toObject();
    settings.themeMode = themeModeFromString(uiObject.value(QLatin1StringView(kThemeModeKey)).toString());
    settings.languageMode = languageModeFromString(uiObject.value(QLatin1StringView(kLanguageModeKey)).toString());

    const QJsonObject startupObject = root.value(QLatin1StringView(kStartupKey)).toObject();
    settings.launchAtStartup = startupObject.value(QLatin1StringView(kLaunchAtStartupKey)).toBool(settings.launchAtStartup);
    settings.startMinimizedToTray = startupObject.value(QLatin1StringView(kStartMinimizedToTrayKey)).toBool(settings.startMinimizedToTray);
    settings.showNotifications = startupObject.value(QLatin1StringView(kShowNotificationsKey)).toBool(settings.showNotifications);

    const QJsonObject remindersObject = root.value(QLatin1StringView(kRemindersKey)).toObject();
    settings.testPrintRule = reminderRuleFromJson(
        remindersObject.value(QLatin1StringView(kTestPrintKey)).toObject(),
        settings.testPrintRule);
    settings.headCleaningRule = reminderRuleFromJson(
        remindersObject.value(QLatin1StringView(kHeadCleaningKey)).toObject(),
        settings.headCleaningRule);

    return settings;
}
} // namespace

namespace printercare::managers
{
ConfigManager::ConfigManager(QString configFilePath)
    : configFilePath_(configFilePath.isEmpty() ? defaultConfigFilePath() : std::move(configFilePath))
{
}

const QString& ConfigManager::configFilePath() const noexcept
{
    return configFilePath_;
}

void ConfigManager::setConfigFilePath(QString configFilePath)
{
    configFilePath_ = configFilePath.isEmpty() ? defaultConfigFilePath() : std::move(configFilePath);
}

std::optional<printercare::models::AppSettings> ConfigManager::load(QString* errorMessage) const
{
    if (configFilePath_.isEmpty())
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QObject::tr("Config file path is empty.");
        }
        return std::nullopt;
    }

    QFile file(configFilePath_);
    if (!file.exists())
    {
        return printercare::models::AppSettings{};
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QObject::tr("Unable to open config file for reading: %1")
                                .arg(QDir::toNativeSeparators(configFilePath_));
        }
        return std::nullopt;
    }

    const QByteArray jsonData = file.readAll();
    QJsonParseError parseError{};
    const QJsonDocument document = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject())
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QObject::tr("Invalid JSON in config file: %1").arg(parseError.errorString());
        }
        return std::nullopt;
    }

    return settingsFromJson(document.object(), errorMessage);
}

bool ConfigManager::save(
    const printercare::models::AppSettings& settings,
    QString* errorMessage) const
{
    if (configFilePath_.isEmpty())
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QObject::tr("Config file path is empty.");
        }
        return false;
    }

    const QFileInfo fileInfo(configFilePath_);
    const QDir parentDir = fileInfo.dir();
    if (!parentDir.exists() && !QDir().mkpath(parentDir.absolutePath()))
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QObject::tr("Unable to create config directory: %1")
                                .arg(QDir::toNativeSeparators(parentDir.absolutePath()));
        }
        return false;
    }

    QSaveFile file(configFilePath_);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QObject::tr("Unable to open config file for writing: %1")
                                .arg(QDir::toNativeSeparators(configFilePath_));
        }
        return false;
    }

    const QJsonDocument document(settingsToJson(settings));
    if (file.write(document.toJson(QJsonDocument::Indented)) < 0)
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QObject::tr("Unable to write config file: %1")
                                .arg(QDir::toNativeSeparators(configFilePath_));
        }
        return false;
    }

    if (!file.commit())
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QObject::tr("Unable to finalize config file: %1")
                                .arg(QDir::toNativeSeparators(configFilePath_));
        }
        return false;
    }

    return true;
}

QString ConfigManager::defaultConfigFilePath()
{
    const QString configLocation = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (!configLocation.isEmpty())
    {
        return QDir(configLocation).filePath(QStringLiteral("settings.json"));
    }

    return QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("settings.json"));
}
} // namespace printercare::managers
