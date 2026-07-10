#pragma once

#include "models/AppSettings.h"

#include <QHash>
#include <QTranslator>
#include <QString>

namespace printercare::localization
{
class AppTranslator final : public QTranslator
{
public:
    enum class UiLanguage
    {
        English,
        Ukrainian,
        Russian
    };

    explicit AppTranslator(printercare::models::LanguageMode mode);

    [[nodiscard]] bool isEnglish() const noexcept;

    [[nodiscard]] QString translate(
        const char* context,
        const char* sourceText,
        const char* disambiguation = nullptr,
        int n = -1) const override;

private:
    UiLanguage language_;
    QHash<QString, QString> translations_;

    void add(const char* sourceText, const char* translatedText);
    void buildTranslations();
};
} // namespace printercare::localization
