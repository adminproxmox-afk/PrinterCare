cmake_minimum_required(VERSION 3.24)

project(PrinterCare
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.5...6.99 REQUIRED COMPONENTS Widgets Network)

qt_standard_project_setup()

qt_add_executable(PrinterCare
    WIN32
    src/main.cpp
    src/managers/HistoryManager.cpp
    src/managers/ReminderManager.cpp
    src/managers/StartupManager.cpp
    src/managers/TrayManager.cpp
    src/localization/AppTranslator.cpp
    src/ui/UiDecor.cpp
    src/ui/MainWindow.cpp
    src/ui/SettingsWindow.cpp
    src/managers/ConfigManager.cpp
    include/core/AppInfo.h
    include/managers/ConfigManager.h
    include/managers/HistoryManager.h
    include/managers/ReminderManager.h
    include/managers/StartupManager.h
    include/managers/TrayManager.h
    include/models/AppSettings.h
    include/ui/UiDecor.h
    include/ui/MainWindow.h
    include/ui/SettingsWindow.h
    resources/printercare.qrc
)

target_include_directories(PrinterCare
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(PrinterCare
    PRIVATE
        Qt6::Widgets
        Qt6::Network
)

target_compile_definitions(PrinterCare
    PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

set_target_properties(PrinterCare
    PROPERTIES
        OUTPUT_NAME "PrinterCare"
)

qt_add_executable(PrinterCareSmoke
    src/smoke/main.cpp
    src/managers/HistoryManager.cpp
    src/managers/ReminderManager.cpp
    src/managers/StartupManager.cpp
    src/managers/TrayManager.cpp
    src/managers/ConfigManager.cpp
    include/core/AppInfo.h
    include/managers/ConfigManager.h
    include/managers/HistoryManager.h
    include/managers/ReminderManager.h
    include/managers/StartupManager.h
    include/managers/TrayManager.h
    include/models/AppSettings.h
    resources/printercare.qrc
)

target_include_directories(PrinterCareSmoke
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(PrinterCareSmoke
    PRIVATE
        Qt6::Widgets
)

target_compile_definitions(PrinterCareSmoke
    PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

set_target_properties(PrinterCareSmoke
    PROPERTIES
        OUTPUT_NAME "PrinterCareSmoke"
)

enable_testing()
add_test(
    NAME PrinterCareSmoke
    COMMAND powershell -ExecutionPolicy Bypass -File ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run-dev.ps1 -Mode smoke -BuildDir ${CMAKE_BINARY_DIR}
)
