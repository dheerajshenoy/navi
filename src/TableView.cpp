#include "TableView.hpp"

TableView::TableView(QWidget *parent) : QTableView(parent) {
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDefaultDropAction(Qt::CopyAction);
    setDragDropMode(QTableView::DragDropMode::DragDrop);

    horizontalHeader()->setStretchLastSection(true);
    // horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    setFrameShadow(QTableView::Shadow::Plain);
    setFrameStyle(0);
    // setSelectionMode(QAbstractItemView::ExtendedSelection);
    // setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    horizontalHeader()->setHidden(true);

}

TableView::~TableView() {}

void TableView::keyPressEvent(QKeyEvent *e) { e->ignore(); }
