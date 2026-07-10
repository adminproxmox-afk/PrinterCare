#include "ui/MainWindow.h"
#include "ui/SettingsWindow.h"
#include "ui/UiDecor.h"

#include "core/AppInfo.h"
#include "managers/ConfigManager.h"
#include "managers/HistoryManager.h"
#include "managers/ReminderManager.h"
#include "managers/StartupManager.h"
#include "managers/TrayManager.h"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QAbstractAnimation>
#include <QColor>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QDate>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QIcon>
#include <QKeySequence>
#include <QLabel>
#include <QMessageBox>
#include <QGridLayout>
#include <QMenuBar>
#include <QMenu>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLocale>
#include <QPalette>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QPixmap>
#include <QEasingCurve>
#include <QScrollArea>
#include <QSize>
#include <QSizePolicy>
#include <QTime>
#include <QShowEvent>
#include <QFrame>
#include <QSystemTrayIcon>
#include <QToolButton>
#include <QStandardPaths>
#include <QTimer>
#include <QVBoxLayout>

namespace
{
enum class Theme
{
    Light,
    Dark
};

QString loadTextResource(const QString& resourcePath)
{
    QFile file(resourcePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return {};
    }

    return QString::fromUtf8(file.readAll());
}

QPalette buildPalette(Theme theme)
{
    QPalette palette;

    if (theme == Theme::Dark)
    {
        palette.setColor(QPalette::Window, QColor(QStringLiteral("#11151F")));
        palette.setColor(QPalette::Base, QColor(QStringLiteral("#171C28")));
        palette.setColor(QPalette::AlternateBase, QColor(QStringLiteral("#1D2432")));
        palette.setColor(QPalette::Button, QColor(QStringLiteral("#1A2130")));
        palette.setColor(QPalette::Text, QColor(QStringLiteral("#F4F7FB")));
        palette.setColor(QPalette::WindowText, QColor(QStringLiteral("#F4F7FB")));
        palette.setColor(QPalette::ButtonText, QColor(QStringLiteral("#F4F7FB")));
        palette.setColor(QPalette::ToolTipBase, QColor(QStringLiteral("#F4F7FB")));
        palette.setColor(QPalette::ToolTipText, QColor(QStringLiteral("#11151F")));
        palette.setColor(QPalette::Highlight, QColor(QStringLiteral("#4F8CFF")));
        palette.setColor(QPalette::HighlightedText, QColor(QStringLiteral("#FFFFFF")));
        palette.setColor(QPalette::Link, QColor(QStringLiteral("#8AB4FF")));
    }
    else
    {
        palette.setColor(QPalette::Window, QColor(QStringLiteral("#F3F5F8")));
        palette.setColor(QPalette::Base, QColor(QStringLiteral("#FFFFFF")));
        palette.setColor(QPalette::AlternateBase, QColor(QStringLiteral("#EEF2F7")));
        palette.setColor(QPalette::Button, QColor(QStringLiteral("#FFFFFF")));
        palette.setColor(QPalette::Text, QColor(QStringLiteral("#172033")));
        palette.setColor(QPalette::WindowText, QColor(QStringLiteral("#172033")));
        palette.setColor(QPalette::ButtonText, QColor(QStringLiteral("#172033")));
        palette.setColor(QPalette::ToolTipBase, QColor(QStringLiteral("#172033")));
        palette.setColor(QPalette::ToolTipText, QColor(QStringLiteral("#FFFFFF")));
        palette.setColor(QPalette::Highlight, QColor(QStringLiteral("#2264FF")));
        palette.setColor(QPalette::HighlightedText, QColor(QStringLiteral("#FFFFFF")));
        palette.setColor(QPalette::Link, QColor(QStringLiteral("#2264FF")));
    }

    return palette;
}

QFrame* createMetricCard(
    QWidget* parent,
    const QString& iconResource,
    const QString& badge,
    const QString& title,
    const QString& description)
{
    auto* card = new QFrame(parent);
    card->setObjectName(QStringLiteral("MetricCard"));

    auto* layout = new QHBoxLayout(card);
    layout->setContentsMargins(
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding);
    layout->setSpacing(printercare::app::kCardSpacing);

    auto* accentColumn = new QVBoxLayout();
    accentColumn->setSpacing(10);
    accentColumn->setAlignment(Qt::AlignTop);

    if (!iconResource.isEmpty())
    {
        accentColumn->addWidget(printercare::ui::createIconBadge(card, iconResource, QStringLiteral("MetricIcon"), 54, 28), 0, Qt::AlignHCenter);
    }

    auto* badgeLabel = new QLabel(badge, card);
    badgeLabel->setObjectName(QStringLiteral("MetricBadge"));
    badgeLabel->setAlignment(Qt::AlignCenter);
    badgeLabel->setFixedSize(48, 48);
    accentColumn->addWidget(badgeLabel, 0, Qt::AlignHCenter);

    auto* textColumn = new QVBoxLayout();
    textColumn->setSpacing(6);

    auto* titleLabel = new QLabel(title, card);
    titleLabel->setObjectName(QStringLiteral("MetricTitle"));

    auto* descriptionLabel = new QLabel(description, card);
    descriptionLabel->setObjectName(QStringLiteral("MetricDescription"));
    descriptionLabel->setWordWrap(true);

    textColumn->addWidget(titleLabel);
    textColumn->addWidget(descriptionLabel);

    layout->addLayout(accentColumn, 0);
    layout->addLayout(textColumn, 1);
    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    card->setMinimumHeight(132);

    return card;
}

QFrame* createServiceStatusRow(
    QWidget* parent,
    const QString& iconResource,
    const QString& title,
    const QString& summary,
    QLabel*& valueLabel)
{
    auto* row = new QFrame(parent);
    row->setObjectName(QStringLiteral("ServiceStatusRow"));

    auto* layout = new QHBoxLayout(row);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(14);

    layout->addWidget(printercare::ui::createIconBadge(row, iconResource, QStringLiteral("ServiceRowIcon"), 38, 20));

    auto* textColumn = new QVBoxLayout();
    textColumn->setSpacing(4);

    auto* titleLabel = new QLabel(title, row);
    titleLabel->setObjectName(QStringLiteral("ServiceRowTitle"));

    auto* summaryLabel = new QLabel(summary, row);
    summaryLabel->setObjectName(QStringLiteral("ServiceRowHint"));
    summaryLabel->setWordWrap(true);

    textColumn->addWidget(titleLabel);
    textColumn->addWidget(summaryLabel);

    valueLabel = new QLabel(QObject::tr("Pending"), row);
    valueLabel->setObjectName(QStringLiteral("ServiceRowValue"));
    valueLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    valueLabel->setMinimumWidth(220);
    valueLabel->setMaximumWidth(320);
    valueLabel->setWordWrap(true);

    layout->addLayout(textColumn, 1);
    layout->addWidget(valueLabel, 0);
    row->setMinimumHeight(88);
    return row;
}

QFrame* createSignalRow(QWidget* parent, const QString& text)
{
    auto* row = new QFrame(parent);
    row->setObjectName(QStringLiteral("SignalRow"));

    auto* layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    layout->addWidget(printercare::ui::createIconBadge(
        row,
        QStringLiteral(":/icons/check.svg"),
        QStringLiteral("SignalMark"),
        24,
        14));

    auto* label = new QLabel(text, row);
    label->setWordWrap(true);
    label->setObjectName(QStringLiteral("SignalText"));
    layout->addWidget(label, 1);
    return row;
}

QFrame* createStatTile(
    QWidget* parent,
    const QString& iconResource,
    const QString& title,
    const QString& value,
    const QString& caption,
    QLabel*& valueLabel)
{
    auto* tile = new QFrame(parent);
    tile->setObjectName(QStringLiteral("StatusTile"));

    auto* layout = new QVBoxLayout(tile);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(10);

    layout->addWidget(printercare::ui::createIconBadge(
        tile,
        iconResource,
        QStringLiteral("StatusTileIcon"),
        34,
        18));

    auto* titleLabel = new QLabel(title, tile);
    titleLabel->setObjectName(QStringLiteral("StatusTileTitle"));
    valueLabel = new QLabel(value, tile);
    valueLabel->setObjectName(QStringLiteral("StatusTileValue"));
    valueLabel->setWordWrap(true);
    auto* captionLabel = new QLabel(caption, tile);
    captionLabel->setObjectName(QStringLiteral("StatusTileCaption"));
    captionLabel->setWordWrap(true);

    layout->addWidget(titleLabel);
    layout->addWidget(valueLabel);
    layout->addWidget(captionLabel);
    layout->addStretch(1);

    return tile;
}

QString themeStylePath(Theme theme)
{
    return theme == Theme::Dark
        ? QStringLiteral(":/styles/dark.qss")
        : QStringLiteral(":/styles/light.qss");
}

QString serviceTypeText(printercare::managers::HistoryManager::ServiceType type)
{
    switch (type)
    {
    case printercare::managers::HistoryManager::ServiceType::HeadCleaning:
        return QObject::tr("Head cleaning");
    case printercare::managers::HistoryManager::ServiceType::TestPrint:
    default:
        return QObject::tr("Test print");
    }
}

QString formatDate(const QDate& date)
{
    if (!date.isValid())
    {
        return QObject::tr("Not yet recorded");
    }

    return QLocale().toString(date, QLocale::LongFormat);
}

QDateTime startOfDay(const QDate& date)
{
    if (!date.isValid())
    {
        return {};
    }

    return QDateTime(date, QTime(0, 0));
}
} // namespace

