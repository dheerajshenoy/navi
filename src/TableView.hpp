#pragma once

#include "pch/pch_tableview.hpp"
#include "FileSystemModel.hpp"
#include "TableHeaderView.hpp"
#include "VerticalHeaderView.hpp"

class TableView : public QTableView {
    Q_OBJECT

public:
  TableView(QWidget *parent = nullptr);
    ~TableView();

    inline void set_font_size(const int &size) noexcept {
        QFont _font = this->font();
        _font.setPixelSize(size);
        setFont(_font);
    }

    inline int get_font_size() noexcept {
        return font().pixelSize();
    }

    inline std::string get_grid_style() noexcept { return m_grid_style; }

    inline VerticalHeaderView* vheader() noexcept { return m_vertical_header_view; }

    void set_grid_style(const std::string &style) noexcept;

    QModelIndex scroll_to_middle() noexcept;

    void setCurrentIndex(const QModelIndex &index) noexcept;

signals:
    void dragRequested();
    void cursorPositionChanged(int row);

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

 private:
    QPoint m_drag_start_position;
    TableHeaderView *m_header_view;
    VerticalHeaderView *m_vertical_header_view;
    std::string m_grid_style = "solid";
};
