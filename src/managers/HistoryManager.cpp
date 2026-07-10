#include "managers/HistoryManager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QSaveFile>
#include <QStandardPaths>

#include <utility>

namespace
{
constexpr int kSchemaVersion = 1;
constexpr char kSchemaVersionKey[] = "schemaVersion";
constexpr char kRecordsKey[] = "records";
constexpr char kTypeKey[] = "type";
constexpr char kTimestampKey[] = "timestamp";
constexpr char kNoteKey[] = "note";

QString serviceTypeToString(printercare::managers::HistoryManager::ServiceType type)
{
    switch (type)
    {
    case printercare::managers::HistoryManager::ServiceType::HeadCleaning:
        return QStringLiteral("headCleaning");
    case printercare::managers::HistoryManager::ServiceType::TestPrint:
    default:
        return QStringLiteral("testPrint");
    }
}

std::optional<printercare::managers::HistoryManager::ServiceType> serviceTypeFromString(const QString& value)
{
    using ServiceType = printercare::managers::HistoryManager::ServiceType;

    if (value.compare(QStringLiteral("headCleaning"), Qt::CaseInsensitive) == 0)
    {
        return ServiceType::HeadCleaning;
    }

    if (value.compare(QStringLiteral("testPrint"), Qt::CaseInsensitive) == 0)
    {
        return ServiceType::TestPrint;
    }

    return std::nullopt;
}

QJsonObject recordToJson(const printercare::managers::HistoryManager::ServiceRecord& record)
{
    QJsonObject object;
    object.insert(kTypeKey, serviceTypeToString(record.type));
    object.insert(kTimestampKey, record.timestamp.toString(Qt::ISODateWithMs));
    object.insert(kNoteKey, record.note);
    return object;
}

std::optional<printercare::managers::HistoryManager::ServiceRecord> recordFromJson(const QJsonObject& object)
{
    using printercare::managers::HistoryManager;

    const auto type = serviceTypeFromString(object.value(QLatin1StringView(kTypeKey)).toString());
    const QString timestampText = object.value(QLatin1StringView(kTimestampKey)).toString();
    if (!type.has_value() || timestampText.isEmpty())
    {
        return std::nullopt;
    }

    const QDateTime timestamp = QDateTime::fromString(timestampText, Qt::ISODateWithMs);
    if (!timestamp.isValid())
    {
        return std::nullopt;
    }

    HistoryManager::ServiceRecord record;
    record.type = *type;
    record.timestamp = timestamp;
    record.note = object.value(QLatin1StringView(kNoteKey)).toString();
    return record;
}
} // namespace

namespace printercare::managers
{
HistoryManager::HistoryManager(QString historyFilePath)
    : historyFilePath_(historyFilePath.isEmpty() ? defaultHistoryFilePath() : std::move(historyFilePath))
{
    (void)load();
}

const QString& HistoryManager::historyFilePath() const noexcept
{
    return historyFilePath_;
}

void HistoryManager::setHistoryFilePath(QString historyFilePath)
{
    historyFilePath_ = historyFilePath.isEmpty() ? defaultHistoryFilePath() : std::move(historyFilePath);
}

QString HistoryManager::serviceTypeToString(ServiceType type)
{
    return ::serviceTypeToString(type);
}

std::optional<HistoryManager::ServiceType> HistoryManager::serviceTypeFromString(const QString& value)
{
    return ::serviceTypeFromString(value);
}

bool HistoryManager::load(QString* errorMessage)
{
    records_.clear();

    if (historyFilePath_.isEmpty())
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QObject::tr("History file path is empty.");
        }
        return false;
    }

    QFile file(historyFilePath_);
    if (!file.exists())
    {
        return true;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QObject::tr("Unable to open history file for reading: %1")
                                .arg(QDir::toNativeSeparators(historyFilePath_));
        }
        return false;
    }

    const QByteArray jsonData = file.readAll();
    QJsonParseError parseError{};
    const QJsonDocument document = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject())
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QObject::tr("Invalid JSON in history file: %1").arg(parseError.errorString());
        }
        return false;
    }

    const QJsonObject root = document.object();
    const int schemaVersion = root.value(QLatin1StringView(kSchemaVersionKey)).toInt(kSchemaVersion);
    if (schemaVersion > kSchemaVersion)
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QObject::tr("Unsupported history schema version: %1").arg(schemaVersion);
        }
        return false;
    }

    const QJsonArray recordsArray = root.value(QLatin1StringView(kRecordsKey)).toArray();
    for (const QJsonValue& value : recordsArray)
    {
        const QJsonObject recordObject = value.toObject();
        const auto record = recordFromJson(recordObject);
        if (record.has_value())
        {
            records_.push_back(*record);
        }
    }

    return true;
}

