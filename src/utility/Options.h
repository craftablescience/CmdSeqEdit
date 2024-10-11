#pragma once

#include <string_view>

#include <QSettings>

// Options
constexpr std::string_view OPT_STYLE = "style";
constexpr std::string_view OPT_DISABLE_STEAM_SCANNER = "disable_steam_scanner";

// Storage
constexpr std::string_view STR_OPEN_RECENT = "open_recent";

namespace Options {

bool isStandalone();

void setupOptions(QSettings& options);

QSettings* getOptions();

template<typename T>
T get(std::string_view option) {
	return getOptions()->value(option).value<T>();
}

template<typename T>
void set(std::string_view option, T value) {
	getOptions()->setValue(option, value);
}

// Only use for booleans!
void invert(std::string_view option);

} // namespace Options
