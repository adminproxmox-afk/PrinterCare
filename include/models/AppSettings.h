#pragma once

namespace printercare::models
{
enum class ThemeMode
{
    System,
    Light,
    Dark
};

enum class LanguageMode
{
    System,
    English,
    Ukrainian,
    Russian
};

enum class WeekDay
{
    Monday = 1,
    Tuesday = 2,
    Wednesday = 3,
    Thursday = 4,
    Friday = 5,
    Saturday = 6,
    Sunday = 7
};

struct ReminderRuleSettings
{
    bool enabled = true;
    WeekDay preferredDay = WeekDay::Sunday;
    int intervalDays = 7;
};

struct AppSettings
{
    ThemeMode themeMode = ThemeMode::System;
    LanguageMode languageMode = LanguageMode::System;
    bool launchAtStartup = true;
    bool startMinimizedToTray = true;
    bool showNotifications = true;
    ReminderRuleSettings testPrintRule{};
    ReminderRuleSettings headCleaningRule{};
};
} // namespace printercare::models
