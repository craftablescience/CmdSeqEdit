#include "CommandList.h"

#include <QMimeData>

#include "SequenceList.h"
#include "Window.h"

using namespace toolpp;

CommandListModel::CommandListModel(const QItemSelectionModel* sequenceListSelectionModel, Window* window_, QObject* parent)
		: QAbstractListModel(parent)
		, window(window_)
		, selectedSequence(-1) {
	// NOLINTNEXTLINE(*-unused-return-value)
	QObject::connect(sequenceListSelectionModel, &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& selected, const QItemSelection&) {
		this->beginRefresh();
		if (selected.indexes().isEmpty()) {
			this->selectedSequence = -1;
		} else {
			this->selectedSequence = selected.indexes().first().row();
		}
		this->endRefresh();
	});
}

int CommandListModel::rowCount(const QModelIndex&) const {
	if (const auto* commands = this->getCommandsOrNullIfInvalid()) {
		return static_cast<int>(commands->size());
	}
	return 0;
}

QVariant CommandListModel::data(const QModelIndex& index, int role) const {
	auto* commands = this->getCommandsOrNullIfInvalid();
	if (!commands || !index.isValid() || index.parent().isValid() || index.row() >= commands->size()) {
		return {};
	}
	if (role == Qt::DisplayRole) {
		return QString{commands->at(index.row()).getExecutableDisplayName().c_str()};
	}
	if (role == Qt::UserRole) {
		return QVariant::fromValue(commands->at(index.row()));
	}
	return {};
}

bool CommandListModel::setData(const QModelIndex& index, const QVariant& value, int role) {
	auto* commands = this->getCommandsOrNullIfInvalid();
	if (!commands || !index.isValid() || index.parent().isValid()) {
		return false;
	}
	if (role == Qt::UserRole) {
		commands->at(index.row()) = value.value<CmdSeq::Command>();
	}
	emit this->dataChanged(index, index);
	return true;
}

Qt::ItemFlags CommandListModel::flags(const QModelIndex& index) const {
	return index.isValid()
		? Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren
		: Qt::NoItemFlags;
}

void CommandListModel::beginRefresh() {
	this->beginResetModel();
}

void CommandListModel::endRefresh() {
	this->endResetModel();
}

void CommandListModel::setSelectedSequence(int sequenceIndex) {
	this->beginRefresh();
	this->selectedSequence = sequenceIndex;
	this->endRefresh();
}

std::vector<CmdSeq::Command>* CommandListModel::getCommandsOrNullIfInvalid() {
	if (auto* cmdSeq = this->window->getCmdSeq(); cmdSeq && this->selectedSequence >= 0 && cmdSeq->getSequences().size() > this->selectedSequence) {
		return &cmdSeq->getSequences()[this->selectedSequence].commands;
	}
	return nullptr;
}

const std::vector<CmdSeq::Command>* CommandListModel::getCommandsOrNullIfInvalid() const {
	if (auto* cmdSeq = this->window->getCmdSeq(); cmdSeq && this->selectedSequence >= 0 && cmdSeq->getSequences().size() > this->selectedSequence) {
		return &cmdSeq->getSequences()[this->selectedSequence].commands;
	}
	return nullptr;
}

CommandListView::CommandListView(const QItemSelectionModel* sequenceListSelectionModel, Window* window, QWidget* parent)
		: QListView(parent)
		, model(new CommandListModel{sequenceListSelectionModel, window, this}) {
	this->QAbstractItemView::setModel(this->model);
	this->setAlternatingRowColors(true);
	this->setSelectionMode(QAbstractItemView::SingleSelection);
	this->setWordWrap(true);
}

void CommandListView::setSelectedSequence(int sequenceIndex) const {
	this->model->setSelectedSequence(sequenceIndex);
}

void CommandListView::beginRefresh() const {
	this->model->beginRefresh();
}

void CommandListView::endRefresh() const {
	this->model->endRefresh();
}
