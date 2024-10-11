#include "Window.h"

#include <cstdlib>
#include <ranges>

#include <kvpp/kvpp.h>
#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QSplitter>
#include <QStyle>
#include <QStyleFactory>
#include <QThread>
#include <QVBoxLayout>
#include <steampp/steampp.h>

#include "../utility/Options.h"
#include "../utility/TGADecoder.h"
#include "../Version.h"

using namespace kvpp;
using namespace steampp;
using namespace toolpp;

Window::Window(QWidget* parent)
		: QMainWindow(parent)
		, cmdSeq(std::nullopt)
		, modified(false)
		, scanSteamGamesWorkerThread(nullptr) {
	this->setWindowTitle(PROJECT_TITLE.data());
	this->setWindowIcon(QIcon(":/logo.png"));

	// File menu
	auto* fileMenu = this->menuBar()->addMenu(tr("File"));
	{
		fileMenu->addAction(this->style()->standardIcon(QStyle::SP_FileIcon), tr("Create"), Qt::CTRL | Qt::Key_N, [this] {
			this->createNewFile();
		});

		fileMenu->addAction(this->style()->standardIcon(QStyle::SP_DirIcon), tr("Open"), Qt::CTRL | Qt::Key_O, [this] {
			this->openFile();
		});

		this->openRelativeToMenu = fileMenu->addMenu(this->style()->standardIcon(QStyle::SP_DirLinkIcon), tr("Open In"));
		this->rebuildOpenInMenu();

		this->openRecentMenu = fileMenu->addMenu(this->style()->standardIcon(QStyle::SP_DirLinkIcon), tr("Open Recent"));
		this->rebuildOpenRecentMenu(Options::get<QStringList>(STR_OPEN_RECENT));

		this->saveAction = fileMenu->addAction(this->style()->standardIcon(QStyle::SP_DialogSaveButton), tr("Save"), Qt::CTRL | Qt::Key_S, [this] {
			this->saveFile(false);
		});

		this->saveAsAction = fileMenu->addAction(this->style()->standardIcon(QStyle::SP_DialogSaveButton), tr("Save As"), Qt::CTRL | Qt::SHIFT | Qt::Key_S, [this] {
			this->saveFile(true);
		});

		this->closeAction = fileMenu->addAction(this->style()->standardIcon(QStyle::SP_BrowserReload), tr("Close"), Qt::CTRL | Qt::Key_W, [this] {
			this->closeFile();
		});

		fileMenu->addSeparator();
		fileMenu->addAction(this->style()->standardIcon(QStyle::SP_DialogHelpButton), tr("Donate On Ko-fi"), [] {
			QDesktopServices::openUrl({"https://ko-fi.com/craftablescience"});
		});

		fileMenu->addAction(this->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Exit"), Qt::ALT | Qt::Key_F4, [this] {
			this->close();
		});
	}

	// Options menu
	auto* optionsMenu = this->menuBar()->addMenu(tr("Options"));
	{
		auto* themeMenu = optionsMenu->addMenu(this->style()->standardIcon(QStyle::SP_DesktopIcon), tr("Theme"));
		auto* themeMenuGroup = new QActionGroup{themeMenu};
		themeMenuGroup->setExclusive(true);
		for (const auto& themeName : QStyleFactory::keys()) {
			auto* action = themeMenu->addAction(themeName, [this, themeName] {
				QApplication::setStyle(themeName);
				Options::set(OPT_STYLE, themeName);
			});
			action->setCheckable(true);
			if (themeName == Options::get<QString>(OPT_STYLE)) {
				action->setChecked(true);
			}
			themeMenuGroup->addAction(action);
		}

		optionsMenu->addSeparator();

		auto* openInEnableAction = optionsMenu->addAction(tr("Disable Open In Menu"), [this] {
			Options::invert(OPT_DISABLE_STEAM_SCANNER);
			this->rebuildOpenInMenu();
		});
		openInEnableAction->setCheckable(true);
		openInEnableAction->setChecked(Options::get<bool>(OPT_DISABLE_STEAM_SCANNER));
	}

	// Help menu
	auto* helpMenu = this->menuBar()->addMenu(tr("Help"));
	helpMenu->addAction(this->style()->standardIcon(QStyle::SP_DialogHelpButton), tr("About"), Qt::Key_F1, [this] {
		this->about();
	});
	helpMenu->addAction(this->style()->standardIcon(QStyle::SP_DialogHelpButton), tr("About Qt"), Qt::ALT | Qt::Key_F1, [this] {
		QMessageBox::aboutQt(this);
	});

	// Split content into two resizeable panes
	auto* splitter = new QSplitter{Qt::Horizontal, this};
	this->setCentralWidget(splitter);

	// Left pane
	auto* leftPane = new QWidget{splitter};
	auto* leftPaneLayout = new QVBoxLayout{leftPane};
	leftPaneLayout->setContentsMargins(4, 4, 0, 0);

	//this->entryTree = new EntryTree(this, leftPane);
	//leftPaneLayout->addWidget(this->entryTree);

	splitter->addWidget(leftPane);

	// Right pane
	auto* rightPane = new QWidget{splitter};
	auto* rightPaneLayout = new QVBoxLayout{rightPane};
	rightPaneLayout->setContentsMargins(0, 4, 4, 0);

	//this->fileViewer = new FileViewer(this, rightPane);
	//rightPaneLayout->addWidget(this->fileViewer);

	splitter->addWidget(rightPane);

	// Clear everything
	this->closeFile();

	// Load the pack file if given one through the command-line or double-clicking a file
	// An error here means shut the application down
	if (const auto& args = QApplication::arguments(); args.length() > 1 && QFile::exists(args[1]) && !this->loadFile(args[1])) {
		std::exit(1);
	}
}