bool HistoryManager::save(QString* errorMessage) const
{
    if (historyFilePath_.isEmpty())
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QObject::tr("History file path is empty.");
        }
        return false;
    }

    const QFileInfo fileInfo(historyFilePath_);
    const QDir parentDir = fileInfo.dir();
    if (!parentDir.exists() && !QDir().mkpath(parentDir.absolutePath()))
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QObject::tr("Unable to create history directory: %1")
                                .arg(QDir::toNativeSeparators(parentDir.absolutePath()));
        }
        return false;
    }

    QJsonArray recordsArray;
    for (const ServiceRecord& record : records_)
    {
        recordsArray.append(recordToJson(record));
    }

    QJsonObject root;
    root.insert(kSchemaVersionKey, kSchemaVersion);
    root.insert(kRecordsKey, recordsArray);

    QSaveFile file(historyFilePath_);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QObject::tr("Unable to open history file for writing: %1")
                                .arg(QDir::toNativeSeparators(historyFilePath_));
        }
        return false;
    }

    const QJsonDocument document(root);
    if (file.write(document.toJson(QJsonDocument::Indented)) < 0)
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QObject::tr("Unable to write history file: %1")
                                .arg(QDir::toNativeSeparators(historyFilePath_));
        }
        return false;
    }

    if (!file.commit())
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QObject::tr("Unable to finalize history file: %1")
                                .arg(QDir::toNativeSeparators(historyFilePath_));
        }
        return false;
    }

    return true;
}

bool HistoryManager::record(ServiceType type, const QDateTime& timestamp, QString note)
{
    ServiceRecord record;
    record.type = type;
    record.timestamp = timestamp.isValid() ? timestamp : QDateTime::currentDateTime();
    record.note = std::move(note);

    records_.push_back(record);

    QString errorMessage;
    if (!save(&errorMessage))
    {
        records_.pop_back();
        return false;
    }

    return true;
}

std::optional<QDate> HistoryManager::lastServiceDate(ServiceType type) const noexcept
{
    for (auto it = records_.crbegin(); it != records_.crend(); ++it)
    {
        if (it->type == type && it->timestamp.isValid())
        {
            return it->timestamp.date();
        }
    }

    return std::nullopt;
}

QList<HistoryManager::ServiceRecord> HistoryManager::recentRecords(int limit) const
{
    if (limit <= 0 || records_.isEmpty())
    {
        return {};
    }

    const int startIndex = qMax(0, records_.size() - limit);
    QList<ServiceRecord> result;
    for (int index = records_.size() - 1; index >= startIndex; --index)
    {
        result.push_back(records_.at(index));
    }
    return result;
}

int HistoryManager::totalRecords() const noexcept
{
    return records_.size();
}

QString HistoryManager::defaultHistoryFilePath()
{
    const QString dataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!dataLocation.isEmpty())
    {
        return QDir(dataLocation).filePath(QStringLiteral("history.json"));
    }

    return QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("history.json"));
}
} // namespace printercare::managers
