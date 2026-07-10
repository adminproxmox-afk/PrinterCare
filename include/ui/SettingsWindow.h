#pragma once

#include "models/AppSettings.h"

#include <QDialog>

#include <memory>

class QShowEvent;

class SettingsWindowPrivate;

class SettingsWindow final : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget* parent = nullptr);
    explicit SettingsWindow(const printercare::models::AppSettings& settings, QWidget* parent = nullptr);
    ~SettingsWindow() override;

    void setSettings(const printercare::models::AppSettings& settings);
    [[nodiscard]] printercare::models::AppSettings settings() const;

signals:
    void settingsSaved(const printercare::models::AppSettings& settings);

protected:
    void showEvent(QShowEvent* event) override;

private:
    std::unique_ptr<SettingsWindowPrivate> d_;

    void configureWindow();
    void buildUi();
    void buildNavigation();
    void buildPages();
    void connectSignals();
    void applySettingsToUi(const printercare::models::AppSettings& settings);
    [[nodiscard]] printercare::models::AppSettings readSettingsFromUi() const;
    void persistCurrentSettings();
    void updatePreview();
    void restoreDefaults();
    void selectPage(int index);
};