bool Window::promptUserToKeepModifications() {
	if (!this->modified) {
		return false;
	}
	const auto response = QMessageBox::warning(this,
			tr("Save changes?"),
			tr("This file has unsaved changes! Would you like to save these changes first?"),
			QMessageBox::Ok | QMessageBox::Discard | QMessageBox::Cancel);
	switch (response) {
		case QMessageBox::Cancel:
			return true;
		case QMessageBox::Discard:
			return false;
		case QMessageBox::Ok:
			this->saveFile(false);
			return false;
		default:
			break;
	}
	return true;
}

void Window::dragEnterEvent(QDragEnterEvent* event) {
	if (!event->mimeData()->hasUrls() || !event->mimeData()->urls()[0].isLocalFile()) {
		return;
	}
	if (const auto path = event->mimeData()->urls()[0].path(); path.endsWith(".wc") || path.endsWith(".WC")) {
		event->acceptProposedAction();
	}
}

void Window::dropEvent(QDropEvent* event) {
	if (!this->cmdSeq || !event->mimeData()->hasUrls()) {
		return;
	}

	if (this->promptUserToKeepModifications()) {
		event->ignore();
		return;
	}
	this->loadFile(event->mimeData()->urls()[0].toLocalFile());
}

void Window::closeEvent(QCloseEvent* event) {
	if (this->promptUserToKeepModifications()) {
		event->ignore();
		return;
	}

	// Write location and sizing
	auto* settings = Options::getOptions();
	settings->beginGroup("main_window");
	settings->setValue("geometry", this->saveGeometry());
	settings->setValue("state", this->saveState());
	settings->setValue("maximized", this->isMaximized());
	if (!this->isMaximized()) {
		settings->setValue("position", this->pos());
		settings->setValue("size", this->size());
	}
	settings->endGroup();

	event->accept();
}

void Window::createNewFile() {
	// todo: create new
}

void Window::openFile(const QString& startDir) {
	const auto path = QFileDialog::getOpenFileName(this, tr("Open Worldcraft Command Sequences File"), startDir, "Supported Files (*.wc)");
	if (path.isEmpty()) {
		return;
	}
	this->loadFile(path);
}

