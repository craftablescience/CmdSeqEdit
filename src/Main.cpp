#include <memory>

#include <QApplication>
#include <QSettings>

#include "utility/Options.h"
#include "widgets/Window.h"
#include "Version.h"

int main(int argc, char** argv) {
    QApplication app{argc, argv};

    QCoreApplication::setOrganizationName(ORGANIZATION_NAME.data());
    QCoreApplication::setApplicationName(PROJECT_NAME.data());
    QCoreApplication::setApplicationVersion(PROJECT_VERSION.data());

#if !defined(__APPLE__) && !defined(_WIN32)
    QGuiApplication::setDesktopFileName(PROJECT_NAME.data());
#endif

    std::unique_ptr<QSettings> options;
    if (Options::isStandalone()) {
        auto configPath = QApplication::applicationDirPath() + "/config.ini";
        options = std::make_unique<QSettings>(configPath, QSettings::Format::IniFormat);
    } else {
        options = std::make_unique<QSettings>();
    }
    Options::setupOptions(*options);

    auto* window = new Window{};
    options->beginGroup("main_window");
    window->restoreGeometry(options->value("geometry", window->saveGeometry()).toByteArray());
    window->restoreState(options->value("state", window->saveState()).toByteArray());
    if (options->contains("position")) {
        window->move(options->value("position", window->pos()).toPoint());
    }
    window->resize(options->value("size", QSize{900, 500}).toSize());
    if (options->value("maximized", window->isMaximized()).toBool()) {
        window->showMaximized();
    } else {
        window->show();
    }
    options->endGroup();

    return QApplication::exec();
}
