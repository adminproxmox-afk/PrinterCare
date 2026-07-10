#include "localization/AppTranslator.h"

#include <QLocale>

namespace
{
printercare::localization::AppTranslator::UiLanguage resolveLanguage(
    printercare::models::LanguageMode mode)
{
    using printercare::models::LanguageMode;
    using UiLanguage = printercare::localization::AppTranslator::UiLanguage;

    switch (mode)
    {
    case LanguageMode::English:
        return UiLanguage::English;
    case LanguageMode::Ukrainian:
        return UiLanguage::Ukrainian;
    case LanguageMode::Russian:
        return UiLanguage::Russian;
    case LanguageMode::System:
    default:
        break;
    }

    switch (QLocale::system().language())
    {
    case QLocale::Ukrainian:
        return UiLanguage::Ukrainian;
    case QLocale::Russian:
        return UiLanguage::Russian;
    default:
        return UiLanguage::English;
    }
}
} // namespace

namespace printercare::localization
{
AppTranslator::AppTranslator(printercare::models::LanguageMode mode)
    : language_(resolveLanguage(mode))
{
    if (!isEnglish())
    {
        buildTranslations();
    }
}

bool AppTranslator::isEnglish() const noexcept
{
    return language_ == UiLanguage::English;
}

void AppTranslator::add(const char* sourceText, const char* translatedText)
{
    translations_.insert(QString::fromUtf8(sourceText), QString::fromUtf8(translatedText));
}

QString AppTranslator::translate(
    const char* context,
    const char* sourceText,
    const char* disambiguation,
    int n) const
{
    Q_UNUSED(context);
    Q_UNUSED(disambiguation);
    Q_UNUSED(n);

    const auto it = translations_.constFind(QString::fromUtf8(sourceText));
    if (it == translations_.constEnd())
    {
        return {};
    }

    return it.value();
}

void AppTranslator::buildTranslations()
{
    if (language_ == UiLanguage::Ukrainian)
    {
        add("&File", "&Файл");
        add("E&xit", "&Вийти");
        add("&View", "&Вигляд");
        add("&Light theme", "&Світла тема");
        add("&Dark theme", "&Темна тема");
        add("&Toggle theme", "&Перемкнути тему");
        add("&Tools", "&Інструменти");
        add("&Settings...", "&Налаштування...");
        add("&Help", "&Довідка");
        add("&About Printer Care", "&Про Printer Care");
        add("LIVE DASHBOARD", "ЖИВА ПАНЕЛЬ");
        add("Tray, reminders, and history stay in sync while closed.",
            "Трей, нагадування та історія синхронізуються навіть коли вікно закрите.");
        add("One process keeps checks and reopen flow alive.",
            "Один процес підтримує перевірки та швидке відкриття.");
        add("Tray live", "Трей активний");
        add("Checks on", "Перевірки увімкнені");
        add("Local log", "Локальний журнал");
        add("Tray active", "Трей активний");
        add("Tray live", "Трей активний");
        add("Auto sync", "Автосинхронізація");
        add("Local history", "Локальна історія");
        add("Always on", "Завжди активний");
        add("Tray runtime, background checks, instant reopen.",
            "Трей, фонові перевірки, миттєве відкриття.");
        add("Open the tray menu or restore the window anytime.",
            "Відкрийте меню трея або відновіть вікно будь-коли.");
        add("Quick actions", "Швидкі дії");
        add("Always awake in the tray", "Завжди активний у треї");
        add("Service history", "Історія обслуговування");
        add("Saved entries", "Збережені записи");
        add("Checks", "Перевірки");
        add("Tray", "Трей");
        add("%1\nLast: %2", "%1\nОстаннє: %2");
        add("Switch between sections without losing the feeling of a single polished surface.",
            "Перемикайтеся між розділами, не втрачаючи відчуття єдиної цілісної панелі.");
        add("Changes are saved automatically while you move through the controls.",
            "Зміни зберігаються автоматично, поки ви працюєте з елементами керування.");
        add("MODERN DESKTOP COMPANION", "СУЧАСНИЙ ДЕСКТОПНИЙ КОМПАНЬЙОН");
        add("Modern desktop companion", "Сучасний десктопний компаньйон");
        add("Printer maintenance, always one click away.",
            "Обслуговування принтера, завжди в один клік.");
        add("Tracks schedules, logs service, and surfaces tray reminders.",
            "Відстежує графіки, журналить дії та показує нагадування в треї.");
        add("About build", "Про збірку");
        add("Theme", "Тема");
        add("Quick actions", "Швидкі дії");
        add("Run check now", "Запустити перевірку зараз");
        add("Open log", "Відкрити журнал");
        add("Periodicity", "Періодичність");
        add("Journal entries", "Записів в журналі");
        add("Records", "Записів");
        add("Every %1 days", "Кожні %1 днів");
        add("Core signals", "Ключові сигнали");
        add("What the app is doing right now.", "Що додаток робить прямо зараз.");
        add("Reminders", "Нагадування");
        add("Rule checks stay current.", "Перевірки правил актуальні.");
        add("Tray flow", "Потік трея");
        add("Always available in the tray.", "Завжди доступний у треї.");
        add("Local history", "Локальна історія");
        add("Recent service stays saved.", "Останні дії зберігаються.");
        add("Maintenance snapshot", "Статус обслуговування");
        add("Schedule, history, and startup state stay aligned.",
            "Графік, історія та автозапуск залишаються синхронними.");
        add("Maintenance cadence", "Періодичність обслуговування");
        add("Nozzle health", "Стан сопел");
        add("Records: %1 | config: %2 | log: %3",
            "Записів: %1 | конфіг: %2 | журнал: %3");
        add("Test print", "Тестовий друк");
        add("Head cleaning", "Очищення голівки");
        add("Pending", "Очікує");
        add("History is loading...", "Історія завантажується...");
        add("Log test print", "Записати тестовий друк");
        add("Log head cleaning", "Записати очищення голівки");
        add("Open settings", "Відкрити налаштування");
        add("Refresh", "Оновити");
        add("C++20 | Qt 6 | CMake", "C++20 | Qt 6 | CMake");
        add("Loading settings", "Завантаження налаштувань");
        add("Tray ready", "Трей готовий");
        add("Active", "Активний");
        add("Window ready", "Вікно готове");
        add("Attention needed", "Потрібна увага");
        add("Overdue", "Прострочено");
        add("Due today", "Сьогодні потрібно");
        add("Next check in %1 day(s)", "Наступна перевірка через %1 дн.");
        add("Disabled", "Вимкнено");
        add("Not yet recorded", "Ще не записано");
        add("No maintenance logged yet.", "Ще немає записаних дій.");
        add("Dark mode", "Темний режим");
        add("Light mode", "Світлий режим");
        add("GitHub: not configured", "GitHub: не налаштовано");
        add("About Printer Care", "Про Printer Care");
        add("Desktop reminder app for inkjet maintenance.",
            "Настільний додаток-нагадувач для обслуговування струменевих принтерів.");
        add("Version %2", "Версія %2");
        add("MainWindow", "Головне вікно");
        add("Settings", "Налаштування");
        add("Printer Care Settings", "Налаштування Printer Care");
        add("Tune reminders, appearance, and startup behavior from one place.",
            "Налаштовуйте нагадування, вигляд і запуск в одному місці.");
        add("Commercial UI shell", "Комерційна оболонка інтерфейсу");
        add("CONTROL CENTER", "ПАНЕЛЬ КЕРУВАННЯ");
        add("Tune the app without losing the sense that it is always awake in the tray.",
            "Налаштовуйте додаток, не втрачаючи відчуття, що він завжди активний у треї.");
        add("Themes, startup behavior, reminder cadence, and saved history all stay aligned with one profile.",
            "Теми, автозапуск, частота нагадувань та збережена історія працюють в одному профілі.");
        add("Tray-first", "Перш за все трей");
        add("Auto-save", "Автозбереження");
        add("One profile", "Один профіль");
        add("General", "Загальні");
        add("Reminders", "Нагадування");
        add("Appearance", "Вигляд");
        add("Advanced", "Додатково");
        add("Restore defaults", "Відновити стандартні");
        add("Cancel", "Скасувати");
        add("Save changes", "Зберегти зміни");
        add("Startup and system integration", "Автозапуск і системна інтеграція");
        add("These settings shape how Printer Care behaves when Windows starts and how aggressively it surfaces reminders.",
            "Ці параметри визначають, як Printer Care поводиться під час запуску Windows і наскільки активно показує нагадування.");
        add("Launch Printer Care when Windows starts", "Запускати Printer Care під час старту Windows");
        add("Start minimized to the system tray", "Запускати згорнутим у системний трей");
        add("Show maintenance notifications", "Показувати сповіщення про обслуговування");
        add("Profile: Desktop reminder shell", "Профіль: оболонка-нагадувач для робочого столу");
        add("Test print reminder", "Нагадування про тестовий друк");
        add("Prompt the user to run a test print so the nozzles stay healthy.",
            "Підказувати користувачу виконати тестовий друк, щоб сопла залишалися справними.");
        add("Head cleaning reminder", "Нагадування про очищення голівки");
        add("Prompt the user to perform head cleaning at the configured cadence.",
            "Підказувати користувачу виконувати очищення голівки з потрібною періодичністю.");
        add("Visual style", "Візуальний стиль");
        add("Choose the application tone and language. Theme changes are applied immediately.",
            "Оберіть стиль і мову додатку. Зміни теми застосовуються одразу.");
        add("System default", "Системне значення");
        add("Light", "Світла");
        add("Dark", "Темна");
        add("Russian", "Російська");
        add("Theme mode", "Режим теми");
        add("Language", "Мова");
        add("Preview", "Попередній перегляд");
        add("Theme changes are applied immediately and written back to the config file.",
            "Зміни теми застосовуються одразу та записуються у файл конфігурації.");
        add("Windows 11 inspired", "Натхненний Windows 11");
        add("Configuration and maintenance data", "Конфігурація та дані обслуговування");
        add("These locations are used by the persistence layer and the service history store.",
            "Ці розташування використовуються шаром збереження та сховищем історії обслуговування.");
        add("Config file", "Файл конфігурації");
        add("History storage", "Сховище історії");
        add("Enable this reminder", "Увімкнути це нагадування");
        add("Preferred day", "Бажаний день");
        add("Interval in days", "Інтервал у днях");
        add(" days", " дн.");
        add("Monday", "Понеділок");
        add("Tuesday", "Вівторок");
        add("Wednesday", "Середа");
        add("Thursday", "Четвер");
        add("Friday", "П'ятниця");
        add("Saturday", "Субота");
        add("Sunday", "Неділя");
        add("Enable this reminder", "Увімкнути це нагадування");
        add("Language changes are applied on the next launch.", "Зміни мови застосуються під час наступного запуску.");
        add("Profile: %1 | Language: %2 | launch %3 | notifications %4",
            "Профіль: %1 | Мова: %2 | запуск %3 | сповіщення %4");
        add("launch on startup", "запускати разом зі стартом");
        add("manual launch", "ручний запуск");
        add("enabled", "увімкнено");
        add("disabled", "вимкнено");
        add("Light mode", "Світлий режим");
        add("Dark mode", "Темний режим");
        add("Head cleaning", "Очищення голівки");
        add("Test print", "Тестовий друк");
        add("About", "Про");
        add("Settings...", "Налаштування...");
        add("Open", "Відкрити");
        add("History", "Історія");
        add("Mark test print", "Позначити тестовий друк");
        add("Mark head cleaning", "Позначити очищення голівки");
        add("System", "Система");
        add("English", "Англійська");
        add("Ukrainian", "Українська");
        add("Printer Care", "Printer Care");
        add("Reminder shell for inkjet maintenance, designed for the tray and the desktop.",
            "Оболонка-нагадувач для обслуговування струменевих принтерів, створена для трея та робочого столу.");
        add("Test print maintenance is due today.", "Потрібно виконати тестовий друк сьогодні.");
        add("Head cleaning maintenance is due today.", "Сьогодні потрібно виконати очищення голівки.");
        add("tray mode", "режим трея");
        add("window mode", "режим вікна");
        add("Config file path is empty.", "Шлях до файла конфігурації порожній.");
        add("History file path is empty.", "Шлях до файла історії порожній.");
        add("Unable to create config directory: %1", "Не вдалося створити каталог конфігурації: %1");
        add("Unable to create history directory: %1", "Не вдалося створити каталог історії: %1");
        add("Unable to determine the application executable path.", "Не вдалося визначити шлях до виконуваного файла додатка.");
        add("Unable to finalize config file: %1", "Не вдалося завершити запис файла конфігурації: %1");
        add("Unable to finalize history file: %1", "Не вдалося завершити запис файла історії: %1");
        add("Unable to open config file for reading: %1", "Не вдалося відкрити файл конфігурації для читання: %1");
        add("Unable to open config file for writing: %1", "Не вдалося відкрити файл конфігурації для запису: %1");
        add("Unable to open history file for reading: %1", "Не вдалося відкрити файл історії для читання: %1");
        add("Unable to open history file for writing: %1", "Не вдалося відкрити файл історії для запису: %1");
        add("Unable to save service history.", "Не вдалося зберегти історію обслуговування.");
        add("Unable to update Windows startup settings.", "Не вдалося оновити параметри автозапуску Windows.");
        add("Unable to write config file: %1", "Не вдалося записати файл конфігурації: %1");
        add("Unable to write history file: %1", "Не вдалося записати файл історії: %1");
        add("Unsupported config schema version: %1", "Непідтримувана версія схеми конфігурації: %1");
        add("Unsupported history schema version: %1", "Непідтримувана версія схеми історії: %1");
        add("Windows startup integration is only available on Windows.",
            "Інтеграція з автозапуском Windows доступна лише у Windows.");
        return;
    }

    // Russian
    add("&File", "&Файл");
    add("E&xit", "&Выход");
    add("&View", "&Вид");
    add("&Light theme", "&Светлая тема");
    add("&Dark theme", "&Тёмная тема");
    add("&Toggle theme", "&Переключить тему");
    add("&Tools", "&Инструменты");
    add("&Settings...", "&Настройки...");
    add("&Help", "&Справка");
    add("&About Printer Care", "&О Printer Care");
    add("LIVE DASHBOARD", "ЖИВАЯ ПАНЕЛЬ");
        add("Tray, reminders, and history stay in sync while closed.",
            "Трей, напоминания и история синхронизируются даже когда окно закрыто.");
    add("One process keeps checks and reopen flow alive.",
        "Один процесс поддерживает проверки и быстрое открытие.");
    add("Tray live", "Трей активен");
    add("Checks on", "Проверки включены");
    add("Local log", "Локальный журнал");
    add("Tray active", "Трей активен");
    add("Tray live", "Трей активен");
    add("Auto sync", "Автосинхронизация");
    add("Local history", "Локальная история");
    add("Always on", "Всегда активен");
    add("Tray runtime, background checks, instant reopen.",
        "Трей, фоновые проверки, мгновенное открытие.");
    add("Open the tray menu or restore the window anytime.",
        "Откройте меню трея или восстановите окно в любой момент.");
    add("Quick actions", "Быстрые действия");
    add("Always awake in the tray", "Всегда активен в трее");
    add("Service history", "История обслуживания");
    add("Saved entries", "Сохранённые записи");
    add("Checks", "Проверки");
    add("Tray", "Трей");
    add("%1\nLast: %2", "%1\nПоследнее: %2");
    add("Switch between sections without losing the feeling of a single polished surface.",
        "Переключайтесь между разделами, не теряя ощущение единой цельной панели.");
    add("Changes are saved automatically while you move through the controls.",
        "Изменения сохраняются автоматически, пока вы работаете с элементами управления.");
    add("MODERN DESKTOP COMPANION", "СОВРЕМЕННЫЙ ДЕСКТОПНЫЙ КОМПАНЬОН");
    add("Modern desktop companion", "Современный десктопный компаньон");
    add("Printer maintenance, always one click away.",
        "Обслуживание принтера, всегда в один клик.");
    add("Tracks schedules, logs service, and surfaces tray reminders.",
        "Отслеживает графики, журналирует действия и показывает напоминания в трее.");
    add("About build", "О сборке");
    add("Theme", "Тема");
    add("Quick actions", "Быстрые действия");
    add("Run check now", "Запустить проверку сейчас");
    add("Open log", "Открыть журнал");
    add("Periodicity", "Периодичность");
    add("Journal entries", "Записей в журнале");
    add("Records", "Записей");
    add("Every %1 days", "Каждые %1 дней");
    add("Core signals", "Ключевые сигналы");
    add("What the app is doing right now.", "Что приложение делает прямо сейчас.");
    add("Reminders", "Напоминания");
    add("Rule checks stay current.", "Проверки правил актуальны.");
    add("Tray flow", "Поток трея");
    add("Always available in the tray.", "Всегда доступно в трее.");
    add("Local history", "Локальная история");
    add("Recent service stays saved.", "Последние действия сохраняются.");
    add("Maintenance snapshot", "Статус обслуживания");
    add("Schedule, history, and startup state stay aligned.",
        "График, история и автозапуск остаются синхронными.");
    add("Maintenance cadence", "Периодичность обслуживания");
    add("Nozzle health", "Состояние сопел");
    add("Records: %1 | config: %2 | log: %3",
        "Записей: %1 | конфиг: %2 | журнал: %3");
    add("Test print", "Тестовая печать");
    add("Head cleaning", "Чистка головки");
    add("Pending", "Ожидает");
    add("History is loading...", "История загружается...");
    add("Log test print", "Записать тестовую печать");
    add("Log head cleaning", "Записать чистку головки");
    add("Open settings", "Открыть настройки");
    add("Refresh", "Обновить");
    add("C++20 | Qt 6 | CMake", "C++20 | Qt 6 | CMake");
    add("Loading settings", "Загрузка настроек");
    add("Tray ready", "Трей готов");
    add("Active", "Активен");
    add("Window ready", "Окно готово");
    add("Attention needed", "Требуется внимание");
    add("Overdue", "Просрочено");
    add("Due today", "Нужно сегодня");
    add("Next check in %1 day(s)", "Следующая проверка через %1 дн.");
    add("Disabled", "Отключено");
    add("Not yet recorded", "Ещё не записано");
    add("No maintenance logged yet.", "Пока нет записанных действий.");
    add("Dark mode", "Тёмный режим");
    add("Light mode", "Светлый режим");
    add("GitHub: not configured", "GitHub: не настроен");
    add("About Printer Care", "О Printer Care");
    add("Desktop reminder app for inkjet maintenance.",
        "Настольное приложение-напоминатель для обслуживания струйных принтеров.");
    add("Settings", "Настройки");
    add("Printer Care Settings", "Настройки Printer Care");
    add("Tune reminders, appearance, and startup behavior from one place.",
        "Настраивайте напоминания, внешний вид и запуск в одном месте.");
    add("Commercial UI shell", "Коммерческая оболочка интерфейса");
    add("CONTROL CENTER", "ЦЕНТР УПРАВЛЕНИЯ");
    add("Tune the app without losing the sense that it is always awake in the tray.",
        "Настраивайте приложение, не теряя ощущение, что оно всегда активно в трее.");
    add("Themes, startup behavior, reminder cadence, and saved history all stay aligned with one profile.",
        "Темы, автозапуск, частота напоминаний и сохранённая история остаются в одном профиле.");
    add("Tray-first", "Сначала трей");
    add("Auto-save", "Автосохранение");
    add("One profile", "Один профиль");
    add("General", "Общие");
    add("Reminders", "Напоминания");
    add("Appearance", "Внешний вид");
    add("Advanced", "Дополнительно");
    add("Restore defaults", "Сбросить настройки");
    add("Cancel", "Отмена");
    add("Save changes", "Сохранить изменения");
    add("Startup and system integration", "Автозапуск и системная интеграция");
    add("These settings shape how Printer Care behaves when Windows starts and how aggressively it surfaces reminders.",
        "Эти параметры определяют, как Printer Care ведёт себя при запуске Windows и насколько активно показывает напоминания.");
    add("Launch Printer Care when Windows starts", "Запускать Printer Care при старте Windows");
    add("Start minimized to the system tray", "Запускать свернутым в системный трей");
    add("Show maintenance notifications", "Показывать уведомления об обслуживании");
    add("Profile: Desktop reminder shell", "Профиль: оболочка-напоминатель для рабочего стола");
    add("Test print reminder", "Напоминание о тестовой печати");
    add("Prompt the user to run a test print so the nozzles stay healthy.",
        "Подсказывать пользователю выполнить тестовую печать, чтобы сопла оставались в порядке.");
    add("Head cleaning reminder", "Напоминание о чистке головки");
    add("Prompt the user to perform head cleaning at the configured cadence.",
        "Подсказывать пользователю выполнять чистку головки с заданной периодичностью.");
    add("Visual style", "Визуальный стиль");
    add("Choose the application tone and language. Theme changes are applied immediately.",
        "Выберите стиль и язык приложения. Изменения темы применяются сразу.");
    add("System default", "Системное значение");
    add("Light", "Светлая");
    add("Dark", "Тёмная");
    add("Russian", "Русский");
    add("Theme mode", "Режим темы");
    add("Language", "Язык");
    add("Preview", "Предпросмотр");
    add("Theme changes are applied immediately and written back to the config file.",
        "Изменения темы применяются сразу и записываются в файл конфигурации.");
    add("Windows 11 inspired", "Вдохновлено Windows 11");
    add("Configuration and maintenance data", "Конфигурация и данные обслуживания");
    add("These locations are used by the persistence layer and the service history store.",
        "Эти пути используются слоем сохранения и хранилищем истории обслуживания.");
    add("Config file", "Файл конфигурации");
    add("History storage", "Хранилище истории");
    add("Enable this reminder", "Включить это напоминание");
    add("Preferred day", "Предпочтительный день");
    add("Interval in days", "Интервал в днях");
    add(" days", " дн.");
    add("Monday", "Понедельник");
    add("Tuesday", "Вторник");
    add("Wednesday", "Среда");
    add("Thursday", "Четверг");
    add("Friday", "Пятница");
    add("Saturday", "Суббота");
    add("Sunday", "Воскресенье");
    add("Language changes are applied on the next launch.", "Изменения языка применятся при следующем запуске.");
    add("Profile: %1 | Language: %2 | launch %3 | notifications %4",
        "Профиль: %1 | Язык: %2 | запуск %3 | уведомления %4");
    add("launch on startup", "запуск при старте");
    add("manual launch", "ручной запуск");
    add("enabled", "включено");
    add("disabled", "выключено");
    add("Light mode", "Светлый режим");
    add("Dark mode", "Тёмный режим");
    add("Head cleaning", "Чистка головки");
    add("Test print", "Тестовая печать");
    add("About", "О программе");
    add("Settings...", "Настройки...");
    add("Open", "Открыть");
    add("History", "История");
    add("Mark test print", "Отметить тестовую печать");
    add("Mark head cleaning", "Отметить чистку головки");
    add("System", "Система");
    add("English", "Английский");
    add("Ukrainian", "Украинский");
    add("Printer Care", "Printer Care");
    add("Reminder shell for inkjet maintenance, designed for the tray and the desktop.",
        "Оболочка-напоминатель для обслуживания струйных принтеров, созданная для трея и рабочего стола.");
    add("Test print maintenance is due today.", "Сегодня нужно выполнить тестовую печать.");
    add("Head cleaning maintenance is due today.", "Сегодня нужно выполнить чистку головки.");
    add("tray mode", "режим трея");
    add("window mode", "режим окна");
    add("Config file path is empty.", "Путь к файлу конфигурации пуст.");
    add("History file path is empty.", "Путь к файлу истории пуст.");
    add("Unable to create config directory: %1", "Не удалось создать каталог конфигурации: %1");
    add("Unable to create history directory: %1", "Не удалось создать каталог истории: %1");
    add("Unable to determine the application executable path.", "Не удалось определить путь к исполняемому файлу приложения.");
    add("Unable to finalize config file: %1", "Не удалось завершить запись файла конфигурации: %1");
    add("Unable to finalize history file: %1", "Не удалось завершить запись файла истории: %1");
    add("Unable to open config file for reading: %1", "Не удалось открыть файл конфигурации для чтения: %1");
    add("Unable to open config file for writing: %1", "Не удалось открыть файл конфигурации для записи: %1");
    add("Unable to open history file for reading: %1", "Не удалось открыть файл истории для чтения: %1");
    add("Unable to open history file for writing: %1", "Не удалось открыть файл истории для записи: %1");
    add("Unable to save service history.", "Не удалось сохранить историю обслуживания.");
    add("Unable to update Windows startup settings.", "Не удалось обновить параметры автозапуска Windows.");
    add("Unable to write config file: %1", "Не удалось записать файл конфигурации: %1");
    add("Unable to write history file: %1", "Не удалось записать файл истории: %1");
    add("Unsupported config schema version: %1", "Неподдерживаемая версия схемы конфигурации: %1");
    add("Unsupported history schema version: %1", "Неподдерживаемая версия схемы истории: %1");
    add("Windows startup integration is only available on Windows.",
        "Интеграция с автозапуском Windows доступна только в Windows.");
}
} // namespace printercare::localization
