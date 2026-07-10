#pragma once

#include <QDateTime>
#include <QList>
#include <QString>

#include <optional>

namespace printercare::managers
{
class HistoryManager final
{
public:
    enum class ServiceType
    {
        TestPrint,
        HeadCleaning
    };

    struct ServiceRecord
    {
        ServiceType type = ServiceType::TestPrint;
        QDateTime timestamp;
        QString note;
    };

    explicit HistoryManager(QString historyFilePath = {});

    [[nodiscard]] const QString& historyFilePath() const noexcept;
    void setHistoryFilePath(QString historyFilePath);

    [[nodiscard]] bool load(QString* errorMessage = nullptr);
    [[nodiscard]] bool save(QString* errorMessage = nullptr) const;

    [[nodiscard]] bool record(
        ServiceType type,
        const QDateTime& timestamp = QDateTime::currentDateTime(),
        QString note = {});

    [[nodiscard]] std::optional<QDate> lastServiceDate(ServiceType type) const noexcept;
    [[nodiscard]] QList<ServiceRecord> recentRecords(int limit = 10) const;
    [[nodiscard]] int totalRecords() const noexcept;
    [[nodiscard]] static QString defaultHistoryFilePath();

private:
    QString historyFilePath_;
    QList<ServiceRecord> records_;

    [[nodiscard]] static QString serviceTypeToString(ServiceType type);
    [[nodiscard]] static std::optional<ServiceType> serviceTypeFromString(const QString& value);
};
} // namespace printercare::managers
