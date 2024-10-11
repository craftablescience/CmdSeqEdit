#pragma once

#include <optional>

#include <QDir>
#include <QMainWindow>
#include <toolpp/CmdSeq.h>

class QAction;
class QMenu;
class QThread;

class Window : public QMainWindow {
	Q_OBJECT;

public:
	explicit Window(QWidget* parent = nullptr);

	[[nodiscard]] bool promptUserToKeepModifications();

protected:
	void dragEnterEvent(QDragEnterEvent* event) override;

	void dropEvent(QDropEvent* event) override;

	void closeEvent(QCloseEvent* event) override;

private:
	void createNewFile();

	void openFile(const QString& startDir = "");

	bool loadFile(const QString& path);

	void saveFile(bool saveAs);

	bool closeFile();

	void about();

	void rebuildOpenInMenu();

	void rebuildOpenRecentMenu(const QStringList& paths);

	void markModified(bool mod);

	void updateMenuState() const;

	std::optional<toolpp::CmdSeq> cmdSeq;

	QMenu*   openRelativeToMenu;
	QMenu*   openRecentMenu;
	QAction* saveAction;
	QAction* saveAsAction;
	QAction* closeAction;

	QThread* scanSteamGamesWorkerThread;
};

class ScanSteamGamesWorker : public QObject {
	Q_OBJECT;

public:
	ScanSteamGamesWorker() = default;

	void run();

signals:
	void taskFinished(const QList<std::tuple<QString, QIcon, QDir>>& sourceGames);
};