bool Window::loadFile(const QString& path) {
	if (!this->closeFile()) {
		return false;
	}

	this->cmdSeq = CmdSeq{path.toLocal8Bit().constData()};
	if (!this->cmdSeq.value()) {
		this->cmdSeq = std::nullopt;
		return false;
	}

	// Add to recent paths
	if (auto recentPaths = Options::get<QStringList>(STR_OPEN_RECENT); !recentPaths.contains(path)) {
		recentPaths.push_front(path);
		if (recentPaths.size() > 10) {
			recentPaths.pop_back();
		}
		Options::set(STR_OPEN_RECENT, recentPaths);
		this->rebuildOpenRecentMenu(recentPaths);
	} else if (const auto pathIndex = recentPaths.indexOf(path); pathIndex > 0) {
		recentPaths.remove(pathIndex);
		recentPaths.push_front(path);
		Options::set(STR_OPEN_RECENT, recentPaths);
		this->rebuildOpenRecentMenu(recentPaths);
	}

	// todo: load

	this->markModified(false);
	return true;
}

void Window::saveFile(bool saveAs) {
	(void) saveAs;

	// todo: save

	this->markModified(false);
}

bool Window::closeFile() {
	if (this->promptUserToKeepModifications()) {
		return false;
	}

	if (!this->cmdSeq) {
		this->updateMenuState();
		return true;
	}
	this->cmdSeq = std::nullopt;

	// todo: close

	this->markModified(false);
	return true;
}

void Window::about() {
	QMessageBox::about(this, tr("About"), tr("%1\n\nCreated by craftablescience\nCopyright (c) 2024, Laura Lewis\n\nBuilt using the SourcePP parser collection").arg(PROJECT_TITLE.data()));
}

void Window::rebuildOpenInMenu() {
	this->openRelativeToMenu->clear();
	auto* loadingGamesAction = this->openRelativeToMenu->addAction(tr("Loading installed games..."));
	loadingGamesAction->setDisabled(true);

	// Set up thread
	this->scanSteamGamesWorkerThread = new QThread{this};
	auto* worker = new ScanSteamGamesWorker{};
	worker->moveToThread(this->scanSteamGamesWorkerThread);
	// NOLINTNEXTLINE(*-unused-return-value)
	QObject::connect(this->scanSteamGamesWorkerThread, &QThread::started, worker, [worker] {
		worker->run();
	});
	// NOLINTNEXTLINE(*-unused-return-value)
	QObject::connect(worker, &ScanSteamGamesWorker::taskFinished, this, [this](const QList<std::tuple<QString, QIcon, QDir>>& sourceGames) {
		// Add them to the menu
		this->openRelativeToMenu->clear();
		if (!sourceGames.empty()) {
			for (const auto& [gameName, icon, relativeDirectoryPath] : sourceGames) {
				const auto relativeDirectory = relativeDirectoryPath.path();
				this->openRelativeToMenu->addAction(icon, gameName, [this, relativeDirectory] {
					this->openFile(relativeDirectory);
				});
			}
		} else {
			auto* noGamesDetectedAction = this->openRelativeToMenu->addAction(tr("No games detected."));
			noGamesDetectedAction->setDisabled(true);
		}

		// Kill thread
		this->scanSteamGamesWorkerThread->quit();
		this->scanSteamGamesWorkerThread->wait();
		delete this->scanSteamGamesWorkerThread;
		this->scanSteamGamesWorkerThread = nullptr;
	});
	this->scanSteamGamesWorkerThread->start();
}

void Window::rebuildOpenRecentMenu(const QStringList& paths) {
	this->openRecentMenu->clear();
	if (paths.empty()) {
		auto* openRecentVPKMenuNoRecentFilesAction = this->openRecentMenu->addAction(tr("No recent files."));
		openRecentVPKMenuNoRecentFilesAction->setDisabled(true);
		return;
	}
	for (int i = 0; i < paths.size(); i++) {
		this->openRecentMenu->addAction(("&%1: \"" + paths[i] + "\"").arg((i + 1) % 10), [this, path=paths[i]] {
			this->loadFile(path);
		});
	}
	this->openRecentMenu->addSeparator();
	this->openRecentMenu->addAction(tr("Clear"), [this] {
		Options::set(STR_OPEN_RECENT, QStringList{});
		this->rebuildOpenRecentMenu({});
	});
}

