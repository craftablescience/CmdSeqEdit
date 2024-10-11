#include "SequenceList.h"

#include "Window.h"

using namespace toolpp;

SequenceListModel::SequenceListModel(Window* window_, QObject* parent)
		: QAbstractListModel(parent)
		, window(window_) {}

int SequenceListModel::rowCount(const QModelIndex&) const {
	if (auto* cmdSeq = this->window->getCmdSeq()) {
		return static_cast<int>(cmdSeq->getSequences().size());
	}
	return 0;
}

QVariant SequenceListModel::data(const QModelIndex& index, int role) const {
	auto* cmdSeq = this->window->getCmdSeq();
	if (!cmdSeq || !index.isValid() || index.row() >= cmdSeq->getSequences().size()) {
		return {};
	}
	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		return QString{cmdSeq->getSequences().at(index.row()).name.c_str()};
	} else if (role == Qt::UserRole) {
		return QVariant::fromValue(cmdSeq->getSequences().at(index.row()));
	}
	return {};
}

bool SequenceListModel::setData(const QModelIndex& index, const QVariant& value, int role) {
	auto* cmdSeq = this->window->getCmdSeq();
	if (!cmdSeq || !index.isValid()) {
		return false;
	}
	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		cmdSeq->getSequences().at(index.row()).name = value.toString().toLocal8Bit().constData();
	} else if (role == Qt::UserRole) {
		cmdSeq->getSequences().at(index.row()).commands = value.value<std::vector<CmdSeq::Command>>();
	}
	emit this->dataChanged(index, index);
	return true;
}

Qt::ItemFlags SequenceListModel::flags(const QModelIndex& index) const {
	return index.isValid()
		? Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren
		: Qt::NoItemFlags;
}

Qt::DropActions SequenceListModel::supportedDropActions() const {
	return Qt::CopyAction | Qt::MoveAction;
}
/*
bool SequenceListModel::insertRows(int row, int count, const QModelIndex& parent) {
	auto* cmdSeq = this->window->getCmdSeq();
	if (!cmdSeq || row < 0 || row > cmdSeq->getSequences().size() || count <= 0) {
		return false;
	}

	this->beginInsertRows(parent, row, row + count - 1);

	for (int i = 0; i < count; ++i) {
		cmdSeq->getSequences().insert(cmdSeq->getSequences().begin() + row, {
			.name = "New Command Sequence",
			.commands = {},
		});
	}

	this->endInsertRows();
	return true;
}

bool SequenceListModel::removeRows(int row, int count, const QModelIndex& parent) {
	auto* cmdSeq = this->window->getCmdSeq();
	if (!cmdSeq || row < 0 || row >= cmdSeq->getSequences().size() || (row + count) > cmdSeq->getSequences().size() || count <= 0) {
		return false;
	}

	this->beginRemoveRows(parent, row, row + count - 1);

	cmdSeq->getSequences().erase(cmdSeq->getSequences().begin() + row, cmdSeq->getSequences().begin() + row + count);

	this->endRemoveRows();
	return true;
}
*/
bool SequenceListModel::moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationRow) {
	auto* cmdSeq = this->window->getCmdSeq();
	if (!cmdSeq || sourceRow < 0 || sourceRow >= cmdSeq->getSequences().size() || count <= 0 || sourceRow + count > cmdSeq->getSequences().size() || destinationRow < 0 || destinationRow > cmdSeq->getSequences().size() || (destinationRow >= sourceRow && destinationRow < sourceRow + count)) {
		return false;
	}

	this->beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationRow);

	std::vector<CmdSeq::Sequence> movingData;
	for (int i = 0; i < count; i++) {
		movingData.push_back(cmdSeq->getSequences().at(sourceRow + i));
	}

	cmdSeq->getSequences().erase(cmdSeq->getSequences().begin() + sourceRow, cmdSeq->getSequences().begin() + sourceRow + count);
	int insertPosition = destinationRow;
	if (destinationRow > sourceRow) {
		insertPosition -= count;
	}
	cmdSeq->getSequences().insert(cmdSeq->getSequences().begin() + insertPosition, movingData.begin(), movingData.end());

	this->endMoveRows();
	return true;
}

void SequenceListModel::beginRefresh() {
	this->beginResetModel();
}

void SequenceListModel::endRefresh() {
	this->endResetModel();
}

SequenceListView::SequenceListView(Window* window, QWidget* parent)
		: QListView(parent)
		, model(new SequenceListModel{window, this}) {
	this->setModel(this->model);
	this->viewport()->setAcceptDrops(true);
	this->setDragDropMode(QAbstractItemView::InternalMove);
	this->setDropIndicatorShown(true);
	this->setSelectionMode(QAbstractItemView::SingleSelection);
}

void SequenceListView::beginRefresh() const {
	this->model->beginRefresh();
}

void SequenceListView::endRefresh() const {
	this->model->endRefresh();
}
