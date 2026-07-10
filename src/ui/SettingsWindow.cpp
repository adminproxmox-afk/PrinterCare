#include "ui/SettingsWindow.h"
#include "ui/UiDecor.h"

#include "core/AppInfo.h"
#include "managers/ConfigManager.h"
#include "managers/HistoryManager.h"

#include <QAbstractItemView>
#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QListWidget>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QShowEvent>
#include <QSpinBox>
#include <QStackedWidget>
#include <QListWidgetItem>
#include <QSize>
#include <QSizePolicy>
#include <QSignalBlocker>
#include <QVBoxLayout>

#include <array>
#include <utility>

namespace
{
struct ReminderEditor
{
    QCheckBox* enabled = nullptr;
    QComboBox* preferredDay = nullptr;
    QSpinBox* intervalDays = nullptr;
};

QString themeModeLabel(printercare::models::ThemeMode mode)
{
    using printercare::models::ThemeMode;

    switch (mode)
    {
    case ThemeMode::Light:
        return QObject::tr("Light");
    case ThemeMode::Dark:
        return QObject::tr("Dark");
    case ThemeMode::System:
    default:
        return QObject::tr("System");
    }
}

QString languageModeLabel(printercare::models::LanguageMode mode)
{
    using printercare::models::LanguageMode;

    switch (mode)
    {
    case LanguageMode::English:
        return QObject::tr("English");
    case LanguageMode::Ukrainian:
        return QObject::tr("Ukrainian");
    case LanguageMode::Russian:
        return QObject::tr("Russian");
    case LanguageMode::System:
    default:
        return QObject::tr("System default");
    }
}

QString weekDayLabel(printercare::models::WeekDay day)
{
    using printercare::models::WeekDay;

    switch (day)
    {
    case WeekDay::Monday:
        return QObject::tr("Monday");
    case WeekDay::Tuesday:
        return QObject::tr("Tuesday");
    case WeekDay::Wednesday:
        return QObject::tr("Wednesday");
    case WeekDay::Thursday:
        return QObject::tr("Thursday");
    case WeekDay::Friday:
        return QObject::tr("Friday");
    case WeekDay::Saturday:
        return QObject::tr("Saturday");
    case WeekDay::Sunday:
    default:
        return QObject::tr("Sunday");
    }
}

QFrame* createCard(QWidget* parent, const QString& title, const QString& description)
{
    auto* card = new QFrame(parent);
    card->setObjectName(QStringLiteral("SettingsCard"));

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(12);

    auto* accentLine = new QFrame(card);
    accentLine->setObjectName(QStringLiteral("SettingsCardAccent"));
    accentLine->setFixedHeight(3);

    auto* titleLabel = new QLabel(title, card);
    titleLabel->setObjectName(QStringLiteral("SettingsSectionTitle"));

    auto* descriptionLabel = new QLabel(description, card);
    descriptionLabel->setObjectName(QStringLiteral("SettingsSectionDescription"));
    descriptionLabel->setWordWrap(true);

    layout->addWidget(accentLine);
    layout->addWidget(titleLabel);
    layout->addWidget(descriptionLabel);

    return card;
}

QWidget* createDivider(QWidget* parent)
{
    auto* line = new QFrame(parent);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);
    line->setObjectName(QStringLiteral("SettingsDivider"));
    return line;
}

void populateWeekDayCombo(QComboBox* combo)
{
    combo->clear();
    combo->addItem(weekDayLabel(printercare::models::WeekDay::Monday), static_cast<int>(printercare::models::WeekDay::Monday));
    combo->addItem(weekDayLabel(printercare::models::WeekDay::Tuesday), static_cast<int>(printercare::models::WeekDay::Tuesday));
    combo->addItem(weekDayLabel(printercare::models::WeekDay::Wednesday), static_cast<int>(printercare::models::WeekDay::Wednesday));
    combo->addItem(weekDayLabel(printercare::models::WeekDay::Thursday), static_cast<int>(printercare::models::WeekDay::Thursday));
    combo->addItem(weekDayLabel(printercare::models::WeekDay::Friday), static_cast<int>(printercare::models::WeekDay::Friday));
    combo->addItem(weekDayLabel(printercare::models::WeekDay::Saturday), static_cast<int>(printercare::models::WeekDay::Saturday));
    combo->addItem(weekDayLabel(printercare::models::WeekDay::Sunday), static_cast<int>(printercare::models::WeekDay::Sunday));
}

