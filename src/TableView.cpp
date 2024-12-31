#include "TableView.hpp"

TableView::TableView(QWidget *parent) : QTableView(parent) {

    QFont font = this->font();
    font.setPixelSize(20);
    setFont(font);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDefaultDropAction(Qt::CopyAction);
    setDragDropMode(QAbstractItemView::InternalMove);

    m_header_view = new TableHeaderView(Qt::Orientation::Horizontal, this);
    m_vertical_header_view = new VerticalHeaderView(Qt::Orientation::Vertical, this);
    this->setHorizontalHeader(m_header_view);
    this->setVerticalHeader(m_vertical_header_view);
    //setFrameShadow(QTableView::Shadow::Plain);
    //setFrameStyle(0);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

    this->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
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

void TableView::wheelEvent(QWheelEvent *event) {
    event->ignore();
}
