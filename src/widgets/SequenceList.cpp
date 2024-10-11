#include "SequenceList.h"

#include <QMimeData>

#include "Window.h"

using namespace toolpp;

SequenceListModel::SequenceListModel(Window* window_, QObject* parent)
		: QAbstractListModel(parent)
		, window(window_) {}

int SequenceListModel::rowCount(const QModelIndex&) const {
	if (auto* sequences = this->getSequencesOrNullIfInvalid()) {
		return static_cast<int>(sequences->size());
	}
	return 0;
}

QVariant SequenceListModel::data(const QModelIndex& index, int role) const {
	auto* sequences = this->getSequencesOrNullIfInvalid();
	if (!sequences || !index.isValid() || index.parent().isValid() || index.row() >= sequences->size()) {
		return {};
	}
	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		return QString{sequences->at(index.row()).name.c_str()};
	}
	if (role == Qt::UserRole) {
		return QVariant::fromValue(sequences->at(index.row()));
	}
	return {};
}

bool SequenceListModel::setData(const QModelIndex& index, const QVariant& value, int role) {
	auto* sequences = this->getSequencesOrNullIfInvalid();
	if (!sequences || !index.isValid() || index.parent().isValid()) {
		return false;
	}
	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		sequences->at(index.row()).name = value.toString().toLocal8Bit().constData();
	} else if (role == Qt::UserRole) {
		sequences->at(index.row()) = value.value<CmdSeq::Sequence>();
	}
	emit this->dataChanged(index, index);
	return true;
}

Qt::ItemFlags SequenceListModel::flags(const QModelIndex& index) const {
	return index.isValid()
		? Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren
		: Qt::NoItemFlags;
}

void SequenceListModel::beginRefresh() {
	this->beginResetModel();
}

void SequenceListModel::endRefresh() {
	this->endResetModel();
}

std::vector<toolpp::CmdSeq::Sequence>* SequenceListModel::getSequencesOrNullIfInvalid() {
	if (auto* cmdSeq = this->window->getCmdSeq()) {
		return &cmdSeq->getSequences();
	}
	return nullptr;
}

const std::vector<toolpp::CmdSeq::Sequence>* SequenceListModel::getSequencesOrNullIfInvalid() const {
	if (auto* cmdSeq = this->window->getCmdSeq()) {
		return &cmdSeq->getSequences();
	}
	return nullptr;
}

SequenceListView::SequenceListView(Window* window, QWidget* parent)
		: QListView(parent)
		, model(new SequenceListModel{window, this}) {
	this->QAbstractItemView::setModel(this->model);
	this->setAlternatingRowColors(true);
	this->setSelectionMode(QAbstractItemView::SingleSelection);
	this->setWordWrap(true);
}

void SequenceListView::beginRefresh() const {
	this->model->beginRefresh();
}

void SequenceListView::endRefresh() const {
	this->model->endRefresh();
}
