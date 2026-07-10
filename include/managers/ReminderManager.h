#pragma once

#include "models/AppSettings.h"

#include <QDate>

#include <optional>

namespace printercare::managers
{
class ReminderManager final
{
public:
    struct Evaluation
    {
        bool isEnabled = false;
        bool isScheduledToday = false;
        bool isDue = false;
        int daysSinceLastService = 0;
        int daysUntilDue = 0;
    };

    [[nodiscard]] static bool isScheduledDay(
        const QDate& date,
        printercare::models::WeekDay preferredDay) noexcept;

    [[nodiscard]] static int daysBetween(const QDate& from, const QDate& to) noexcept;

    [[nodiscard]] static bool isDue(
        const QDate& currentDate,
        const std::optional<QDate>& lastServiceDate,
        const printercare::models::ReminderRuleSettings& rule) noexcept;

    [[nodiscard]] static QDate nextScheduledDate(
        const QDate& currentDate,
        printercare::models::WeekDay preferredDay) noexcept;

    [[nodiscard]] static Evaluation evaluate(
        const QDate& currentDate,
        const std::optional<QDate>& lastServiceDate,
        const printercare::models::ReminderRuleSettings& rule) noexcept;

    [[nodiscard]] static QDate nextDueDate(
        const std::optional<QDate>& lastServiceDate,
        const printercare::models::ReminderRuleSettings& rule) noexcept;

private:
    static int normalizedIntervalDays(const printercare::models::ReminderRuleSettings& rule) noexcept;
};
} // namespace printercare::managers
