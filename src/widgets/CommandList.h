#pragma once

#include <string_view>

#include <QAbstractItemModel>
#include <QListView>
#include <QMetaType>
#include <toolpp/toolpp.h>

Q_DECLARE_METATYPE(toolpp::CmdSeq::Command)

class QItemSelectionModel;
class Window;

class CommandListModel : public QAbstractListModel {
	Q_OBJECT;

public:
	CommandListModel(const QItemSelectionModel* sequenceListSelectionModel, Window* window_, QObject* parent = nullptr);

	[[nodiscard]] int rowCount(const QModelIndex&) const override;

	[[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;

	bool setData(const QModelIndex& index, const QVariant& value, int role) override;

	[[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;

	void beginRefresh();

	void endRefresh();

	void setSelectedSequence(int sequenceIndex);

private:
	[[nodiscard]] std::vector<toolpp::CmdSeq::Command>* getCommandsOrNullIfInvalid();

	[[nodiscard]] const std::vector<toolpp::CmdSeq::Command>* getCommandsOrNullIfInvalid() const;

	Window* window;
	int selectedSequence;
};

class CommandListView : public QListView {
	Q_OBJECT;

public:
	CommandListView(const QItemSelectionModel* sequenceListSelectionModel, Window* window, QWidget* parent = nullptr);

	void setSelectedSequence(int sequenceIndex) const;

	void beginRefresh() const;

	void endRefresh() const;

private:
	CommandListModel* model;
};
