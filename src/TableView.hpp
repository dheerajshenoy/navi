#pragma once

#include <QDrag>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QFile>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QKeyEvent>
#include <QTableView>
#include <QMimeData>
#include <QMimeDatabase>
#include <QObject>
#include <Qt>
#include <QHeaderView>

class TableView : public QTableView {
    Q_OBJECT

public:
  TableView(QWidget *parent = nullptr);
    ~TableView();

protected:
    void keyPressEvent(QKeyEvent *e) override;
};