ReminderEditor createReminderEditor(
    QWidget* parent,
    const QString& title,
    const QString& description)
{
    ReminderEditor editor{};

    auto* card = createCard(parent, title, description);
    auto* cardLayout = qobject_cast<QVBoxLayout*>(card->layout());

    auto* enabledCheck = new QCheckBox(QObject::tr("Enable this reminder"), card);
    enabledCheck->setObjectName(QStringLiteral("SettingsToggle"));

    auto* formLayout = new QGridLayout();
    formLayout->setHorizontalSpacing(16);
    formLayout->setVerticalSpacing(12);

    auto* dayLabel = new QLabel(QObject::tr("Preferred day"), card);
    dayLabel->setObjectName(QStringLiteral("SettingsFieldLabel"));

    auto* dayCombo = new QComboBox(card);
    dayCombo->setObjectName(QStringLiteral("SettingsCombo"));
    populateWeekDayCombo(dayCombo);

    auto* intervalLabel = new QLabel(QObject::tr("Interval in days"), card);
    intervalLabel->setObjectName(QStringLiteral("SettingsFieldLabel"));

    auto* intervalSpin = new QSpinBox(card);
    intervalSpin->setObjectName(QStringLiteral("SettingsSpinBox"));
    intervalSpin->setRange(1, 60);
    intervalSpin->setSuffix(QObject::tr(" days"));

    formLayout->addWidget(dayLabel, 0, 0);
    formLayout->addWidget(dayCombo, 0, 1);
    formLayout->addWidget(intervalLabel, 1, 0);
    formLayout->addWidget(intervalSpin, 1, 1);
    formLayout->setColumnStretch(1, 1);

    cardLayout->addWidget(enabledCheck);
    cardLayout->addLayout(formLayout);

    editor.enabled = enabledCheck;
    editor.preferredDay = dayCombo;
    editor.intervalDays = intervalSpin;
    return editor;
}

printercare::models::ThemeMode themeModeFromIndex(int index)
{
    using printercare::models::ThemeMode;

    switch (index)
    {
    case 1:
        return ThemeMode::Light;
    case 2:
        return ThemeMode::Dark;
    case 0:
    default:
        return ThemeMode::System;
    }
}

int themeModeToIndex(printercare::models::ThemeMode mode)
{
    using printercare::models::ThemeMode;

    switch (mode)
    {
    case ThemeMode::Light:
        return 1;
    case ThemeMode::Dark:
        return 2;
    case ThemeMode::System:
    default:
        return 0;
    }
}

int weekDayToIndex(printercare::models::WeekDay day)
{
    return static_cast<int>(day) - 1;
}

printercare::models::WeekDay weekDayFromIndex(int index)
{
    using printercare::models::WeekDay;

    switch (index)
    {
    case 0:
        return WeekDay::Monday;
    case 1:
        return WeekDay::Tuesday;
    case 2:
        return WeekDay::Wednesday;
    case 3:
        return WeekDay::Thursday;
    case 4:
        return WeekDay::Friday;
    case 5:
        return WeekDay::Saturday;
    case 6:
    default:
        return WeekDay::Sunday;
    }
}

printercare::models::LanguageMode languageModeFromIndex(int index)
{
    using printercare::models::LanguageMode;

    switch (index)
    {
    case 1:
        return LanguageMode::English;
    case 2:
        return LanguageMode::Ukrainian;
    case 3:
        return LanguageMode::Russian;
    case 0:
    default:
        return LanguageMode::System;
    }
}

int languageModeToIndex(printercare::models::LanguageMode mode)
{
    using printercare::models::LanguageMode;

    switch (mode)
    {
    case LanguageMode::English:
        return 1;
    case LanguageMode::Ukrainian:
        return 2;
    case LanguageMode::Russian:
        return 3;
    case LanguageMode::System:
    default:
        return 0;
    }
}
} // namespace

struct SettingsWindowPrivate
{
    printercare::models::AppSettings currentSettings{};
    bool uiBuilt = false;

    QWidget* rootWidget = nullptr;
    QListWidget* navigation = nullptr;
    QStackedWidget* pages = nullptr;
    QPushButton* saveButton = nullptr;
    QPushButton* restoreDefaultsButton = nullptr;
    QComboBox* themeCombo = nullptr;
    QComboBox* languageCombo = nullptr;
    QCheckBox* launchAtStartupCheck = nullptr;
    QCheckBox* startMinimizedCheck = nullptr;
    QCheckBox* showNotificationsCheck = nullptr;
    ReminderEditor testPrintEditor{};
    ReminderEditor headCleaningEditor{};
    QLabel* configPathValue = nullptr;
    QLabel* currentProfileValue = nullptr;
};

SettingsWindow::SettingsWindow(QWidget* parent)
    : SettingsWindow(printercare::models::AppSettings{}, parent)
{
}

SettingsWindow::SettingsWindow(const printercare::models::AppSettings& settings, QWidget* parent)
    : QDialog(parent)
    , d_(std::make_unique<SettingsWindowPrivate>())
{
    setWindowOpacity(1.0);
    configureWindow();
    buildUi();
    connectSignals();
    applySettingsToUi(settings);
}

SettingsWindow::~SettingsWindow() = default;

void SettingsWindow::configureWindow()
{
    setObjectName(QStringLiteral("SettingsWindow"));
    setWindowTitle(tr("Printer Care Settings"));
    setWindowIcon(QIcon(QStringLiteral(":/icons/settings.svg")));
    setModal(true);
    setMinimumSize(
        printercare::app::kSettingsWindowWidth,
        printercare::app::kSettingsWindowHeight);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
}