struct MainWindowPrivate
{
    Theme theme = Theme::Light;
    bool introStarted = false;

    QWidget* rootWidget = nullptr;
    QFrame* shellCard = nullptr;
    QListWidget* sidebarNavigation = nullptr;
    QFrame* heroCard = nullptr;
    QToolButton* themeToggleButton = nullptr;
    QAction* lightThemeAction = nullptr;
    QAction* darkThemeAction = nullptr;
    QAction* settingsAction = nullptr;
    QFrame* maintenanceCard = nullptr;
    QLabel* statusChip = nullptr;
    QLabel* testPrintStatusValue = nullptr;
    QLabel* headCleaningStatusValue = nullptr;
    QLabel* historySummaryValue = nullptr;
    QLabel* historyPathsValue = nullptr;
    QListWidget* historyList = nullptr;
    QPushButton* logTestPrintButton = nullptr;
    QPushButton* logHeadCleaningButton = nullptr;
    QPushButton* openSettingsButton = nullptr;
    QPushButton* refreshButton = nullptr;
    printercare::models::AppSettings settings{};
    printercare::managers::ConfigManager configManager{};
    printercare::managers::HistoryManager historyManager{};
    std::unique_ptr<printercare::managers::TrayManager> trayManager;
    QTimer reminderTimer;
    bool allowClose = false;
    bool trayReady = false;
    bool startHiddenOnLaunch = false;
    QDate lastTestPrintNotificationDate;
    QDate lastHeadCleaningNotificationDate;
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , d_(std::make_unique<MainWindowPrivate>())
{
    setWindowOpacity(0.0);
    configureWindow();
    buildMenuBar();
    buildUi();
    connect(&d_->reminderTimer, &QTimer::timeout, this, [this]()
    {
        checkReminders(true);
        scheduleReminderCheck();
    });
    loadInitialState();
    initializeTray();
}

MainWindow::~MainWindow() = default;

bool MainWindow::shouldStartHiddenOnLaunch() const noexcept
{
    return d_->startHiddenOnLaunch;
}

void MainWindow::configureWindow()
{
    setObjectName(QStringLiteral("MainWindow"));
    setWindowTitle(QString::fromLatin1(printercare::app::kApplicationName));
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setMinimumSize(
        printercare::app::kMinimumWindowWidth,
        printercare::app::kMinimumWindowHeight);
    setWindowIcon(QIcon(QStringLiteral(":/icons/app.svg")));
}

void MainWindow::buildMenuBar()
{
    auto* fileMenu = menuBar()->addMenu(tr("&File"));

    auto* exitAction = fileMenu->addAction(tr("E&xit"));
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &MainWindow::requestApplicationExit);

    auto* viewMenu = menuBar()->addMenu(tr("&View"));

    auto* themeGroup = new QActionGroup(this);
    themeGroup->setExclusive(true);

    d_->lightThemeAction = viewMenu->addAction(tr("&Light theme"));
    d_->lightThemeAction->setCheckable(true);
    themeGroup->addAction(d_->lightThemeAction);

    d_->darkThemeAction = viewMenu->addAction(tr("&Dark theme"));
    d_->darkThemeAction->setCheckable(true);
    themeGroup->addAction(d_->darkThemeAction);

    viewMenu->addSeparator();

    auto* toggleThemeAction = viewMenu->addAction(tr("&Toggle theme"));
    toggleThemeAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+T")));

    connect(d_->lightThemeAction, &QAction::triggered, this, [this]()
    {
        d_->settings.themeMode = printercare::models::ThemeMode::Light;
        applyCurrentSettings();
    });

    connect(d_->darkThemeAction, &QAction::triggered, this, [this]()
    {
        d_->settings.themeMode = printercare::models::ThemeMode::Dark;
        applyCurrentSettings();
    });

    connect(toggleThemeAction, &QAction::triggered, this, [this]()
    {
        d_->settings.themeMode = (d_->theme == Theme::Light)
            ? printercare::models::ThemeMode::Dark
            : printercare::models::ThemeMode::Light;
        applyCurrentSettings();
    });

    auto* toolsMenu = menuBar()->addMenu(tr("&Tools"));
    d_->settingsAction = toolsMenu->addAction(tr("&Settings..."));
    d_->settingsAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+,")));
    connect(d_->settingsAction, &QAction::triggered, this, &MainWindow::showSettingsWindow);

    auto* helpMenu = menuBar()->addMenu(tr("&Help"));

