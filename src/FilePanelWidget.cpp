#include "FilePanelWidget.hpp"

FilePanelWidget::FilePanelWidget(Statusbar *sb, Inputbar *ib,
                                 const QString &initDirectory, QWidget *parent)
: QWidget(parent) {

    m_file_panel = new FilePanel(ib, sb, initDirectory, parent);
    m_splitter = new QSplitter();
    m_layout = new QVBoxLayout();
    m_preview_panel = new PreviewPanel();
    m_splitter->addWidget(m_file_panel);
    m_splitter->addWidget(m_preview_panel);
    m_layout->addWidget(m_splitter);
    this->setLayout(m_layout);

    connect(m_file_panel, &FilePanel::currentItemChanged, m_preview_panel,
            &PreviewPanel::onFileSelected);

    connect(m_file_panel, &FilePanel::currentItemChanged, m_file_panel->m_statusbar,
            &Statusbar::SetFile);

    connect(m_file_panel, &FilePanel::fileOperationDone, this,
            [&](const bool &state, const QString &reason) {
                if (state)
                    m_file_panel->m_statusbar->Message("Operation Successful");
                else
                    m_file_panel->m_statusbar->Message(
                                         QString("Error during file operation! (%1)").arg(reason),
                                         MessageType::ERROR, 5);
            });
}

void FilePanelWidget::applyConfiguration(FilePanelWidget *widget) noexcept {
    if (m_file_pane_table.valid()) {
        auto file_panel = widget->filePanel();
        auto model = file_panel->model();
        sol::optional<sol::table> columns_table =
            m_file_pane_table["columns"];
        if (columns_table.has_value()) {
            QList<FileSystemModel::Column>
            columnList; // List to store column configuration
            FileSystemModel::Column column;
            bool file_name_type_check = false;
            sol::table columns = columns_table.value();
            for (std::size_t i = 1; i < columns.size(); i++) {
                auto col = columns[i];
                auto name =
                    QString::fromStdString(col["name"].get_or<std::string>(""));
                auto type =
                    QString::fromStdString(col["type"].get_or<std::string>(""));

                if (type == "file_name") {
                    column.type = FileSystemModel::ColumnType::FileName;
                    file_name_type_check = true;
                } else if (type == "file_size")
                    column.type = FileSystemModel::ColumnType::FileSize;
                else if (type == "file_date")
                    column.type = FileSystemModel::ColumnType::FileModifiedDate;
                else if (type == "file_permission")
                    column.type = FileSystemModel::ColumnType::FilePermission;
                else {
                    file_panel->m_statusbar->Message(QString("Unknown column type %1").arg(type),
                                         MessageType::WARNING);
                    continue;
                }

                column.name = name;
                columnList.append(column);
            }

            // If file_name type is not found in the configuration, inform the
            // user
            if (!file_name_type_check) {
                file_panel->m_statusbar->Message(
                                     "*file_name* key is mandatory in the columns table."
                                     "Consider adding it to get the columns working",
                                     MessageType::ERROR);
            }

            model->setColumns(columnList);
        }

        // headers
        sol::optional<bool> headers_visible = m_file_pane_table["headers"];
        if (headers_visible) {
            file_panel->ToggleHeaders(headers_visible.value());
        }

        // cycle
        sol::optional<bool> cycle = m_file_pane_table["cycle"];
        if (cycle) {
            file_panel->SetCycle(cycle.value());
        }

        // symlink
        sol::optional<sol::table> symlink_table = m_file_pane_table["symlink"];
        if (symlink_table) {
            auto symlink = symlink_table.value();

            auto shown = symlink["shown"].get_or(true);
            auto foreground = QString::fromStdString(
                                                     symlink["foreground"].get_or<std::string>(""));
            auto separator = QString::fromStdString(
                                                    symlink["separator"].get_or<std::string>("->"));

            model->setSymlinkVisible(shown);
            model->setSymlinkSeparator(separator);
            model->setSymlinkForeground(foreground);
        }

        // highlight
        sol::optional<sol::table> highlight_table =
            m_file_pane_table["highlight"];
        if (highlight_table) {
            auto highlight = highlight_table.value();

            auto foreground = QString::fromStdString(
                                                     highlight["foreground"].get_or<std::string>(""));
            auto background = QString::fromStdString(
                                                     highlight["background"].get_or<std::string>(""));

            if (!(foreground.isNull() || foreground.isEmpty())) {
                file_panel->setCurrentForeground(foreground);
            }

            if (!(background.isNull() || background.isEmpty())) {
                file_panel->setCurrentBackground(background);
            }
        }

        // marks
        sol::optional<sol::table> mark_table = m_file_pane_table["mark"];
        if (mark_table) {
            auto mark = mark_table.value();
            auto markBackground = QString::fromStdString(
                                                         mark["background"].get_or<std::string>(""));
            auto markForeground = QString::fromStdString(
                                                         mark["foreground"].get_or<std::string>(""));
            auto markFont =
                QString::fromStdString(mark["font"].get_or<std::string>(""));
            auto markItalic = mark["italic"].get_or(false);
            auto markBold = mark["bold"].get_or(false);

            if (!(markFont.isEmpty() || markFont.isNull()))
                model->setMarkHeaderFontFamily(markFont);

            if (markItalic)
                model->setMarkFontItalic(true);

            if (markBold)
                model->setMarkFontBold(true);

            if (!(markBackground.isNull() || markBackground.isEmpty()))
                model->setMarkBackgroundColor(markBackground);
            else
                model->setMarkBackgroundColor(file_panel->tableView()
                                              ->palette()
                                              .brush(QWidget::backgroundRole())
                                                                                .color()
                                              .name());

            if (!(markForeground.isNull() || markForeground.isEmpty()))
                model->setMarkForegroundColor(markForeground);
            else
                model->setMarkForegroundColor(file_panel->tableView()
                                              ->palette()
                                              .brush(QWidget::backgroundRole())
                                                                                .color()
                                              .name());

            // header
            sol::optional<sol::table> header_table = mark["header"];

            if (header_table) {
                auto header = header_table.value();

                auto markHeaderBackground = QString::fromStdString(
                                                                   header["background"].get_or<std::string>(""));
                auto markHeaderForeground = QString::fromStdString(
                                                                   header["foreground"].get_or<std::string>(""));

                auto markFont =
                    QString::fromStdString(header["font"].get_or<std::string>(""));
                auto markItalic = header["italic"].get_or(false);
                auto markBold = header["bold"].get_or(false);

                if (!(markFont.isEmpty() || markFont.isNull()))
                    model->setMarkHeaderFontFamily(markFont);

                if (markItalic)
                    model->setMarkHeaderFontItalic(true);

                if (markBold)
                    model->setMarkHeaderFontBold(true);

                if (!(markHeaderBackground.isNull() ||
                      markHeaderBackground.isEmpty()))
                    model->setMarkHeaderBackgroundColor(markHeaderBackground);
                else
                    model->setMarkHeaderBackgroundColor(
                                                        file_panel->tableView()
                      ->palette()
                      .brush(QWidget::backgroundRole())
                      .color()
                      .name());

                if (!(markHeaderForeground.isNull() ||
                      markHeaderForeground.isEmpty()))
                    model->setMarkHeaderForegroundColor(markHeaderForeground);
                else
                    model->setMarkHeaderForegroundColor(
                                                        file_panel->tableView()
                      ->palette()
                      .brush(QWidget::backgroundRole())
                      .color()
                      .name());
            }
        }
    }
}