void SettingsWindow::buildUi()
{
    auto* rootWidget = new QWidget(this);
    rootWidget->setObjectName(QStringLiteral("SettingsWindowRoot"));
    d_->rootWidget = rootWidget;

    auto* mainLayout = new QVBoxLayout(rootWidget);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(18);

    auto* headerCard = new QFrame(rootWidget);
    headerCard->setObjectName(QStringLiteral("SettingsHeaderCard"));
    headerCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto* headerShadow = new QGraphicsDropShadowEffect(headerCard);
    headerShadow->setBlurRadius(24.0);
    headerShadow->setOffset(0.0, 8.0);
    headerShadow->setColor(QColor(0, 0, 0, 36));
    headerCard->setGraphicsEffect(headerShadow);

    auto* headerLayout = new QHBoxLayout(headerCard);
    headerLayout->setContentsMargins(20, 18, 20, 18);
    headerLayout->setSpacing(16);

    auto* headerIcon = new QLabel(headerCard);
    headerIcon->setObjectName(QStringLiteral("SettingsHeaderIcon"));
    headerIcon->setFixedSize(64, 64);
    headerIcon->setAlignment(Qt::AlignCenter);

    QPixmap iconPixmap(QStringLiteral(":/icons/settings.svg"));
    if (!iconPixmap.isNull())
    {
        headerIcon->setPixmap(iconPixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    else
    {
        headerIcon->setText(QStringLiteral("PC"));
    }

    auto* headerText = new QVBoxLayout();
    headerText->setSpacing(6);

    auto* titleLabel = new QLabel(tr("Settings"), headerCard);
    titleLabel->setObjectName(QStringLiteral("SettingsWindowTitle"));

    auto* subtitleLabel = new QLabel(
<<<<<<< HEAD
        tr("Tune reminders, appearance, and startup behavior from one place."),
=======
        tr("Tune reminders, appearance, and startup behavior in one place."),
>>>>>>> 2a89a04941611fe5ae80dcc4f741cbfb21fd10f6
        headerCard);
    subtitleLabel->setObjectName(QStringLiteral("SettingsWindowSubtitle"));
    subtitleLabel->setWordWrap(true);

    headerText->addWidget(titleLabel);
    headerText->addWidget(subtitleLabel);

<<<<<<< HEAD
    auto* profileBadge = new QLabel(tr("Commercial UI shell"), headerCard);
=======
    auto* profileBadge = new QLabel(tr("Tray-first shell"), headerCard);
>>>>>>> 2a89a04941611fe5ae80dcc4f741cbfb21fd10f6
    profileBadge->setObjectName(QStringLiteral("SettingsProfileBadge"));
    profileBadge->setAlignment(Qt::AlignCenter);

    headerLayout->addWidget(headerIcon, 0, Qt::AlignTop);
    headerLayout->addLayout(headerText, 1);
    headerLayout->addWidget(profileBadge, 0, Qt::AlignTop);

    auto* overviewBanner = new QFrame(rootWidget);
    overviewBanner->setObjectName(QStringLiteral("SettingsOverviewBanner"));
    overviewBanner->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    auto* overviewShadow = new QGraphicsDropShadowEffect(overviewBanner);
    overviewShadow->setBlurRadius(22.0);
    overviewShadow->setOffset(0.0, 6.0);
    overviewShadow->setColor(QColor(0, 0, 0, 22));
    overviewBanner->setGraphicsEffect(overviewShadow);

    auto* overviewLayout = new QHBoxLayout(overviewBanner);
    overviewLayout->setContentsMargins(20, 18, 20, 18);
    overviewLayout->setSpacing(16);

    overviewLayout->addWidget(printercare::ui::createIconBadge(
        overviewBanner,
        QStringLiteral(":/icons/language.svg"),
        QStringLiteral("BannerIcon"),
        68,
        34),
        0,
        Qt::AlignTop);

    auto* overviewLeft = new QVBoxLayout();
    overviewLeft->setSpacing(6);

    auto* overviewKicker = new QLabel(tr("CONTROL CENTER"), overviewBanner);
    overviewKicker->setObjectName(QStringLiteral("SettingsBannerKicker"));

    auto* overviewTitle = new QLabel(
<<<<<<< HEAD
        tr("Tune the app without losing the sense that it is always awake in the tray."),
=======
        tr("Control the app without losing the feeling that it stays awake in the tray."),
>>>>>>> 2a89a04941611fe5ae80dcc4f741cbfb21fd10f6
        overviewBanner);
    overviewTitle->setObjectName(QStringLiteral("SettingsBannerTitle"));
    overviewTitle->setWordWrap(true);

    auto* overviewSubtitle = new QLabel(
<<<<<<< HEAD
        tr("Themes, startup behavior, reminder cadence, and saved history all stay aligned with one profile."),
=======
        tr("Themes, startup behavior, reminder cadence, and saved history stay aligned with one profile."),
>>>>>>> 2a89a04941611fe5ae80dcc4f741cbfb21fd10f6
        overviewBanner);
    overviewSubtitle->setObjectName(QStringLiteral("SettingsBannerSubtitle"));
    overviewSubtitle->setWordWrap(true);

    overviewLeft->addWidget(overviewKicker);
    overviewLeft->addWidget(overviewTitle);
    overviewLeft->addWidget(overviewSubtitle);

    auto* pillRow = new QHBoxLayout();
    pillRow->setSpacing(10);
    pillRow->addWidget(printercare::ui::createMiniPill(overviewBanner, tr("Tray-first")));
    pillRow->addWidget(printercare::ui::createMiniPill(overviewBanner, tr("Auto-save")));
<<<<<<< HEAD
    pillRow->addWidget(printercare::ui::createMiniPill(overviewBanner, tr("One profile")));
=======
    pillRow->addWidget(printercare::ui::createMiniPill(overviewBanner, tr("Single profile")));
>>>>>>> 2a89a04941611fe5ae80dcc4f741cbfb21fd10f6

    overviewLayout->addLayout(overviewLeft, 1);
    overviewLayout->addStretch(1);
    overviewLayout->addLayout(pillRow, 0);

    d_->navigation = new QListWidget(rootWidget);
    d_->navigation->setObjectName(QStringLiteral("SettingsNavigation"));
    d_->navigation->setFixedWidth(printercare::app::kSettingsNavigationWidth);
    d_->navigation->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    d_->navigation->setSpacing(6);
    d_->navigation->setSelectionMode(QAbstractItemView::SingleSelection);
    d_->navigation->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_->navigation->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_->navigation->setFrameShape(QFrame::NoFrame);

    const std::array<std::pair<QString, QString>, 4> pages{
        std::make_pair(tr("General"), QStringLiteral(":/icons/tray.svg")),
        std::make_pair(tr("Reminders"), QStringLiteral(":/icons/reminder.svg")),
        std::make_pair(tr("Appearance"), QStringLiteral(":/icons/appearance.svg")),
        std::make_pair(tr("Advanced"), QStringLiteral(":/icons/history.svg"))
    };

    for (const auto& page : pages)
    {
        auto* item = new QListWidgetItem(QIcon(page.second), page.first, d_->navigation);
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        item->setSizeHint(QSize(0, 48));
    }

    d_->pages = new QStackedWidget(rootWidget);
    d_->pages->setObjectName(QStringLiteral("SettingsPages"));
    d_->pages->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    buildNavigation();
    buildPages();

    auto* bodyLayout = new QHBoxLayout();
    bodyLayout->setSpacing(18);

    auto* railShell = new QFrame(rootWidget);
    railShell->setObjectName(QStringLiteral("SettingsRail"));
    railShell->setMinimumWidth(printercare::app::kSettingsNavigationWidth + 32);
    railShell->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    auto* railShadow = new QGraphicsDropShadowEffect(railShell);
    railShadow->setBlurRadius(24.0);
    railShadow->setOffset(0.0, 8.0);
    railShadow->setColor(QColor(0, 0, 0, 34));
    railShell->setGraphicsEffect(railShadow);

    auto* railLayout = new QVBoxLayout(railShell);
    railLayout->setContentsMargins(18, 18, 18, 18);
    railLayout->setSpacing(12);

    auto* railTitle = new QLabel(tr("Settings"), railShell);
    railTitle->setObjectName(QStringLiteral("SettingsSectionTitle"));
    auto* railSubtitle = new QLabel(
        tr("Switch between sections without losing the feeling of a single polished surface."),
        railShell);
    railSubtitle->setObjectName(QStringLiteral("SettingsSectionDescription"));
    railSubtitle->setWordWrap(true);

    railLayout->addWidget(railTitle);
    railLayout->addWidget(railSubtitle);
    railLayout->addWidget(d_->navigation, 1);

    auto* railHint = new QLabel(
<<<<<<< HEAD
        tr("Changes are saved automatically while you move through the controls."),
=======
        tr("Changes are saved automatically as you move through the controls."),
>>>>>>> 2a89a04941611fe5ae80dcc4f741cbfb21fd10f6
        railShell);
    railHint->setObjectName(QStringLiteral("SettingsInfoLine"));
    railHint->setWordWrap(true);
    railLayout->addWidget(railHint);

    auto* pagesShell = new QFrame(rootWidget);
    pagesShell->setObjectName(QStringLiteral("SettingsPagesCard"));
    pagesShell->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto* pagesShadow = new QGraphicsDropShadowEffect(pagesShell);
    pagesShadow->setBlurRadius(28.0);
    pagesShadow->setOffset(0.0, 10.0);
    pagesShadow->setColor(QColor(0, 0, 0, 40));
    pagesShell->setGraphicsEffect(pagesShadow);

    auto* pagesLayout = new QVBoxLayout(pagesShell);
    pagesLayout->setContentsMargins(18, 18, 18, 18);
    pagesLayout->setSpacing(0);
    pagesLayout->addWidget(d_->pages);

    bodyLayout->addWidget(railShell);
    bodyLayout->addWidget(pagesShell, 1);
    bodyLayout->setStretch(0, 0);
    bodyLayout->setStretch(1, 1);

    auto* footerCard = new QFrame(rootWidget);
    footerCard->setObjectName(QStringLiteral("SettingsFooterCard"));
    footerCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto* footerLayout = new QHBoxLayout(footerCard);
    footerLayout->setContentsMargins(20, 16, 20, 16);
    footerLayout->setSpacing(12);

    d_->restoreDefaultsButton = new QPushButton(tr("Restore defaults"), footerCard);
    d_->restoreDefaultsButton->setObjectName(QStringLiteral("SecondaryActionButton"));
    d_->restoreDefaultsButton->setIcon(QIcon(QStringLiteral(":/icons/history.svg")));
    d_->restoreDefaultsButton->setIconSize(QSize(18, 18));

    auto* cancelButton = new QPushButton(tr("Cancel"), footerCard);
    cancelButton->setObjectName(QStringLiteral("SecondaryActionButton"));
    cancelButton->setIcon(QIcon(QStringLiteral(":/icons/tray.svg")));
    cancelButton->setIconSize(QSize(18, 18));

    d_->saveButton = new QPushButton(tr("Save changes"), footerCard);
    d_->saveButton->setObjectName(QStringLiteral("PrimaryActionButton"));
    d_->saveButton->setIcon(QIcon(QStringLiteral(":/icons/settings.svg")));
    d_->saveButton->setIconSize(QSize(18, 18));

    auto* footerSpacer = new QWidget(footerCard);
    footerSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    footerLayout->addWidget(d_->restoreDefaultsButton);
    footerLayout->addWidget(footerSpacer, 1);
    footerLayout->addWidget(cancelButton);
    footerLayout->addWidget(d_->saveButton);

    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    mainLayout->addWidget(headerCard);
    mainLayout->addWidget(overviewBanner);
    mainLayout->addLayout(bodyLayout, 1);
    mainLayout->addWidget(footerCard);

    setLayout(new QVBoxLayout(this));
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->addWidget(rootWidget);

    d_->uiBuilt = true;
    d_->navigation->setCurrentRow(0);
}

void SettingsWindow::buildNavigation()
{
    connect(d_->navigation, &QListWidget::currentRowChanged, this, &SettingsWindow::selectPage);
}

void SettingsWindow::buildPages()
{
    auto* generalPage = new QWidget(d_->pages);
    auto* generalLayout = new QVBoxLayout(generalPage);
    generalLayout->setContentsMargins(0, 0, 0, 0);
    generalLayout->setSpacing(printercare::app::kSettingsPageSpacing);

    auto* generalIntro = createCard(
        generalPage,
        tr("Startup and system integration"),
<<<<<<< HEAD
        tr("These settings shape how Printer Care behaves when Windows starts and how aggressively it surfaces reminders."));
=======
        tr("These settings shape how Printer Care behaves when Windows starts and how it surfaces reminders."));
>>>>>>> 2a89a04941611fe5ae80dcc4f741cbfb21fd10f6

    auto* generalIntroLayout = qobject_cast<QVBoxLayout*>(generalIntro->layout());

    d_->launchAtStartupCheck = new QCheckBox(tr("Launch Printer Care when Windows starts"), generalIntro);
    d_->launchAtStartupCheck->setObjectName(QStringLiteral("SettingsToggle"));
    d_->startMinimizedCheck = new QCheckBox(tr("Start minimized to the system tray"), generalIntro);
    d_->startMinimizedCheck->setObjectName(QStringLiteral("SettingsToggle"));
    d_->showNotificationsCheck = new QCheckBox(tr("Show maintenance notifications"), generalIntro);
    d_->showNotificationsCheck->setObjectName(QStringLiteral("SettingsToggle"));

    generalIntroLayout->addWidget(d_->launchAtStartupCheck);
    generalIntroLayout->addWidget(d_->startMinimizedCheck);
    generalIntroLayout->addWidget(d_->showNotificationsCheck);

    d_->currentProfileValue = new QLabel(tr("Profile: Desktop reminder shell"), generalPage);
    d_->currentProfileValue->setObjectName(QStringLiteral("SettingsInfoLine"));

    generalLayout->addWidget(generalIntro);
    generalLayout->addWidget(d_->currentProfileValue);
    generalLayout->addStretch(1);

    auto* remindersPage = new QWidget(d_->pages);
    auto* remindersLayout = new QVBoxLayout(remindersPage);
    remindersLayout->setContentsMargins(0, 0, 0, 0);
    remindersLayout->setSpacing(printercare::app::kSettingsPageSpacing);

    d_->testPrintEditor = createReminderEditor(
        remindersPage,
        tr("Test print reminder"),
        tr("Prompt the user to run a test print so the nozzles stay healthy."));
    d_->headCleaningEditor = createReminderEditor(
        remindersPage,
        tr("Head cleaning reminder"),
        tr("Prompt the user to perform head cleaning at the configured cadence."));

    remindersLayout->addWidget(d_->testPrintEditor.enabled->parentWidget());
    remindersLayout->addWidget(d_->headCleaningEditor.enabled->parentWidget());
    remindersLayout->addStretch(1);

    auto* appearancePage = new QWidget(d_->pages);
    auto* appearanceLayout = new QVBoxLayout(appearancePage);
    appearanceLayout->setContentsMargins(0, 0, 0, 0);
    appearanceLayout->setSpacing(printercare::app::kSettingsPageSpacing);

    auto* appearanceIntro = createCard(
        appearancePage,
        tr("Visual style"),
        tr("Choose the application tone and language. Theme changes are applied immediately."));

    auto* appearanceIntroLayout = qobject_cast<QVBoxLayout*>(appearanceIntro->layout());

    d_->themeCombo = new QComboBox(appearanceIntro);
    d_->themeCombo->setObjectName(QStringLiteral("SettingsCombo"));
    d_->themeCombo->addItem(QIcon(QStringLiteral(":/icons/appearance.svg")), tr("System default"));
    d_->themeCombo->addItem(QIcon(QStringLiteral(":/icons/dashboard.svg")), tr("Light"));
    d_->themeCombo->addItem(QIcon(QStringLiteral(":/icons/history.svg")), tr("Dark"));

    auto* themeLabel = new QLabel(tr("Theme mode"), appearanceIntro);
    themeLabel->setObjectName(QStringLiteral("SettingsFieldLabel"));
    appearanceIntroLayout->addWidget(themeLabel);
    appearanceIntroLayout->addWidget(d_->themeCombo);

    auto* languageLabel = new QLabel(tr("Language"), appearanceIntro);
    languageLabel->setObjectName(QStringLiteral("SettingsFieldLabel"));
    d_->languageCombo = new QComboBox(appearanceIntro);
    d_->languageCombo->setObjectName(QStringLiteral("SettingsCombo"));
    d_->languageCombo->addItem(QIcon(QStringLiteral(":/icons/language.svg")), tr("System default"));
    d_->languageCombo->addItem(QIcon(QStringLiteral(":/icons/language.svg")), tr("English"));
    d_->languageCombo->addItem(QIcon(QStringLiteral(":/icons/language.svg")), tr("Ukrainian"));
    d_->languageCombo->addItem(QIcon(QStringLiteral(":/icons/language.svg")), tr("Russian"));
    appearanceIntroLayout->addWidget(languageLabel);
    appearanceIntroLayout->addWidget(d_->languageCombo);

    auto* languageHint = new QLabel(
        tr("Language changes are applied on the next launch."),
        appearanceIntro);
    languageHint->setObjectName(QStringLiteral("SettingsDescriptionLabel"));
    languageHint->setWordWrap(true);
    appearanceIntroLayout->addWidget(languageHint);

    auto* previewCard = createCard(
        appearancePage,
        tr("Preview"),
        tr("Theme changes are applied immediately and written back to the config file."));
    auto* previewLayout = qobject_cast<QVBoxLayout*>(previewCard->layout());

    auto* previewBadge = new QLabel(tr("Windows 11 inspired"), previewCard);
    previewBadge->setObjectName(QStringLiteral("SettingsPreviewBadge"));
    previewBadge->setAlignment(Qt::AlignCenter);

    previewLayout->addWidget(previewBadge);

    appearanceLayout->addWidget(appearanceIntro);
    appearanceLayout->addWidget(previewCard);
    appearanceLayout->addStretch(1);

    auto* advancedPage = new QWidget(d_->pages);
    auto* advancedLayout = new QVBoxLayout(advancedPage);
    advancedLayout->setContentsMargins(0, 0, 0, 0);
    advancedLayout->setSpacing(printercare::app::kSettingsPageSpacing);

    auto* advancedCard = createCard(
        advancedPage,
        tr("Configuration and maintenance data"),
        tr("These locations are used by the persistence layer and the service history store."));
    auto* advancedCardLayout = qobject_cast<QVBoxLayout*>(advancedCard->layout());

    auto* configPathLabel = new QLabel(tr("Config file"), advancedCard);
    configPathLabel->setObjectName(QStringLiteral("SettingsFieldLabel"));
    d_->configPathValue = new QLabel(
        QDir::toNativeSeparators(printercare::managers::ConfigManager::defaultConfigFilePath()),
        advancedCard);
    d_->configPathValue->setObjectName(QStringLiteral("SettingsValueLabel"));
    d_->configPathValue->setTextInteractionFlags(Qt::TextSelectableByMouse);

    advancedCardLayout->addWidget(configPathLabel);
    advancedCardLayout->addWidget(d_->configPathValue);
    advancedCardLayout->addWidget(createDivider(advancedCard));

    auto* historyLabel = new QLabel(tr("History storage"), advancedCard);
    historyLabel->setObjectName(QStringLiteral("SettingsFieldLabel"));
    auto* historyValue = new QLabel(
        QDir::toNativeSeparators(printercare::managers::HistoryManager::defaultHistoryFilePath()),
        advancedCard);
    historyValue->setObjectName(QStringLiteral("SettingsDescriptionLabel"));
    historyValue->setWordWrap(true);
    historyValue->setTextInteractionFlags(Qt::TextSelectableByMouse);

    advancedCardLayout->addWidget(historyLabel);
    advancedCardLayout->addWidget(historyValue);

    advancedLayout->addWidget(advancedCard);
    advancedLayout->addStretch(1);

    auto* generalScroll = new QScrollArea(d_->pages);
    generalScroll->setWidgetResizable(true);
    generalScroll->setFrameShape(QFrame::NoFrame);
    generalScroll->setWidget(generalPage);

    auto* remindersScroll = new QScrollArea(d_->pages);
    remindersScroll->setWidgetResizable(true);
    remindersScroll->setFrameShape(QFrame::NoFrame);
    remindersScroll->setWidget(remindersPage);

    auto* appearanceScroll = new QScrollArea(d_->pages);
    appearanceScroll->setWidgetResizable(true);
    appearanceScroll->setFrameShape(QFrame::NoFrame);
    appearanceScroll->setWidget(appearancePage);

    auto* advancedScroll = new QScrollArea(d_->pages);
    advancedScroll->setWidgetResizable(true);
    advancedScroll->setFrameShape(QFrame::NoFrame);
    advancedScroll->setWidget(advancedPage);

    d_->pages->addWidget(generalScroll);
    d_->pages->addWidget(remindersScroll);
    d_->pages->addWidget(appearanceScroll);
    d_->pages->addWidget(advancedScroll);
}

void SettingsWindow::connectSignals()
{
    connect(d_->saveButton, &QPushButton::clicked, this, [this]()
    {
        d_->currentSettings = readSettingsFromUi();
        emit settingsSaved(d_->currentSettings);
        accept();
    });

    connect(d_->restoreDefaultsButton, &QPushButton::clicked, this, [this]()
    {
        restoreDefaults();
    });

    connect(d_->themeCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this]()
    {
        persistCurrentSettings();
    });

    connect(d_->languageCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this]()
    {
        persistCurrentSettings();
    });

    connect(d_->launchAtStartupCheck, &QCheckBox::toggled, this, [this]()
    {
        persistCurrentSettings();
    });

    connect(d_->startMinimizedCheck, &QCheckBox::toggled, this, [this]()
    {
        persistCurrentSettings();
    });

    connect(d_->showNotificationsCheck, &QCheckBox::toggled, this, [this]()
    {
        persistCurrentSettings();
    });

    connect(d_->testPrintEditor.enabled, &QCheckBox::toggled, this, [this]()
    {
        persistCurrentSettings();
    });

    connect(d_->testPrintEditor.preferredDay, qOverload<int>(&QComboBox::currentIndexChanged), this, [this]()
    {
        persistCurrentSettings();
    });

    connect(d_->testPrintEditor.intervalDays, qOverload<int>(&QSpinBox::valueChanged), this, [this]()
    {
        persistCurrentSettings();
    });

    connect(d_->headCleaningEditor.enabled, &QCheckBox::toggled, this, [this]()
    {
        persistCurrentSettings();
    });

    connect(d_->headCleaningEditor.preferredDay, qOverload<int>(&QComboBox::currentIndexChanged), this, [this]()
    {
        persistCurrentSettings();
    });

    connect(d_->headCleaningEditor.intervalDays, qOverload<int>(&QSpinBox::valueChanged), this, [this]()
    {
        persistCurrentSettings();
    });
}

