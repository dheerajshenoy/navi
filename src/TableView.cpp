#include "TableView.hpp"

TableView::TableView(QWidget *parent) : QTableView(parent) {
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDefaultDropAction(Qt::CopyAction);
    setDragDropMode(QAbstractItemView::InternalMove);

    horizontalHeader()->setStretchLastSection(true);
    // horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
//setFrameShadow(QTableView::Shadow::Plain);
    //setFrameStyle(0);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    horizontalHeader()->setHidden(true);

}

TableView::~TableView() {}

void TableView::keyPressEvent(QKeyEvent *e) { e->ignore(); }


void TableView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_drag_start_position = event->pos();
    QTableView::mousePressEvent(event);
}

void TableView::mouseMoveEvent(QMouseEvent *event) {

    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - m_drag_start_position).manhattanLength()
        < QApplication::startDragDistance())
        return;

    emit dragRequested();
}