    auto* aboutAction = helpMenu->addAction(tr("&About Printer Care"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);
}

void MainWindow::buildUi()
{
    auto* rootWidget = new QWidget(this);
    rootWidget->setObjectName(QStringLiteral("MainWindowRoot"));
    setCentralWidget(rootWidget);
    d_->rootWidget = rootWidget;

    auto* rootLayout = new QVBoxLayout(rootWidget);
    rootLayout->setContentsMargins(
        printercare::app::kWindowMargin,
        printercare::app::kWindowMargin,
        printercare::app::kWindowMargin,
        printercare::app::kWindowMargin);
    rootLayout->setSpacing(printercare::app::kWindowSectionSpacing);

    auto* shellCard = new QFrame(rootWidget);
    shellCard->setObjectName(QStringLiteral("ShellCard"));
    shellCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d_->shellCard = shellCard;

    auto* shellShadow = new QGraphicsDropShadowEffect(shellCard);
    shellShadow->setBlurRadius(36.0);
    shellShadow->setOffset(0.0, 12.0);
    shellShadow->setColor(QColor(0, 0, 0, 56));
    shellCard->setGraphicsEffect(shellShadow);

    auto* shellLayout = new QHBoxLayout(shellCard);
    shellLayout->setContentsMargins(
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding);
    shellLayout->setSpacing(printercare::app::kWindowSectionSpacing);

    auto setupCardShadow = [](QWidget* card, int blurRadius, qreal offsetY, const QColor& color)
    {
        auto* shadow = new QGraphicsDropShadowEffect(card);
        shadow->setBlurRadius(blurRadius);
        shadow->setOffset(0.0, offsetY);
        shadow->setColor(color);
        card->setGraphicsEffect(shadow);
    };

    auto* sidebarCard = new QFrame(shellCard);
    sidebarCard->setObjectName(QStringLiteral("SidebarPanel"));
    sidebarCard->setMinimumWidth(288);
    sidebarCard->setMaximumWidth(332);
    sidebarCard->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    setupCardShadow(sidebarCard, 28, 10, QColor(0, 0, 0, 44));

    auto* sidebarLayout = new QVBoxLayout(sidebarCard);
    sidebarLayout->setContentsMargins(
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding);
    sidebarLayout->setSpacing(18);

    auto* brandRow = new QHBoxLayout();
    brandRow->setSpacing(16);

    auto* logoLabel = new QLabel(sidebarCard);
    logoLabel->setObjectName(QStringLiteral("AppLogo"));
    logoLabel->setFixedSize(printercare::app::kHeroLogoSize, printercare::app::kHeroLogoSize);
    logoLabel->setAlignment(Qt::AlignCenter);

    QPixmap logoPixmap(QStringLiteral(":/icons/app.svg"));
    if (!logoPixmap.isNull())
    {
        logoLabel->setPixmap(logoPixmap.scaled(
            printercare::app::kHeroLogoSize,
            printercare::app::kHeroLogoSize,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation));
    }
    else
    {
        logoLabel->setText(QStringLiteral("PC"));
    }

    auto* brandText = new QVBoxLayout();
    brandText->setSpacing(4);

    auto* appNameLabel = new QLabel(QString::fromLatin1(printercare::app::kApplicationName), sidebarCard);
    appNameLabel->setObjectName(QStringLiteral("SidebarBrandName"));

    auto* appSubtitleLabel = new QLabel(
        tr("Reminder shell for inkjet maintenance, designed for the tray and the desktop."),
        sidebarCard);
    appSubtitleLabel->setObjectName(QStringLiteral("SidebarBrandText"));
    appSubtitleLabel->setWordWrap(true);

    brandText->addWidget(appNameLabel);
    brandText->addWidget(appSubtitleLabel);
    brandRow->addWidget(logoLabel, 0, Qt::AlignTop);
    brandRow->addLayout(brandText, 1);
    sidebarLayout->addLayout(brandRow);

    d_->sidebarNavigation = new QListWidget(sidebarCard);
    d_->sidebarNavigation->setObjectName(QStringLiteral("SidebarNavigation"));
    d_->sidebarNavigation->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d_->sidebarNavigation->setSelectionMode(QAbstractItemView::SingleSelection);
    d_->sidebarNavigation->setFocusPolicy(Qt::StrongFocus);
    d_->sidebarNavigation->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_->sidebarNavigation->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_->sidebarNavigation->setFrameShape(QFrame::NoFrame);
    d_->sidebarNavigation->setSpacing(8);

    const struct NavItem
    {
        const char* text;
        const char* icon;
        const char* action;
    };

    const std::array<NavItem, 8> navItems{{
        { "LIVE DASHBOARD", ":/icons/dashboard.svg", "dashboard" },
        { "Tray", ":/icons/tray.svg", "tray" },
        { "Reminders", ":/icons/reminder.svg", "reminders" },
        { "History", ":/icons/history.svg", "history" },
        { "Checks", ":/icons/reminder.svg", "checks" },
        { "Local log", ":/icons/history.svg", "journal" },
        { "Settings", ":/icons/settings.svg", "settings" },
        { "About build", ":/icons/app.svg", "about" }
    }};

    for (const auto& navItem : navItems)
    {
        auto* item = new QListWidgetItem(QIcon(QString::fromLatin1(navItem.icon)), tr(navItem.text), d_->sidebarNavigation);
        item->setData(Qt::UserRole, QString::fromLatin1(navItem.action));
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        item->setSizeHint(QSize(0, 52));
    }
    d_->sidebarNavigation->setCurrentRow(0);

    connect(d_->sidebarNavigation, &QListWidget::itemClicked, this, [this](QListWidgetItem* item)
    {
        if (item == nullptr)
        {
            return;
        }

        const QString action = item->data(Qt::UserRole).toString();
        if (action == QStringLiteral("settings"))
        {
            showSettingsWindow();
        }
        else if (action == QStringLiteral("history"))
        {
            showHistoryWindow();
        }
        else if (action == QStringLiteral("about"))
        {
            showAboutDialog();
        }
        else if (action == QStringLiteral("checks"))
        {
            checkReminders(false);
        }
        else if (action == QStringLiteral("tray"))
        {
            showFromTray();
        }
    });

    sidebarLayout->addWidget(d_->sidebarNavigation, 1);

    auto* sidebarFooter = new QVBoxLayout();
    sidebarFooter->setSpacing(10);

    d_->themeToggleButton = new QToolButton(sidebarCard);
    d_->themeToggleButton->setObjectName(QStringLiteral("ThemeToggleButton"));
    d_->themeToggleButton->setIcon(QIcon(QStringLiteral(":/icons/appearance.svg")));
    d_->themeToggleButton->setIconSize(QSize(18, 18));
    d_->themeToggleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    d_->themeToggleButton->setAutoRaise(false);
    d_->themeToggleButton->setCursor(Qt::PointingHandCursor);
    connect(d_->themeToggleButton, &QToolButton::clicked, this, [this]()
    {
        d_->settings.themeMode = (d_->theme == Theme::Light)
            ? printercare::models::ThemeMode::Dark
            : printercare::models::ThemeMode::Light;
        applyCurrentSettings();
    });

    sidebarFooter->addWidget(d_->themeToggleButton);
    sidebarLayout->addLayout(sidebarFooter);

    auto* sidebarCopyright = new QLabel(tr("C++20 | Qt 6 | CMake"), sidebarCard);
    sidebarCopyright->setObjectName(QStringLiteral("SidebarCopyright"));
    sidebarCopyright->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    sidebarLayout->addWidget(sidebarCopyright);

    auto* contentScroll = new QScrollArea(shellCard);
    contentScroll->setObjectName(QStringLiteral("DashboardScroll"));
    contentScroll->setWidgetResizable(true);
    contentScroll->setFrameShape(QFrame::NoFrame);
    contentScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto* contentHost = new QWidget(contentScroll);
    contentHost->setObjectName(QStringLiteral("DashboardContent"));
    contentScroll->setWidget(contentHost);

    auto* contentLayout = new QVBoxLayout(contentHost);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(printercare::app::kWindowSectionSpacing);

    auto* headerBlock = new QVBoxLayout();
    headerBlock->setSpacing(10);

    auto* headerTitle = new QLabel(tr("LIVE DASHBOARD"), contentHost);
    headerTitle->setObjectName(QStringLiteral("DashboardTitle"));
    headerTitle->setWordWrap(true);

    auto* headerStatusRow = new QHBoxLayout();
    headerStatusRow->setSpacing(12);

    auto* statusTray = new QLabel(tr("Tray active"), contentHost);
    statusTray->setObjectName(QStringLiteral("HeaderStatusText"));
    auto* statusDot1 = new QLabel(contentHost);
    statusDot1->setObjectName(QStringLiteral("HeaderStatusDot"));
    statusDot1->setFixedSize(6, 6);
    auto* statusChecks = new QLabel(tr("Checks on"), contentHost);
    statusChecks->setObjectName(QStringLiteral("HeaderStatusText"));
    auto* statusDot2 = new QLabel(contentHost);
    statusDot2->setObjectName(QStringLiteral("HeaderStatusDot"));
    statusDot2->setFixedSize(6, 6);
    auto* statusLog = new QLabel(tr("Local log"), contentHost);
    statusLog->setObjectName(QStringLiteral("HeaderStatusText"));

    headerStatusRow->addWidget(statusTray);
    headerStatusRow->addWidget(statusDot1);
    headerStatusRow->addWidget(statusChecks);
    headerStatusRow->addWidget(statusDot2);
    headerStatusRow->addWidget(statusLog);
    headerStatusRow->addStretch(1);

    headerBlock->addWidget(headerTitle);
    headerBlock->addLayout(headerStatusRow);
    contentLayout->addLayout(headerBlock);

    auto* dashboardGrid = new QGridLayout();
    dashboardGrid->setHorizontalSpacing(printercare::app::kWindowSectionSpacing);
    dashboardGrid->setVerticalSpacing(printercare::app::kWindowSectionSpacing);
    dashboardGrid->setColumnStretch(0, 1);
    dashboardGrid->setColumnStretch(1, 1);

    auto* summaryCard = new QFrame(contentHost);
    summaryCard->setObjectName(QStringLiteral("SummaryCard"));
    summaryCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setupCardShadow(summaryCard, 24, 8, QColor(0, 0, 0, 32));

    auto* summaryLayout = new QVBoxLayout(summaryCard);
    summaryLayout->setContentsMargins(
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding);
    summaryLayout->setSpacing(14);

    auto* summaryTop = new QHBoxLayout();
    summaryTop->setSpacing(14);
    summaryTop->addWidget(printercare::ui::createIconBadge(
        summaryCard,
        QStringLiteral(":/icons/app.svg"),
        QStringLiteral("SummaryIcon"),
        66,
        34),
        0,
        Qt::AlignTop);

    auto* summaryText = new QVBoxLayout();
    summaryText->setSpacing(5);

    auto* summaryPrefix = new QLabel(tr("PC"), summaryCard);
    summaryPrefix->setObjectName(QStringLiteral("SummaryPrefix"));
    auto* summaryTitle = new QLabel(QString::fromLatin1(printercare::app::kApplicationName), summaryCard);
    summaryTitle->setObjectName(QStringLiteral("CardTitle"));

    auto* summaryDescription = new QLabel(
<<<<<<< HEAD
        tr("Reminder shell for inkjet maintenance, designed for the tray and the desktop."),
=======
        tr("Tray-first printer care with reminders, history, and maintenance actions in one surface."),
>>>>>>> 2a89a04941611fe5ae80dcc4f741cbfb21fd10f6
        summaryCard);
    summaryDescription->setObjectName(QStringLiteral("CardDescription"));
    summaryDescription->setWordWrap(true);

    summaryText->addWidget(summaryPrefix);
    summaryText->addWidget(summaryTitle);
    summaryText->addWidget(summaryDescription);
    summaryTop->addLayout(summaryText, 1);
    summaryLayout->addLayout(summaryTop);

    auto* summaryFooter = new QHBoxLayout();
    summaryFooter->setSpacing(12);
    auto* summaryFooterLabel = new QLabel(tr("Tray ready"), summaryCard);
    summaryFooterLabel->setObjectName(QStringLiteral("SummaryFooterText"));

    d_->statusChip = new QLabel(tr("Active"), summaryCard);
    d_->statusChip->setObjectName(QStringLiteral("StatusChip"));
    d_->statusChip->setAlignment(Qt::AlignCenter);
    summaryFooter->addWidget(summaryFooterLabel);
    summaryFooter->addStretch(1);
    summaryFooter->addWidget(d_->statusChip);
    summaryLayout->addStretch(1);
    summaryLayout->addLayout(summaryFooter);

    d_->heroCard = new QFrame(contentHost);
    d_->heroCard->setObjectName(QStringLiteral("HeroCard"));
    d_->heroCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setupCardShadow(d_->heroCard, 28, 8, QColor(0, 0, 0, 34));

    auto* heroLayout = new QHBoxLayout(d_->heroCard);
    heroLayout->setContentsMargins(
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding);
    heroLayout->setSpacing(printercare::app::kWindowSectionSpacing);

    auto* heroCopy = new QVBoxLayout();
    heroCopy->setSpacing(14);

    auto* heroKicker = new QLabel(QStringLiteral("PC"), d_->heroCard);
    heroKicker->setObjectName(QStringLiteral("HeroKicker"));

    auto* heroTitle = new QLabel(
        tr("LIVE DASHBOARD"),
        d_->heroCard);
    heroTitle->setObjectName(QStringLiteral("HeroTitle"));
    heroTitle->setWordWrap(true);

    auto* heroDescription = new QLabel(
<<<<<<< HEAD
        tr("Tray, reminders, and history stay in sync while closed."),
=======
        tr("Keep the printer healthy while the app stays quiet in the tray."),
>>>>>>> 2a89a04941611fe5ae80dcc4f741cbfb21fd10f6
        d_->heroCard);
    heroDescription->setObjectName(QStringLiteral("HeroDescription"));
    heroDescription->setWordWrap(true);

    heroCopy->addWidget(heroKicker);
    heroCopy->addWidget(heroTitle);
    heroCopy->addWidget(heroDescription);
<<<<<<< HEAD
    heroCopy->addWidget(createSignalRow(d_->heroCard, tr("Tray, reminders, and history stay in sync while closed.")));
    heroCopy->addWidget(createSignalRow(d_->heroCard, tr("One process keeps checks and reminders alive.")));
    heroCopy->addWidget(createSignalRow(d_->heroCard, tr("Tray active and ready for work.")));
=======
    heroCopy->addWidget(createSignalRow(d_->heroCard, tr("Test print and head cleaning stay on schedule.")));
    heroCopy->addWidget(createSignalRow(d_->heroCard, tr("Service history is saved locally and easy to review.")));
    heroCopy->addWidget(createSignalRow(d_->heroCard, tr("Tray mode keeps the app ready but unobtrusive.")));
>>>>>>> 2a89a04941611fe5ae80dcc4f741cbfb21fd10f6

    heroLayout->addLayout(heroCopy, 1);

    auto* heroArtFrame = new QFrame(d_->heroCard);
    heroArtFrame->setObjectName(QStringLiteral("HeroArtFrame"));
    heroArtFrame->setMinimumSize(310, 260);
    heroArtFrame->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto* heroArtLayout = new QVBoxLayout(heroArtFrame);
    heroArtLayout->setContentsMargins(18, 18, 18, 18);
    heroArtLayout->setSpacing(10);

    auto* heroArtBadge = printercare::ui::createMiniPill(heroArtFrame, tr("Tray live"));
    heroArtBadge->setAlignment(Qt::AlignCenter);
    heroArtLayout->addWidget(heroArtBadge, 0, Qt::AlignLeft);

    auto* heroArt = new QLabel(heroArtFrame);
    heroArt->setObjectName(QStringLiteral("HeroArt"));
    heroArt->setAlignment(Qt::AlignCenter);
    heroArt->setMinimumHeight(170);
    QPixmap heroPixmap(QStringLiteral(":/icons/printer-hero.svg"));
    if (!heroPixmap.isNull())
    {
        heroArt->setPixmap(heroPixmap.scaled(260, 190, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    else
    {
        heroArt->setText(QStringLiteral("PC"));
    }

    auto* heroArtCaption = new QLabel(tr("Always awake in the tray"), heroArtFrame);
    heroArtCaption->setObjectName(QStringLiteral("HeroArtCaption"));
    heroArtCaption->setAlignment(Qt::AlignCenter);

    heroArtLayout->addWidget(heroArt, 1);
    heroArtLayout->addWidget(heroArtCaption);

    heroLayout->addWidget(heroArtFrame, 0, Qt::AlignCenter);
    heroLayout->setStretch(0, 3);
    heroLayout->setStretch(1, 2);

    auto* companionCard = new QFrame(contentHost);
    companionCard->setObjectName(QStringLiteral("CompanionCard"));
    companionCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setupCardShadow(companionCard, 24, 8, QColor(0, 0, 0, 30));

    auto* companionLayout = new QVBoxLayout(companionCard);
    companionLayout->setContentsMargins(
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding);
    companionLayout->setSpacing(12);

    auto* companionTitle = new QLabel(tr("Modern desktop companion"), companionCard);
    companionTitle->setObjectName(QStringLiteral("CardTitle"));
    auto* companionDescription = new QLabel(
<<<<<<< HEAD
        tr("Printer maintenance, always one click away."),
=======
        tr("Log service, open settings, and inspect history without leaving the dashboard."),
>>>>>>> 2a89a04941611fe5ae80dcc4f741cbfb21fd10f6
        companionCard);
    companionDescription->setObjectName(QStringLiteral("CardDescription"));
    companionDescription->setWordWrap(true);

    companionLayout->addWidget(companionTitle);
    companionLayout->addWidget(companionDescription);

    auto* companionActionRow = new QHBoxLayout();
    companionActionRow->setSpacing(12);

    auto* aboutButton = new QPushButton(tr("About build"), companionCard);
    aboutButton->setObjectName(QStringLiteral("PrimaryActionButton"));
    aboutButton->setIcon(QIcon(QStringLiteral(":/icons/dashboard.svg")));
    aboutButton->setIconSize(QSize(18, 18));
    connect(aboutButton, &QPushButton::clicked, this, &MainWindow::showAboutDialog);

    auto* themeButton = new QPushButton(tr("Theme"), companionCard);
    themeButton->setObjectName(QStringLiteral("SecondaryActionButton"));
    themeButton->setIcon(QIcon(QStringLiteral(":/icons/appearance.svg")));
    themeButton->setIconSize(QSize(18, 18));
    connect(themeButton, &QPushButton::clicked, this, [this]()
    {
        applyTheme(d_->theme == Theme::Light);
    });

    companionActionRow->addWidget(aboutButton);
    companionActionRow->addWidget(themeButton);
    companionActionRow->addStretch(1);
    companionLayout->addLayout(companionActionRow);

    auto* signalsCard = new QFrame(contentHost);
    signalsCard->setObjectName(QStringLiteral("SignalsCard"));
    signalsCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setupCardShadow(signalsCard, 24, 8, QColor(0, 0, 0, 30));

    auto* signalsLayout = new QVBoxLayout(signalsCard);
    signalsLayout->setContentsMargins(
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding);
    signalsLayout->setSpacing(12);

    auto* signalsTitle = new QLabel(tr("Core signals"), signalsCard);
    signalsTitle->setObjectName(QStringLiteral("CardTitle"));
    auto* signalsSubtitle = new QLabel(tr("What the app is doing right now."), signalsCard);
    signalsSubtitle->setObjectName(QStringLiteral("CardDescription"));
    signalsSubtitle->setWordWrap(true);

    signalsLayout->addWidget(signalsTitle);
    signalsLayout->addWidget(signalsSubtitle);
    signalsLayout->addWidget(createSignalRow(signalsCard, tr("Reminders")));
    signalsLayout->addWidget(createSignalRow(signalsCard, tr("Tray flow")));
    signalsLayout->addWidget(createSignalRow(signalsCard, tr("Local history")));

    auto* quickActionsCard = new QFrame(contentHost);
    quickActionsCard->setObjectName(QStringLiteral("QuickActionsCard"));
    quickActionsCard->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setupCardShadow(quickActionsCard, 24, 8, QColor(0, 0, 0, 30));

    auto* quickActionsLayout = new QVBoxLayout(quickActionsCard);
    quickActionsLayout->setContentsMargins(
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding);
    quickActionsLayout->setSpacing(12);

    auto* quickActionsTitle = new QLabel(tr("Quick actions"), quickActionsCard);
    quickActionsTitle->setObjectName(QStringLiteral("CardTitle"));
    auto* quickActionsSubtitle = new QLabel(
        tr("Printer maintenance, always one click away."),
        quickActionsCard);
    quickActionsSubtitle->setObjectName(QStringLiteral("CardDescription"));
    quickActionsSubtitle->setWordWrap(true);

    quickActionsLayout->addWidget(quickActionsTitle);
    quickActionsLayout->addWidget(quickActionsSubtitle);

    d_->logTestPrintButton = new QPushButton(tr("Run check now"), quickActionsCard);
    d_->logTestPrintButton->setObjectName(QStringLiteral("SecondaryActionButton"));
    d_->logTestPrintButton->setIcon(QIcon(QStringLiteral(":/icons/reminder.svg")));
    d_->logTestPrintButton->setIconSize(QSize(18, 18));

    d_->logHeadCleaningButton = new QPushButton(tr("Open log"), quickActionsCard);
    d_->logHeadCleaningButton->setObjectName(QStringLiteral("SecondaryActionButton"));
    d_->logHeadCleaningButton->setIcon(QIcon(QStringLiteral(":/icons/history.svg")));
    d_->logHeadCleaningButton->setIconSize(QSize(18, 18));

    quickActionsLayout->addWidget(d_->logTestPrintButton);
    quickActionsLayout->addWidget(d_->logHeadCleaningButton);
    quickActionsLayout->addStretch(1);

    d_->maintenanceCard = new QFrame(contentHost);
    d_->maintenanceCard->setObjectName(QStringLiteral("StatusBoardCard"));
    d_->maintenanceCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setupCardShadow(d_->maintenanceCard, 24, 8, QColor(0, 0, 0, 30));

    auto* maintenanceLayout = new QVBoxLayout(d_->maintenanceCard);
    maintenanceLayout->setContentsMargins(
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding);
    maintenanceLayout->setSpacing(14);

    auto* maintenanceTitle = new QLabel(tr("Maintenance snapshot"), d_->maintenanceCard);
    maintenanceTitle->setObjectName(QStringLiteral("CardTitle"));
    auto* maintenanceDescription = new QLabel(
        tr("Schedule, history, and startup state stay aligned."),
        d_->maintenanceCard);
    maintenanceDescription->setObjectName(QStringLiteral("CardDescription"));
    maintenanceDescription->setWordWrap(true);

    maintenanceLayout->addWidget(maintenanceTitle);
    maintenanceLayout->addWidget(maintenanceDescription);

    auto* statusGrid = new QGridLayout();
    statusGrid->setHorizontalSpacing(12);
    statusGrid->setVerticalSpacing(12);

    auto* testPrintRow = createStatTile(
        d_->maintenanceCard,
        QStringLiteral(":/icons/reminder.svg"),
        tr("Test print"),
        tr("Loading"),
        tr("Periodicity"),
        d_->testPrintStatusValue);

    auto* headCleaningRow = createStatTile(
        d_->maintenanceCard,
        QStringLiteral(":/icons/tray.svg"),
        tr("Next check"),
        tr("Loading"),
        tr("Head cleaning"),
        d_->headCleaningStatusValue);

    d_->historySummaryValue = new QLabel(tr("0"), d_->maintenanceCard);
    d_->historySummaryValue->setObjectName(QStringLiteral("StatusTileValue"));
    d_->historySummaryValue->setWordWrap(true);

    auto* historyTile = new QFrame(d_->maintenanceCard);
    historyTile->setObjectName(QStringLiteral("StatusTile"));
    auto* historyTileLayout = new QVBoxLayout(historyTile);
    historyTileLayout->setContentsMargins(16, 16, 16, 16);
    historyTileLayout->setSpacing(10);
    historyTileLayout->addWidget(printercare::ui::createIconBadge(
        historyTile,
        QStringLiteral(":/icons/history.svg"),
        QStringLiteral("StatusTileIcon"),
        34,
        18));

    auto* historyTileTitle = new QLabel(tr("Journal entries"), historyTile);
    historyTileTitle->setObjectName(QStringLiteral("StatusTileTitle"));
    auto* historyTileCaption = new QLabel(tr("Records"), historyTile);
    historyTileCaption->setObjectName(QStringLiteral("StatusTileCaption"));
    d_->historySummaryValue->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    historyTileLayout->addWidget(historyTileTitle);
    historyTileLayout->addWidget(d_->historySummaryValue);
    historyTileLayout->addWidget(historyTileCaption);
    historyTileLayout->addStretch(1);

    statusGrid->addWidget(testPrintRow, 0, 0);
    statusGrid->addWidget(headCleaningRow, 0, 1);
    statusGrid->addWidget(historyTile, 0, 2);
    statusGrid->setColumnStretch(0, 1);
    statusGrid->setColumnStretch(1, 1);
    statusGrid->setColumnStretch(2, 1);
    maintenanceLayout->addLayout(statusGrid);

    d_->historyPathsValue = new QLabel(d_->maintenanceCard);
    d_->historyPathsValue->setObjectName(QStringLiteral("HistoryMetaLine"));
    d_->historyPathsValue->setWordWrap(true);
    maintenanceLayout->addWidget(d_->historyPathsValue);

    auto* historyPanel = new QFrame(contentHost);
    historyPanel->setObjectName(QStringLiteral("HistoryPanel"));
    historyPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setupCardShadow(historyPanel, 24, 8, QColor(0, 0, 0, 30));

    auto* historyPanelLayout = new QVBoxLayout(historyPanel);
    historyPanelLayout->setContentsMargins(16, 16, 16, 16);
    historyPanelLayout->setSpacing(12);

    auto* historyHeader = new QHBoxLayout();
    historyHeader->setSpacing(10);
    historyHeader->addWidget(printercare::ui::createIconBadge(
        historyPanel,
        QStringLiteral(":/icons/history.svg"),
        QStringLiteral("HistoryHeaderIcon"),
        38,
        20));

    auto* historyTitle = new QLabel(tr("Service history"), historyPanel);
    historyTitle->setObjectName(QStringLiteral("HistoryPanelTitle"));
    historyHeader->addWidget(historyTitle, 1);
    historyPanelLayout->addLayout(historyHeader);

    auto* historySubtitle = new QLabel(
        tr("Recent service stays saved."),
        historyPanel);
    historySubtitle->setObjectName(QStringLiteral("HistoryMetaLine"));
    historySubtitle->setWordWrap(true);
    historyPanelLayout->addWidget(historySubtitle);

    d_->historyList = new QListWidget(historyPanel);
    d_->historyList->setObjectName(QStringLiteral("HistoryList"));
    d_->historyList->setSelectionMode(QAbstractItemView::NoSelection);
    d_->historyList->setFocusPolicy(Qt::NoFocus);
    d_->historyList->setSpacing(8);
    d_->historyList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    d_->historyList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_->historyList->setMinimumWidth(320);
    d_->historyList->setMaximumHeight(220);
    d_->historyList->setFrameShape(QFrame::NoFrame);
    historyPanelLayout->addWidget(d_->historyList);

    connect(d_->logTestPrintButton, &QPushButton::clicked, this, [this]()
    {
        checkReminders(false);
    });

    connect(d_->logHeadCleaningButton, &QPushButton::clicked, this, [this]()
    {
        showHistoryWindow();
    });

    auto* promptCard = new QFrame(rootWidget);
    promptCard->setObjectName(QStringLiteral("DesignerPromptCard"));
    promptCard->setMinimumWidth(430);
    promptCard->setMaximumWidth(460);
    promptCard->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    setupCardShadow(promptCard, 28, 10, QColor(0, 0, 0, 44));

    auto* promptLayout = new QVBoxLayout(promptCard);
    promptLayout->setContentsMargins(
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding,
        printercare::app::kInnerPadding);
    promptLayout->setSpacing(14);

    auto* promptScroll = new QScrollArea(promptCard);
    promptScroll->setObjectName(QStringLiteral("PromptScroll"));
    promptScroll->setWidgetResizable(true);
    promptScroll->setFrameShape(QFrame::NoFrame);
    promptScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto* promptContent = new QWidget(promptScroll);
    promptContent->setObjectName(QStringLiteral("PromptContent"));
    promptScroll->setWidget(promptContent);

    auto* promptContentLayout = new QVBoxLayout(promptContent);
    promptContentLayout->setContentsMargins(0, 0, 0, 0);
    promptContentLayout->setSpacing(14);

    auto* promptTitle = new QLabel(tr("ПРОМТ ДЛЯ ДИЗАЙНЕРА / AI (UI/UX)"), promptContent);
    promptTitle->setObjectName(QStringLiteral("PromptTitle"));
    promptTitle->setWordWrap(true);

    auto* promptLead = new QLabel(
        tr("Створи сучасний темний інтерфейс десктопного додатка для обслуговування принтерів."),
        promptContent);
    promptLead->setObjectName(QStringLiteral("PromptLead"));
    promptLead->setWordWrap(true);

    promptContentLayout->addWidget(promptTitle);
    promptContentLayout->addWidget(promptLead);

    auto addPromptBullet = [promptContentLayout](const QString& text)
    {
        auto* row = new QHBoxLayout();
        row->setSpacing(10);
        auto* bullet = new QLabel(QStringLiteral("•"), promptContentLayout->parentWidget());
        bullet->setObjectName(QStringLiteral("PromptBullet"));
        bullet->setFixedWidth(14);
        auto* label = new QLabel(text, promptContentLayout->parentWidget());
        label->setObjectName(QStringLiteral("PromptBulletText"));
        label->setWordWrap(true);
        row->addWidget(bullet, 0, Qt::AlignTop);
        row->addWidget(label, 1);
        promptContentLayout->addLayout(row);
    };

    auto* styleTitle = new QLabel(tr("Стиль:"), promptContent);
    styleTitle->setObjectName(QStringLiteral("PromptSectionTitle"));
    promptContentLayout->addWidget(styleTitle);
    addPromptBullet(tr("Темна тема (Windows 11 look and feel)"));
    addPromptBullet(tr("М'які тіні, скруглені картки (12–16px)"));
    addPromptBullet(tr("Синій акцент (#3B82F6) + зелені індикатори"));
    addPromptBullet(tr("Чітка типографіка, хороша ієрархія"));
    addPromptBullet(tr("Іконки в стилі Fluent / Material Symbols"));

    auto* structureTitle = new QLabel(tr("Структура інтерфейсу:"), promptContent);
    structureTitle->setObjectName(QStringLiteral("PromptSectionTitle"));
    promptContentLayout->addWidget(structureTitle);
    addPromptBullet(tr("Ліва бічна панель навігації з іконками"));
    addPromptBullet(tr("Верхня панель станів (трей, перевірки, журнал)"));
    addPromptBullet(tr("Сітка карток з інформацією та діями"));
    addPromptBullet(tr("Кнопки з іконками, стани (активний/неактивний)"));
    addPromptBullet(tr("Перемикач світлої теми внизу сайдбару"));

    auto* addTitle = new QLabel(tr("Додай:"), promptContent);
    addTitle->setObjectName(QStringLiteral("PromptSectionTitle"));
    promptContentLayout->addWidget(addTitle);
    addPromptBullet(tr("Ілюстрацію принтера у правому верхньому блоці"));
    addPromptBullet(tr("Індикатори стану (зелений = ok)"));
    addPromptBullet(tr("Приємні відступи та баланс елементів"));
    addPromptBullet(tr("Адаптивність під різні розміри вікна"));

    auto* styleTextTitle = new QLabel(tr("Текстовий стиль:"), promptContent);
    styleTextTitle->setObjectName(QStringLiteral("PromptSectionTitle"));
    promptContentLayout->addWidget(styleTextTitle);
    addPromptBullet(tr("Українська мова"));
    addPromptBullet(tr("Зрозуміла мікротипографіка"));
    addPromptBullet(tr("Професійний, чистий, дружній до користувача"));

    auto* goalTitle = new QLabel(tr("Ціль:"), promptContent);
    goalTitle->setObjectName(QStringLiteral("PromptSectionTitle"));
    auto* goalText = new QLabel(
        tr("Зробити інтерфейс сучасним, зручним і приємним для щоденного використання."),
        promptContent);
    goalText->setObjectName(QStringLiteral("PromptLead"));
    goalText->setWordWrap(true);

    promptContentLayout->addWidget(goalTitle);
    promptContentLayout->addWidget(goalText);
    promptContentLayout->addStretch(1);

    promptLayout->addWidget(promptScroll);

    dashboardGrid->addWidget(summaryCard, 0, 0);
    dashboardGrid->addWidget(d_->heroCard, 0, 1, 1, 2);
    dashboardGrid->addWidget(companionCard, 1, 0);
    dashboardGrid->addWidget(signalsCard, 1, 1);
    dashboardGrid->addWidget(quickActionsCard, 1, 2);
    dashboardGrid->addWidget(d_->maintenanceCard, 2, 0, 1, 2);
    dashboardGrid->addWidget(historyPanel, 2, 2);
    dashboardGrid->setRowStretch(0, 0);
    dashboardGrid->setRowStretch(1, 0);
    dashboardGrid->setRowStretch(2, 1);
    dashboardGrid->setColumnStretch(0, 1);
    dashboardGrid->setColumnStretch(1, 1);
    dashboardGrid->setColumnStretch(2, 1);

    contentLayout->addLayout(dashboardGrid, 1);

    auto* footerLabel = new QLabel(
        tr("C++20 | Qt 6 | CMake | Windows 11 style"),
        contentHost);
    footerLabel->setObjectName(QStringLiteral("FooterLabel"));
    footerLabel->setAlignment(Qt::AlignCenter);
    footerLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    contentLayout->addWidget(footerLabel);

    shellLayout->addWidget(sidebarCard, 0);
    shellLayout->addWidget(contentScroll, 1);

    auto* bodyLayout = new QHBoxLayout();
    bodyLayout->setSpacing(printercare::app::kWindowSectionSpacing);
    bodyLayout->addWidget(shellCard, 1);
    bodyLayout->addWidget(promptCard, 0);
    rootLayout->addLayout(bodyLayout, 1);
}

void MainWindow::applyTheme(bool darkTheme)
{
    d_->theme = darkTheme ? Theme::Dark : Theme::Light;

    QApplication::setPalette(buildPalette(d_->theme));
    qApp->setStyleSheet(loadTextResource(themeStylePath(d_->theme)));
    updateThemeActions();
}

void MainWindow::loadInitialState()
{
    QString configError;
    const std::optional<printercare::models::AppSettings> loadedSettings = d_->configManager.load(&configError);
    if (loadedSettings.has_value())
    {
        d_->settings = *loadedSettings;
    }
    else if (!configError.isEmpty())
    {
        QMessageBox::warning(this, tr("Printer Care"), configError);
    }

    QString historyError;
    if (!d_->historyManager.load(&historyError) && !historyError.isEmpty())
    {
        QMessageBox::warning(this, tr("Printer Care"), historyError);
    }

    applyTheme(d_->settings.themeMode == printercare::models::ThemeMode::Dark);
    refreshMaintenanceOverview();
}

void MainWindow::saveCurrentSettings()
{
    QString configError;
    if (!d_->configManager.save(d_->settings, &configError))
    {
        QMessageBox::warning(this, tr("Printer Care"), configError);
    }
}

void MainWindow::applyCurrentSettings(bool persistStartupIntegration)
{
    applyTheme(d_->settings.themeMode == printercare::models::ThemeMode::Dark);

    if (d_->trayManager != nullptr && d_->trayReady)
    {
        d_->trayManager->setToolTip(
            tr("%1 - %2")
                .arg(QString::fromLatin1(printercare::app::kApplicationName))
                .arg(d_->settings.startMinimizedToTray
                    ? tr("tray mode")
                    : tr("window mode")));
    }

    if (persistStartupIntegration)
    {
        saveCurrentSettings();
    }

    if (printercare::managers::StartupManager::isSupported())
    {
        QString startupError;
        if (!printercare::managers::StartupManager::setLaunchAtStartupEnabled(
                d_->settings.launchAtStartup,
                QCoreApplication::applicationFilePath(),
                QString::fromLatin1(printercare::app::kApplicationName),
                &startupError))
        {
            if (!startupError.isEmpty())
            {
                QMessageBox::warning(this, tr("Printer Care"), startupError);
            }
        }
    }

    d_->startHiddenOnLaunch = d_->trayReady && d_->settings.startMinimizedToTray;
    refreshMaintenanceOverview();
    scheduleReminderCheck();
    checkReminders(false);
}

void MainWindow::refreshMaintenanceOverview()
{
    const auto testPrintLastServiceDate = d_->historyManager.lastServiceDate(
        printercare::managers::HistoryManager::ServiceType::TestPrint);
    const auto headCleaningLastServiceDate = d_->historyManager.lastServiceDate(
        printercare::managers::HistoryManager::ServiceType::HeadCleaning);

    const auto testPrintEvaluation = printercare::managers::ReminderManager::evaluate(
        QDate::currentDate(),
        testPrintLastServiceDate,
        d_->settings.testPrintRule);

    const auto headCleaningEvaluation = printercare::managers::ReminderManager::evaluate(
        QDate::currentDate(),
        headCleaningLastServiceDate,
        d_->settings.headCleaningRule);

    if (d_->testPrintStatusValue != nullptr)
    {
        d_->testPrintStatusValue->setText(
            d_->settings.testPrintRule.enabled
                ? tr("Every %1 days").arg(d_->settings.testPrintRule.intervalDays)
                : tr("Disabled"));
    }

    if (d_->headCleaningStatusValue != nullptr)
    {
        const auto nextDueDate = printercare::managers::ReminderManager::nextDueDate(
            headCleaningLastServiceDate,
            d_->settings.headCleaningRule);
        d_->headCleaningStatusValue->setText(
            nextDueDate.isValid()
                ? QLocale().toString(QDateTime(nextDueDate, QTime(21, 0)), QLocale::ShortFormat)
                : tr("Not yet recorded"));
    }

    if (d_->historySummaryValue != nullptr)
    {
        d_->historySummaryValue->setText(QString::number(d_->historyManager.totalRecords()));
    }

    if (d_->historyPathsValue != nullptr)
    {
        d_->historyPathsValue->setText(
            tr("Config: %1  |  Log: %2")
                .arg(QDir::toNativeSeparators(d_->configManager.configFilePath()))
                .arg(QDir::toNativeSeparators(d_->historyManager.historyFilePath())));
    }

    if (d_->historyList != nullptr)
    {
        d_->historyList->clear();
        const QList<printercare::managers::HistoryManager::ServiceRecord> records = d_->historyManager.recentRecords(5);
        for (const auto& record : records)
        {
            const QString line = tr("%1\n%2%3")
                .arg(QLocale().toString(record.timestamp, QLocale::ShortFormat))
                .arg(serviceTypeLabel(record.type))
                .arg(record.note.isEmpty() ? QString() : tr(" - %1").arg(record.note));
            auto* item = new QListWidgetItem(line, d_->historyList);
            item->setSizeHint(QSize(0, 54));
        }

        if (records.isEmpty())
        {
            auto* item = new QListWidgetItem(tr("No maintenance logged yet."), d_->historyList);
            item->setSizeHint(QSize(0, 42));
        }
    }

    if (d_->statusChip != nullptr)
    {
        const bool anyDue = testPrintEvaluation.isDue || headCleaningEvaluation.isDue;
        d_->statusChip->setText(
            anyDue
                ? tr("Attention needed")
                : (d_->settings.startMinimizedToTray ? tr("Tray ready") : tr("Window ready")));
    }
}

void MainWindow::scheduleReminderCheck()
{
    if (d_->reminderTimer.isActive())
    {
        d_->reminderTimer.stop();
    }

    d_->reminderTimer.setSingleShot(true);

    const QDateTime now = QDateTime::currentDateTime();
    QDateTime nextCheck = startOfDay(QDate::currentDate().addDays(1));

    const auto testPrintDueDate = printercare::managers::ReminderManager::nextDueDate(
        d_->historyManager.lastServiceDate(printercare::managers::HistoryManager::ServiceType::TestPrint),
        d_->settings.testPrintRule);
    const auto headCleaningDueDate = printercare::managers::ReminderManager::nextDueDate(
        d_->historyManager.lastServiceDate(printercare::managers::HistoryManager::ServiceType::HeadCleaning),
        d_->settings.headCleaningRule);

    const QDateTime testPrintCheck = testPrintDueDate.isValid() && testPrintDueDate > QDate::currentDate()
        ? startOfDay(testPrintDueDate)
        : startOfDay(QDate::currentDate().addDays(1));
    const QDateTime headCleaningCheck = headCleaningDueDate.isValid() && headCleaningDueDate > QDate::currentDate()
        ? startOfDay(headCleaningDueDate)
        : startOfDay(QDate::currentDate().addDays(1));

    if (testPrintCheck.isValid() && testPrintCheck < nextCheck)
    {
        nextCheck = testPrintCheck;
    }

    if (headCleaningCheck.isValid() && headCleaningCheck < nextCheck)
    {
        nextCheck = headCleaningCheck;
    }

    int intervalMs = now.msecsTo(nextCheck);
    if (intervalMs < 60 * 1000)
    {
        intervalMs = 60 * 1000;
    }

    d_->reminderTimer.start(intervalMs);
}

void MainWindow::checkReminders(bool allowNotifications)
{
    const QDate currentDate = QDate::currentDate();

    const auto testPrintLastDate = d_->historyManager.lastServiceDate(printercare::managers::HistoryManager::ServiceType::TestPrint);
    const auto headCleaningLastDate = d_->historyManager.lastServiceDate(printercare::managers::HistoryManager::ServiceType::HeadCleaning);

    const auto testPrintEvaluation = printercare::managers::ReminderManager::evaluate(
        currentDate,
        testPrintLastDate,
        d_->settings.testPrintRule);
    const auto headCleaningEvaluation = printercare::managers::ReminderManager::evaluate(
        currentDate,
        headCleaningLastDate,
        d_->settings.headCleaningRule);

    if (allowNotifications && d_->trayManager != nullptr && d_->settings.showNotifications)
    {
        if (testPrintEvaluation.isDue && d_->lastTestPrintNotificationDate != currentDate)
        {
            d_->trayManager->showMessage(
                tr("Printer Care"),
                tr("Test print maintenance is due today."),
                QSystemTrayIcon::Information);
            d_->lastTestPrintNotificationDate = currentDate;
        }

        if (headCleaningEvaluation.isDue && d_->lastHeadCleaningNotificationDate != currentDate)
        {
            d_->trayManager->showMessage(
                tr("Printer Care"),
                tr("Head cleaning maintenance is due today."),
                QSystemTrayIcon::Information);
            d_->lastHeadCleaningNotificationDate = currentDate;
        }
    }

    refreshMaintenanceOverview();
}

void MainWindow::recordServiceAction(printercare::managers::HistoryManager::ServiceType type)
{
    const QString note = tr("Logged from the main window");
    if (!d_->historyManager.record(type, QDateTime::currentDateTime(), note))
    {
        QMessageBox::warning(this, tr("Printer Care"), tr("Unable to save service history."));
        return;
    }

    if (type == printercare::managers::HistoryManager::ServiceType::TestPrint)
    {
        d_->lastTestPrintNotificationDate = {};
    }
    else
    {
        d_->lastHeadCleaningNotificationDate = {};
    }

    refreshMaintenanceOverview();
    checkReminders(false);

    if (d_->trayManager != nullptr && d_->settings.showNotifications)
    {
        d_->trayManager->showMessage(
            tr("Printer Care"),
            tr("%1 entry saved to service history.").arg(serviceTypeLabel(type)),
            QSystemTrayIcon::Information);
    }
}

QString MainWindow::serviceTypeLabel(printercare::managers::HistoryManager::ServiceType type) const
{
    return ::serviceTypeText(type);
}

void MainWindow::initializeTray()
{
    d_->trayManager = std::make_unique<printercare::managers::TrayManager>();
    d_->trayReady = d_->trayManager->initialize(
        QIcon(QStringLiteral(":/icons/app.svg")),
        QStringLiteral("Printer Care"));

    if (d_->trayReady)
    {
        connect(d_->trayManager.get(), &printercare::managers::TrayManager::showRequested, this, &MainWindow::showFromTray);
        connect(d_->trayManager.get(), &printercare::managers::TrayManager::openRequested, this, &MainWindow::showFromTray);
        connect(d_->trayManager.get(), &printercare::managers::TrayManager::markTestPrintRequested, this, [this]()
        {
            recordServiceAction(printercare::managers::HistoryManager::ServiceType::TestPrint);
        });
        connect(d_->trayManager.get(), &printercare::managers::TrayManager::markHeadCleaningRequested, this, [this]()
        {
            recordServiceAction(printercare::managers::HistoryManager::ServiceType::HeadCleaning);
        });
        connect(d_->trayManager.get(), &printercare::managers::TrayManager::settingsRequested, this, &MainWindow::showSettingsWindow);
        connect(d_->trayManager.get(), &printercare::managers::TrayManager::historyRequested, this, &MainWindow::showHistoryWindow);
        connect(d_->trayManager.get(), &printercare::managers::TrayManager::aboutRequested, this, &MainWindow::showAboutDialog);
        connect(d_->trayManager.get(), &printercare::managers::TrayManager::exitRequested, this, &MainWindow::requestApplicationExit);
        connect(d_->trayManager.get(), &printercare::managers::TrayManager::notificationClicked, this, &MainWindow::showFromTray);
    }

    applyCurrentSettings(false);
}

void MainWindow::showFromTray()
{
    if (isMinimized())
    {
        showNormal();
    }

    if (!isVisible())
    {
        show();
    }

    raise();
    activateWindow();
}

void MainWindow::updateThemeActions() const
{
    if (d_->lightThemeAction != nullptr)
    {
        d_->lightThemeAction->setChecked(d_->theme == Theme::Light);
    }

    if (d_->darkThemeAction != nullptr)
    {
        d_->darkThemeAction->setChecked(d_->theme == Theme::Dark);
    }

    if (d_->themeToggleButton != nullptr)
    {
        d_->themeToggleButton->setText(
            d_->theme == Theme::Light
                ? tr("Dark mode")
                : tr("Light mode"));
    }
}

void MainWindow::startIntroAnimation()
{
    if (d_->introStarted)
    {
        return;
    }

    auto* animation = new QPropertyAnimation(this, "windowOpacity", this);
    animation->setDuration(printercare::app::kIntroAnimationMs);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    d_->introStarted = true;
}

void MainWindow::showAboutDialog()
{
    const QString repositoryLine = QString::fromLatin1(printercare::app::kRepositoryUrl).isEmpty()
        ? tr("GitHub: not configured")
        : tr("GitHub: <a href=\"%1\">%1</a>").arg(QString::fromLatin1(printercare::app::kRepositoryUrl));

    QMessageBox::about(
        this,
        tr("About Printer Care"),
        tr("<b>%1</b><br>"
           "Version %2<br><br>"
           "Desktop reminder app for inkjet maintenance.<br><br>"
           "Author: %3<br>"
           "%4<br>"
           "License: %5")
            .arg(QString::fromLatin1(printercare::app::kApplicationName))
            .arg(QString::fromLatin1(printercare::app::kApplicationVersion))
            .arg(QString::fromLatin1(printercare::app::kAuthor))
            .arg(repositoryLine)
            .arg(QString::fromLatin1(printercare::app::kLicenseName)));
}

void MainWindow::showSettingsWindow()
{
    SettingsWindow dialog(d_->settings, this);
    connect(&dialog, &SettingsWindow::settingsSaved, this, [this](const printercare::models::AppSettings& settings)
    {
        d_->settings = settings;
        applyCurrentSettings();
    });
    dialog.exec();
}

void MainWindow::showHistoryWindow()
{
    showFromTray();

    if (d_->historyList != nullptr)
    {
        d_->historyList->setFocus();
        if (d_->historyList->count() > 0)
        {
            d_->historyList->scrollToBottom();
        }
    }
}

void MainWindow::requestApplicationExit()
{
    d_->allowClose = true;
    qApp->quit();
}

void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    startIntroAnimation();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (d_->allowClose)
    {
        event->accept();
        return;
    }

    if (d_->trayReady)
    {
        event->ignore();
        hide();
        return;
    }

    event->accept();
    QTimer::singleShot(0, qApp, &QCoreApplication::quit);
}