void SettingsWindow::applySettingsToUi(const printercare::models::AppSettings& settings)
{
    d_->currentSettings = settings;

    const QSignalBlocker themeBlocker(d_->themeCombo);
    const QSignalBlocker languageBlocker(d_->languageCombo);
    const QSignalBlocker launchBlocker(d_->launchAtStartupCheck);
    const QSignalBlocker trayBlocker(d_->startMinimizedCheck);
    const QSignalBlocker notificationsBlocker(d_->showNotificationsCheck);
    const QSignalBlocker testEnabledBlocker(d_->testPrintEditor.enabled);
    const QSignalBlocker testDayBlocker(d_->testPrintEditor.preferredDay);
    const QSignalBlocker testIntervalBlocker(d_->testPrintEditor.intervalDays);
    const QSignalBlocker headEnabledBlocker(d_->headCleaningEditor.enabled);
    const QSignalBlocker headDayBlocker(d_->headCleaningEditor.preferredDay);
    const QSignalBlocker headIntervalBlocker(d_->headCleaningEditor.intervalDays);

    if (d_->themeCombo != nullptr)
    {
        d_->themeCombo->setCurrentIndex(themeModeToIndex(settings.themeMode));
    }

    if (d_->languageCombo != nullptr)
    {
        d_->languageCombo->setCurrentIndex(languageModeToIndex(settings.languageMode));
    }

    if (d_->launchAtStartupCheck != nullptr)
    {
        d_->launchAtStartupCheck->setChecked(settings.launchAtStartup);
    }

    if (d_->startMinimizedCheck != nullptr)
    {
        d_->startMinimizedCheck->setChecked(settings.startMinimizedToTray);
    }

    if (d_->showNotificationsCheck != nullptr)
    {
        d_->showNotificationsCheck->setChecked(settings.showNotifications);
    }

    if (d_->testPrintEditor.enabled != nullptr)
    {
        d_->testPrintEditor.enabled->setChecked(settings.testPrintRule.enabled);
        d_->testPrintEditor.preferredDay->setCurrentIndex(weekDayToIndex(settings.testPrintRule.preferredDay));
        d_->testPrintEditor.intervalDays->setValue(settings.testPrintRule.intervalDays);
    }

    if (d_->headCleaningEditor.enabled != nullptr)
    {
        d_->headCleaningEditor.enabled->setChecked(settings.headCleaningRule.enabled);
        d_->headCleaningEditor.preferredDay->setCurrentIndex(weekDayToIndex(settings.headCleaningRule.preferredDay));
        d_->headCleaningEditor.intervalDays->setValue(settings.headCleaningRule.intervalDays);
    }

    updatePreview();
}

