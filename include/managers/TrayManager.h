#pragma once

#include <QObject>
#include <QString>
#include <QSystemTrayIcon>

#include <memory>

class QIcon;

namespace printercare::managers
{
class TrayManager final : public QObject
{
    Q_OBJECT

public:
    explicit TrayManager(QObject* parent = nullptr);
    ~TrayManager() override;

    [[nodiscard]] bool initialize(const QIcon& icon, const QString& toolTip = {});
    [[nodiscard]] bool isSupported() const noexcept;

    void setVisible(bool visible);
    void setToolTip(const QString& toolTip);
    void showMessage(
        const QString& title,
        const QString& message,
        QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information,
        int msecs = 10000);

signals:
    void openRequested();
    void markTestPrintRequested();
    void markHeadCleaningRequested();
    void historyRequested();
    void aboutRequested();
    void showRequested();
    void settingsRequested();
    void exitRequested();
    void notificationClicked();

private:
    struct TrayManagerPrivate;
    std::unique_ptr<TrayManagerPrivate> d_;

    void buildMenu();
    void connectTrayIcon();
    void handleActivation(int activationReason);
};
} // namespace printercare::managers
