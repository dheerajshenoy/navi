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
#include <QMouseEvent>
#include <QApplication>

#include "FileSystemModel.hpp"
#include "TableHeaderView.hpp"
#include "HeaderProxyStyle.hpp"

class TableView : public QTableView {
    Q_OBJECT

public:
  TableView(QWidget *parent = nullptr);
    ~TableView();

signals:
    void dragRequested();

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

 private:
    QPoint m_drag_start_position;
    TableHeaderView *m_header_view = nullptr;
};