printercare::models::AppSettings SettingsWindow::readSettingsFromUi() const
{
    printercare::models::AppSettings settings = d_->currentSettings;

    if (d_->themeCombo != nullptr)
    {
        settings.themeMode = themeModeFromIndex(d_->themeCombo->currentIndex());
    }

    if (d_->languageCombo != nullptr)
    {
        settings.languageMode = languageModeFromIndex(d_->languageCombo->currentIndex());
    }

    if (d_->launchAtStartupCheck != nullptr)
    {
        settings.launchAtStartup = d_->launchAtStartupCheck->isChecked();
    }

    if (d_->startMinimizedCheck != nullptr)
    {
        settings.startMinimizedToTray = d_->startMinimizedCheck->isChecked();
    }

    if (d_->showNotificationsCheck != nullptr)
    {
        settings.showNotifications = d_->showNotificationsCheck->isChecked();
    }

    if (d_->testPrintEditor.enabled != nullptr)
    {
        settings.testPrintRule.enabled = d_->testPrintEditor.enabled->isChecked();
        settings.testPrintRule.preferredDay = weekDayFromIndex(d_->testPrintEditor.preferredDay->currentIndex());
        settings.testPrintRule.intervalDays = d_->testPrintEditor.intervalDays->value();
    }

    if (d_->headCleaningEditor.enabled != nullptr)
    {
        settings.headCleaningRule.enabled = d_->headCleaningEditor.enabled->isChecked();
        settings.headCleaningRule.preferredDay = weekDayFromIndex(d_->headCleaningEditor.preferredDay->currentIndex());
        settings.headCleaningRule.intervalDays = d_->headCleaningEditor.intervalDays->value();
    }

    return settings;
}

