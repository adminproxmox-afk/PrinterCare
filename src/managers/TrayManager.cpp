#include "managers/TrayManager.h"

#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QSystemTrayIcon>

namespace printercare::managers
{
struct TrayManager::TrayManagerPrivate
{
    QSystemTrayIcon* trayIcon = nullptr;
    std::unique_ptr<QMenu> trayMenu;
    QAction* openAction = nullptr;
    QAction* testPrintAction = nullptr;
    QAction* headCleaningAction = nullptr;
    QAction* settingsAction = nullptr;
    QAction* historyAction = nullptr;
    QAction* aboutAction = nullptr;
    QAction* exitAction = nullptr;
    bool initialized = false;
};

TrayManager::TrayManager(QObject* parent)
    : QObject(parent)
    , d_(std::make_unique<TrayManagerPrivate>())
{
}

TrayManager::~TrayManager() = default;

bool TrayManager::initialize(const QIcon& icon, const QString& toolTip)
{
    if (!isSupported())
    {
        return false;
    }

    if (!d_->initialized)
    {
        d_->trayIcon = new QSystemTrayIcon(this);
        d_->trayMenu = std::make_unique<QMenu>();

        d_->openAction = d_->trayMenu->addAction(tr("Open"));
        d_->testPrintAction = d_->trayMenu->addAction(tr("Mark test print"));
        d_->headCleaningAction = d_->trayMenu->addAction(tr("Mark head cleaning"));
        d_->settingsAction = d_->trayMenu->addAction(tr("Settings..."));
        d_->historyAction = d_->trayMenu->addAction(tr("History"));
        d_->aboutAction = d_->trayMenu->addAction(tr("About"));
        d_->trayMenu->addSeparator();
        d_->exitAction = d_->trayMenu->addAction(tr("Exit"));

        d_->trayIcon->setContextMenu(d_->trayMenu.get());
        connectTrayIcon();
        buildMenu();
        d_->initialized = true;
    }

    d_->trayIcon->setIcon(icon);
    d_->trayIcon->setToolTip(toolTip.isEmpty() ? tr("Printer Care") : toolTip);
    d_->trayIcon->show();
    return true;
}

bool TrayManager::isSupported() const noexcept
{
    return QSystemTrayIcon::isSystemTrayAvailable();
}

void TrayManager::setVisible(bool visible)
{
    if (d_->trayIcon != nullptr)
    {
        d_->trayIcon->setVisible(visible);
    }
}

void TrayManager::setToolTip(const QString& toolTip)
{
    if (d_->trayIcon != nullptr)
    {
        d_->trayIcon->setToolTip(toolTip);
    }
}

void TrayManager::showMessage(
    const QString& title,
    const QString& message,
    QSystemTrayIcon::MessageIcon icon,
    int msecs)
{
    if (d_->trayIcon != nullptr)
    {
        d_->trayIcon->showMessage(title, message, icon, msecs);
    }
}

void TrayManager::buildMenu()
{
    connect(d_->openAction, &QAction::triggered, this, [this]()
    {
        emit openRequested();
    });

    connect(d_->testPrintAction, &QAction::triggered, this, [this]()
    {
        emit markTestPrintRequested();
    });

    connect(d_->headCleaningAction, &QAction::triggered, this, [this]()
    {
        emit markHeadCleaningRequested();
    });

    connect(d_->settingsAction, &QAction::triggered, this, [this]()
    {
        emit settingsRequested();
    });

    connect(d_->historyAction, &QAction::triggered, this, [this]()
    {
        emit historyRequested();
    });

    connect(d_->aboutAction, &QAction::triggered, this, [this]()
    {
        emit aboutRequested();
    });

    connect(d_->exitAction, &QAction::triggered, this, [this]()
    {
        emit exitRequested();
    });
}

void TrayManager::connectTrayIcon()
{
    connect(d_->trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason)
    {
        handleActivation(static_cast<int>(reason));
    });

    connect(d_->trayIcon, &QSystemTrayIcon::messageClicked, this, [this]()
    {
        emit notificationClicked();
    });
}

void TrayManager::handleActivation(int activationReason)
{
    const auto reason = static_cast<QSystemTrayIcon::ActivationReason>(activationReason);
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        emit showRequested();
        break;
    case QSystemTrayIcon::Context:
    case QSystemTrayIcon::MiddleClick:
    case QSystemTrayIcon::Unknown:
    default:
        break;
    }
}
} // namespace printercare::managers
