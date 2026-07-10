#include "managers/ReminderManager.h"

#include <algorithm>

namespace
{
int weekDayIndex(printercare::models::WeekDay day) noexcept
{
    return static_cast<int>(day);
}

bool isValidDateRange(const QDate& from, const QDate& to) noexcept
{
    return from.isValid() && to.isValid() && from <= to;
}

int preferredDayIndex(printercare::models::WeekDay day) noexcept
{
    return static_cast<int>(day);
}
} // namespace

namespace printercare::managers
{
int ReminderManager::normalizedIntervalDays(const printercare::models::ReminderRuleSettings& rule) noexcept
{
    return std::max(1, rule.intervalDays);
}

bool ReminderManager::isScheduledDay(const QDate& date, printercare::models::WeekDay preferredDay) noexcept
{
    if (!date.isValid())
    {
        return false;
    }

    return date.dayOfWeek() == preferredDayIndex(preferredDay);
}

int ReminderManager::daysBetween(const QDate& from, const QDate& to) noexcept
{
    if (!isValidDateRange(from, to))
    {
        return 0;
    }

    return from.daysTo(to);
}

bool ReminderManager::isDue(
    const QDate& currentDate,
    const std::optional<QDate>& lastServiceDate,
    const printercare::models::ReminderRuleSettings& rule) noexcept
{
    return evaluate(currentDate, lastServiceDate, rule).isDue;
}

QDate ReminderManager::nextScheduledDate(
    const QDate& currentDate,
    printercare::models::WeekDay preferredDay) noexcept
{
    if (!currentDate.isValid())
    {
        return {};
    }

    const int currentDayIndex = currentDate.dayOfWeek();
    const int targetDayIndex = preferredDayIndex(preferredDay);
    const int daysUntilPreferredDay = (targetDayIndex - currentDayIndex + 7) % 7;
    return currentDate.addDays(daysUntilPreferredDay);
}

ReminderManager::Evaluation ReminderManager::evaluate(
    const QDate& currentDate,
    const std::optional<QDate>& lastServiceDate,
    const printercare::models::ReminderRuleSettings& rule) noexcept
{
    Evaluation evaluation{};
    evaluation.isEnabled = rule.enabled;

    if (!evaluation.isEnabled || !currentDate.isValid())
    {
        return evaluation;
    }

    evaluation.isScheduledToday = isScheduledDay(currentDate, rule.preferredDay);
    const QDate dueDate = nextDueDate(lastServiceDate, rule);

    if (!lastServiceDate.has_value() || !lastServiceDate->isValid())
    {
        evaluation.daysSinceLastService = 0;
        evaluation.daysUntilDue = std::max(
            0,
            static_cast<int>(currentDate.daysTo(nextScheduledDate(currentDate, rule.preferredDay))));
        evaluation.isDue = evaluation.isScheduledToday;
        return evaluation;
    }

    evaluation.daysSinceLastService = lastServiceDate->daysTo(currentDate);
    if (evaluation.daysSinceLastService < 0)
    {
        return evaluation;
    }

    evaluation.daysUntilDue = std::max(0, static_cast<int>(currentDate.daysTo(dueDate)));
    evaluation.isDue = currentDate >= dueDate;
    return evaluation;
}

QDate ReminderManager::nextDueDate(
    const std::optional<QDate>& lastServiceDate,
    const printercare::models::ReminderRuleSettings& rule) noexcept
{
    if (!lastServiceDate.has_value() || !lastServiceDate->isValid())
    {
        return nextScheduledDate(QDate::currentDate(), rule.preferredDay);
    }

    return lastServiceDate->addDays(normalizedIntervalDays(rule));
}
} // namespace printercare::managers
