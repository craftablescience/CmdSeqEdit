#include "Options.h"

#include <QApplication>
#include <QFileInfo>
#include <QStyle>

QSettings* opts = nullptr;

bool Options::isStandalone() {
	const QFileInfo nonportable(QApplication::applicationDirPath() + "/.nonportable");
	return !(nonportable.exists() && nonportable.isFile());
}

void Options::setupOptions(QSettings& options) {
	if (!options.contains(OPT_STYLE)) {
		options.setValue(OPT_STYLE, QApplication::style()->name());
	}
	QApplication::setStyle(options.value(OPT_STYLE).toString());

	if (!options.contains(OPT_DISABLE_STEAM_SCANNER)) {
		options.setValue(OPT_DISABLE_STEAM_SCANNER, false);
	}

	if (!options.contains(STR_OPEN_RECENT)) {
		options.setValue(STR_OPEN_RECENT, QStringList{});
	}

	opts = &options;
}

QSettings* Options::getOptions() {
	return opts;
}

void Options::invert(std::string_view option) {
	set(option, !get<bool>(option));
}
