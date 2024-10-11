#pragma once

#include <QAbstractItemModel>
#include <QListView>
#include <QMetaType>
#include <toolpp/toolpp.h>

Q_DECLARE_METATYPE(toolpp::CmdSeq::Sequence)

class Window;

class SequenceListModel : public QAbstractListModel {
	Q_OBJECT;

public:
	explicit SequenceListModel(Window* window_, QObject* parent = nullptr);

	[[nodiscard]] int rowCount(const QModelIndex&) const override;

	[[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;

	bool setData(const QModelIndex& index, const QVariant& value, int role) override;

	[[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;

	[[nodiscard]] Qt::DropActions supportedDropActions() const override;

	//bool insertRows(int row, int count, const QModelIndex& parent) override;

	//bool removeRows(int row, int count, const QModelIndex& parent) override;

	bool moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationRow) override;

	void beginRefresh();

	void endRefresh();

private:
	Window* window;
};

class SequenceListView : public QListView {
	Q_OBJECT;

public:
	explicit SequenceListView(Window* window, QWidget* parent = nullptr);

	void beginRefresh() const;

	void endRefresh() const;

private:
	SequenceListModel* model;
};
