#pragma once

#include "managers/HistoryManager.h"

#include <QMainWindow>

#include <memory>

class QShowEvent;
class QCloseEvent;

class MainWindowPrivate;
class SettingsWindow;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    [[nodiscard]] bool shouldStartHiddenOnLaunch() const noexcept;

protected:
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private slots:
    void showFromTray();

private:
    std::unique_ptr<MainWindowPrivate> d_;

    void configureWindow();
    void buildUi();
    void buildMenuBar();
    void applyTheme(bool darkTheme);
    void updateThemeActions() const;
    void loadInitialState();
    void saveCurrentSettings();
    void applyCurrentSettings(bool persistStartupIntegration = true);
    void refreshMaintenanceOverview();
    void scheduleReminderCheck();
    void checkReminders(bool allowNotifications = true);
    void recordServiceAction(printercare::managers::HistoryManager::ServiceType type);
    QString serviceTypeLabel(printercare::managers::HistoryManager::ServiceType type) const;
    void startIntroAnimation();
    void initializeTray();
    void showAboutDialog();
    void showSettingsWindow();
    void showHistoryWindow();
    void requestApplicationExit();
};
