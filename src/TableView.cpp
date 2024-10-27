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
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    horizontalHeader()->setHidden(true);
}

TableView::~TableView() {}

void TableView::keyPressEvent(QKeyEvent *e) { e->ignore(); }

void TableView::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void TableView::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void TableView::dropEvent(QDropEvent *event) {
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        for (const QUrl &url : mimeData->urls()) {
            QString sourcePath = url.toLocalFile();
            QFileSystemModel *model = qobject_cast<QFileSystemModel *>(this->model());

            if (!model)
                return;

            QString destPath = QFileInfo(model->rootPath()).absoluteFilePath();

            qDebug() << sourcePath << " " << destPath;

            qDebug() << event->proposedAction();
            // Move or copy the file
            if (event->proposedAction() == Qt::MoveAction) {
                dropCutRequested(sourcePath);
            } else if (event->proposedAction() == Qt::CopyAction) {
                dropCopyRequested(sourcePath);
            }
        }
        event->acceptProposedAction();
    }
}

void TableView::startDrag(Qt::DropActions supportedActions) {
    QModelIndexList selectedIndexes = selectionModel()->selectedIndexes();

    if (selectedIndexes.isEmpty())
        return;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    QFileSystemModel *model = qobject_cast<QFileSystemModel *>(this->model());

    if (!model)
        return;

    // Collect file paths
    QList<QUrl> urls;
    for (const QModelIndex &index : selectedIndexes) {
        QString filePath = model->rootPath() + QDir::separator() +
                           model->data(index, Qt::DisplayRole).toString();
        urls << QUrl::fromLocalFile(filePath);
    }

    qDebug() << urls;

    mimeData->setUrls(urls);

    drag->setMimeData(mimeData);
    drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
}