void Window::markModified(bool mod) {
	this->modified = mod;
	this->updateMenuState();

	if (this->modified) {
		this->setWindowTitle(PROJECT_TITLE.data() + QString{" (*)"});
	} else {
		this->setWindowTitle(PROJECT_TITLE.data());
	}
}

void Window::updateMenuState() const {
	if (!this->cmdSeq) {
		this->saveAction->setDisabled(true);
		this->saveAsAction->setDisabled(true);
		this->closeAction->setDisabled(true);
	} else {
		this->saveAction->setDisabled(!this->modified);
		this->saveAsAction->setDisabled(!this->modified);
		this->closeAction->setDisabled(false);
	}
}

void ScanSteamGamesWorker::run() {
	QList<std::tuple<QString, QIcon, QDir>> sourceGames;

	if (Options::get<bool>(OPT_DISABLE_STEAM_SCANNER)) {
		emit this->taskFinished(sourceGames);
		return;
	}

	Steam steam;
	if (!steam) {
		emit this->taskFinished(sourceGames);
		return;
	}

	// Add Steam games
	for (auto appID : steam.getInstalledApps()) {
		if (!steam.isAppUsingSourceEngine(appID) && !steam.isAppUsingSource2Engine(appID)) {
			continue;
		}
		auto iconPath = steam.getAppIconPath(appID);
		sourceGames.emplace_back(steam.getAppName(appID).data(), iconPath.empty() ? QIcon(":/icons/missing_app.png") : QIcon(iconPath.c_str()), steam.getAppInstallDir(appID).c_str());
	}

	// Add mods in the sourcemods directory
	for (const auto& modDir : std::filesystem::directory_iterator{steam.getSourceModDir()}) {
		if (!modDir.is_directory()) {
			continue;
		}

		const auto gameInfoPath = (modDir.path() / "gameinfo.txt").string();
		if (!std::filesystem::exists(gameInfoPath)) {
			continue;
		}

		std::ifstream gameInfoFile{gameInfoPath};
		auto gameInfoSize = std::filesystem::file_size(gameInfoPath);
		std::string gameInfoData;
		gameInfoData.resize(gameInfoSize);
		gameInfoFile.read(gameInfoData.data(), static_cast<std::streamsize>(gameInfoSize));

		KV1 gameInfoRoot{gameInfoData};
		if (gameInfoRoot.isInvalid()) {
			continue;
		}
		const auto& gameInfo = gameInfoRoot["GameInfo"];
		if (gameInfo.isInvalid()) {
			continue;
		}
		const auto& gameInfoName = gameInfo["game"];
		const auto& gameInfoIconPath = gameInfo["icon"];

		std::string modName;
		if (!gameInfoName.isInvalid()) {
			modName = gameInfoName.getValue();
		} else {
			modName = std::filesystem::path{gameInfoPath}.parent_path().filename().string();
		}

		std::string modIconPath;
		if (!gameInfoIconPath.isInvalid()) {
			if (auto modIconBigPath = modDir.path() / (std::string{gameInfoIconPath.getValue()} + "_big.tga"); std::filesystem::exists(modIconBigPath)) {
				modIconPath = modIconBigPath.string();
			} else if (auto modIconRegularPath = modDir.path() / (std::string{gameInfoIconPath.getValue()} + ".tga"); std::filesystem::exists(modIconRegularPath)) {
				modIconPath = modIconRegularPath.string();
			}
		}

		std::optional<QImage> modIconTGA = modIconPath.empty() ? std::nullopt : TGADecoder::decodeImage(modIconPath.c_str());
		sourceGames.emplace_back(modName.c_str(), modIconTGA ? QIcon(QPixmap::fromImage(*modIconTGA)) : QIcon(":/icons/missing_app.png"), modDir.path().string().c_str());
	}

	// Replace & with && in game names
	for (auto& games : sourceGames) {
		// Having an & before a character makes that the shortcut character and hides the &, so we need to escape it
		std::get<0>(games).replace("&", "&&");
	}

	// Sort games and return
	std::ranges::sort(sourceGames, [](const auto& lhs, const auto& rhs) {
		return std::get<0>(lhs) < std::get<0>(rhs);
	});
	emit this->taskFinished(sourceGames);
}