void SettingsWindow::updatePreview()
{
    if (d_->currentProfileValue != nullptr)
    {
        const QString profile = tr("Profile: %1 | Language: %2 | launch %3 | notifications %4")
            .arg(themeModeLabel(themeModeFromIndex(d_->themeCombo != nullptr ? d_->themeCombo->currentIndex() : 0)))
            .arg(languageModeLabel(languageModeFromIndex(d_->languageCombo != nullptr ? d_->languageCombo->currentIndex() : 0)))
            .arg(d_->launchAtStartupCheck != nullptr && d_->launchAtStartupCheck->isChecked()
                ? tr("launch on startup")
                : tr("manual launch"))
            .arg(d_->showNotificationsCheck != nullptr && d_->showNotificationsCheck->isChecked()
                ? tr("enabled")
                : tr("disabled"));
        d_->currentProfileValue->setText(profile);
    }
}

void SettingsWindow::restoreDefaults()
{
    applySettingsToUi(printercare::models::AppSettings{});
    persistCurrentSettings();
}

void SettingsWindow::persistCurrentSettings()
{
    d_->currentSettings = readSettingsFromUi();
    emit settingsSaved(d_->currentSettings);
    updatePreview();
}

void SettingsWindow::selectPage(int index)
{
    if (d_->pages == nullptr)
    {
        return;
    }

    d_->pages->setCurrentIndex(index);
}

void SettingsWindow::setSettings(const printercare::models::AppSettings& settings)
{
    applySettingsToUi(settings);
}

printercare::models::AppSettings SettingsWindow::settings() const
{
    return readSettingsFromUi();
}

void SettingsWindow::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    if (d_->uiBuilt && d_->navigation != nullptr)
    {
        d_->navigation->setCurrentRow(0);
    }
}
