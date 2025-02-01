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
    setMouseTracking(true);
    setShowGrid(true);


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

void TableView::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        m_drag_start_position = e->pos();
    }

    if (e->button() == Qt::RightButton) {
        setCurrentIndex(indexAt(e->pos()));
    }

    QModelIndex index = indexAt(e->pos());
    if (!index.isValid()) {
        QTableView::mousePressEvent(e);
        return;
    }

    QItemSelectionModel *selectionModel = this->selectionModel();

    if (e->modifiers() & Qt::ControlModifier) {  // Ctrl + Click handling
        if (selectionModel->isSelected(index)) {
            selectionModel->select(index, QItemSelectionModel::Deselect | QItemSelectionModel::Rows);
        } else {
            selectionModel->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
        }
    } else {
        QTableView::mousePressEvent(e);  // Default behavior
    }
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


QModelIndex TableView::scroll_to_middle() noexcept {
    if (this->model()->rowCount() == 0)
        return QModelIndex();

    // Get the vertical scrollbar
    QScrollBar *verticalScrollBar = this->verticalScrollBar();

    // Get the range of visible rows
    int firstVisibleRow = this->rowAt(0);
    int lastVisibleRow = this->rowAt(this->viewport()->height() - 1);

    // If the last row is -1, adjust it to the maximum row index
    if (lastVisibleRow == -1) {
        lastVisibleRow = this->model()->rowCount() - 1;
    }

    if (firstVisibleRow <= lastVisibleRow) {
        // Calculate the middle row index
        int middleRow = firstVisibleRow + (lastVisibleRow - firstVisibleRow) / 2;

        // Select the middle row
        QItemSelectionModel *selectionModel = this->selectionModel();
        if (selectionModel) {
            QModelIndex middleIndex = this->model()->index(middleRow, 0); // Assuming column 0
            selectionModel->select(
                middleIndex,
                QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows
            );

            // Optionally ensure the middle row is fully visible
            this->scrollTo(middleIndex);
            this->setCurrentIndex(middleIndex);
            return middleIndex;
        }
    }

    return QModelIndex();
}

void TableView::setCurrentIndex(const QModelIndex &index) noexcept {
    if (index.isValid()) {
        emit cursorPositionChanged(index.row());
    }
    QTableView::setCurrentIndex(index);
}


void TableView::set_grid_style(const std::string &style) noexcept {
    m_grid_style = style;

    if (style == "solid") {
        setGridStyle(Qt::PenStyle::SolidLine);
    } else if (style == "dotline") {
        setGridStyle(Qt::PenStyle::DotLine);
    } else if (style == "dashline") {
        setGridStyle(Qt::PenStyle::DashLine);
    } else if (style == "none") {
        setGridStyle(Qt::PenStyle::DashLine);
    } else if (style == "dashdotline") {
        setGridStyle(Qt::PenStyle::DashDotLine);
    } else if (style == "dashdotdotline") {
        setGridStyle(Qt::PenStyle::DashDotDotLine);
    } else {
        setGridStyle(Qt::PenStyle::CustomDashLine);
        // Custom Line
        // TODO: Handle custom line
    }

}
