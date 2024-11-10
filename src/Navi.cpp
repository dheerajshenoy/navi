#include "Navi.hpp"
#include "DriveWidget.hpp"
#include "FilePanelWidget.hpp"
#include "argparse.hpp"
#include "sol/sol.hpp"

Navi::Navi(QWidget *parent) : QMainWindow(parent) {}

void Navi::initThings() noexcept {
    initLayout();       // init layout
    initMenubar();      // init menubar
    initSignalsSlots(); // init signals and slots
    setupCommandMap();
    initBookmarks();
    initTabs();

    if (m_load_config)
        initConfiguration();
    else
        initKeybinds();

    if (!m_default_location_list.isEmpty())
        getCurrentFilePanel()->setCurrentDir(m_default_location_list.at(0), true);
}

FilePanel *Navi::getCurrentFilePanel() noexcept {
    if (m_tab_widget->count() != 0) {
        auto filePanelWidget = qobject_cast<FilePanelWidget *>(m_tab_widget->currentWidget());
        return filePanelWidget->filePanel();
    }
    else
        return nullptr;
}

void Navi::initTabs() noexcept {
    // m_tab_bar->addTab(new FilePanel(m_inputbar, m_statusbar), );
    connect(m_tab_widget, &QTabWidget::tabBarDoubleClicked, this,
            [&]() { this->AddTab(); });
    connect(m_tab_widget, &QTabWidget::tabCloseRequested, this,
            [&](const int &index) {
              // TODO: handle tab close
            });

    connect(m_tab_widget, &QTabWidget::currentChanged, this,
            [&](const int &index) {
                auto file_panel = getCurrentFilePanel();
                m_statusbar->SetFile(file_panel->getCurrentItem());
                m_statusbar->SetNumItems(file_panel->getNumItems());
                m_file_path_widget->setCurrentDir(file_panel->getCurrentDir());
            });

    m_tab_widget->setTabPosition(QTabWidget::TabPosition::North);
    m_tab_widget->setTabsClosable(true);
    firstTab("~");
}

void Navi::AddTab(const QString &directory) noexcept {
  auto filepanelwidget =
      qobject_cast<FilePanelWidget *>(m_tab_widget->widget(0))
          ->clone(m_statusbar, m_inputbar, directory);

    connect(filepanelwidget->filePanel(), &FilePanel::afterDirChange, m_file_path_widget,
            &FilePathWidget::setCurrentDir);

    connect(m_file_path_widget, &FilePathWidget::directoryChangeRequested,
            filepanelwidget->filePanel(),
            [&](const QString &path) { getCurrentFilePanel()->setCurrentDir(path); });

    connect(filepanelwidget->filePanel()->model(),
            &FileSystemModel::marksListChanged, m_marks_buffer,
            &MarksBuffer::refreshMarksList);

    m_tab_widget->addTab(filepanelwidget, directory);
}

void Navi::firstTab(const QString &directory) noexcept {
    auto filepanelwidget =
        new FilePanelWidget(m_statusbar, m_inputbar, directory, this);

    connect(filepanelwidget->filePanel(), &FilePanel::afterDirChange, m_file_path_widget,
            &FilePathWidget::setCurrentDir);

    connect(m_file_path_widget, &FilePathWidget::directoryChangeRequested,
            filepanelwidget->filePanel(),
            [&](const QString &path) { getCurrentFilePanel()->setCurrentDir(path); });

    connect(filepanelwidget->filePanel()->model(),
            &FileSystemModel::marksListChanged, m_marks_buffer,
            &MarksBuffer::refreshMarksList);

    m_tab_widget->addTab(filepanelwidget, directory);
}

void Navi::initConfiguration() noexcept {

    lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::string);

    try {
        lua.safe_script_file(m_config_location.toStdString(), sol::load_mode::any);
    } catch (const sol::error &e) {
        m_statusbar->Message(QString::fromStdString(e.what()));
        initKeybinds();

        return;
    }

    auto currentTab = getCurrentTab();
    auto file_panel = currentTab->filePanel();
    auto model = file_panel->model();
    auto preview_panel = currentTab->previewPanel();
    auto splitter = currentTab->splitter();

    // Read the SETTINGS table
    sol::optional<sol::table> settings_table_opt = lua["settings"];

    if (settings_table_opt) {
        sol::table settings_table = settings_table_opt.value();
        sol::optional<sol::table> ui_table_opt = settings_table["ui"];
        if (ui_table_opt) {
            sol::table ui_table = ui_table_opt.value();

            // tabs
            sol::optional<sol::table> tabs_table = ui_table["tabs"];

            if (tabs_table) {
                sol::table tabs = tabs_table.value();

                auto shown_on_multiple = tabs["shown_on_multiple"].get_or(true);

                // TODO: shown only on multiple tabs
                // if (!shown_on_multiple)
                //     m_tab_bar->setVisible(true);
            }

            // Preview pane
            sol::optional<sol::table> preview_pane_table = ui_table["preview_pane"];

            if (preview_pane_table) {
                auto preview_pane = preview_pane_table.value();
                auto shown = preview_pane["shown"].get_or(true);
                // TogglePreviewPanel(shown);
                preview_panel->show();

                auto fraction = preview_pane["fraction"].get_or(0.5);
                auto totalSize = splitter->width();
                QList<int> sizes = {static_cast<int>(totalSize * (1 - fraction)),
                            static_cast<int>(totalSize * fraction)};
                splitter->setSizes(sizes);

                auto max_file_size = QString::fromStdString(preview_pane["max_size"].get_or<std::string>("10M"));

                auto max_file_bytes = utils::parseFileSize(max_file_size);
                preview_panel->SetMaxPreviewThreshold(max_file_bytes);

                auto syntax_highlighting =
                    preview_pane["syntax_highlight"].get_or(true);
                preview_panel->SetSyntaxHighlighting(syntax_highlighting);
            }

            // Menu bar
            sol::optional<sol::table> menu_bar_table = ui_table["menu_bar"];

            if (menu_bar_table) {
                auto menu_bar = menu_bar_table.value();
                auto shown = menu_bar["shown"].get_or(true);
                ToggleMenuBar(shown);
            }

            // Status bar
            sol::optional<sol::table> status_bar_table = ui_table["status_bar"];

            if (status_bar_table) {
                auto status_bar = status_bar_table.value();
                auto shown = status_bar["shown"].get_or(true);
                ToggleStatusBar(shown);
            }

            // File Pane
            sol::optional<sol::table> file_pane_table_opt =
                ui_table["file_pane"];

            if (file_pane_table_opt) {
                auto file_pane_table = file_pane_table_opt.value();
                if (file_pane_table.valid()) {
                    currentTab->SaveConfiguration(file_pane_table);
                    sol::optional<sol::table> columns_table =
                        file_pane_table["columns"];
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
                                m_statusbar->Message(QString("Unknown column type %1").arg(type),
                                                     MessageType::WARNING);
                                continue;
                            }

                            column.name = name;
                            columnList.append(column);
                        }

                        // If file_name type is not found in the configuration, inform the
                        // user
                        if (!file_name_type_check) {
                            m_statusbar->Message(
                                                 "*file_name* key is mandatory in the columns table."
                                                 "Consider adding it to get the columns working",
                                                 MessageType::ERROR);
                        }

                        model->setColumns(columnList);
                    }

                    // headers
                    sol::optional<bool> headers_visible = file_pane_table["headers"];
                    if (headers_visible) {
                        file_panel->ToggleHeaders(headers_visible.value());
                    }

                    // cycle
                    sol::optional<bool> cycle = file_pane_table["cycle"];
                    if (cycle) {
                        file_panel->SetCycle(cycle.value());
                    }

                    // symlink
                    sol::optional<sol::table> symlink_table = file_pane_table["symlink"];
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
                        file_pane_table["highlight"];
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
                    sol::optional<sol::table> mark_table = file_pane_table["mark"];
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

                // input_bar
                sol::optional<sol::table> input_bar_table = ui_table["input_bar"];

                if (input_bar_table) {
                    auto input_bar = input_bar_table.value();

                    auto foregroundColor = QString::fromStdString(
                                                                  input_bar["foreground"].get_or<std::string>(""));
                    auto backgroundColor = QString::fromStdString(
                                                                  input_bar["background"].get_or<std::string>(""));
                    auto font =
                        QString::fromStdString(input_bar["font"].get_or<std::string>(""));

                    // TODO: background unable to apply
                    m_inputbar->setForeground(foregroundColor);
                    m_inputbar->setBackground(backgroundColor);
                    m_inputbar->setFontFamily(font);
                }
            }
        }
    }

    // Read the KEYBINDINGS table
    sol::optional<sol::table> keybindings_table_opt = lua["keybindings"];

    if (keybindings_table_opt) {

        sol::table keybindings_table = keybindings_table_opt.value();
        m_keybind_list.reserve(keybindings_table.size());
        for (std::size_t i = 1; i <= keybindings_table.size(); i++) {
            sol::table entry = keybindings_table[i];
            if (!entry)
                continue;

            auto key = QString::fromStdString(entry["key"].get_or<std::string>(""));

            if (key.isEmpty() || key.isNull())
                continue;

            auto command =
                QString::fromStdString(entry["command"].get_or<std::string>(""));
            auto desc = QString::fromStdString(entry["desc"].get_or<std::string>(""));

            if (command.isEmpty() || command.isNull())
                continue;

            Keybind kb;
            kb.key = key;
            kb.command = command;
            kb.desc = desc;

            m_keybind_list.append(kb);
        }
        generateKeybinds();
    }

    m_statusbar->Message("Reading configuration file done!");
}

// Function to create Qt keybindings from list of ‘Keybinds’ struct
void Navi::generateKeybinds() noexcept {

    for (const auto &keybind : m_keybind_list) {
        QShortcut *shortcut = new QShortcut(QKeySequence(keybind.key), this);
        connect(shortcut, &QShortcut::activated, this,
                [&]() { ProcessCommand(keybind.command); });
    }
}

void Navi::initBookmarks() noexcept {
    // TODO: load bookmarks from config directory
    m_bookmark_manager = new BookmarkManager();
}

// Handle signals and slots
void Navi::initSignalsSlots() noexcept {


    connect(m_drives_widget, &DriveWidget::driveLoadRequested, this,
            [&](const QString &mountPoint) {
                getCurrentFilePanel()->setCurrentDir(mountPoint);
            });
    connect(m_drives_widget, &DriveWidget::driveMountRequested, this,
            [&](const QString &driveName) {
                QString confirm = m_inputbar->getInput(
                                                       QString("Do you want to mount %1 ? (y, N)").arg(driveName));
                if (confirm == "n" || confirm.isNull() || confirm.isEmpty())
                    return;
                MountDrive(driveName);
            });

    connect(m_drives_widget, &DriveWidget::driveUnmountRequested, this,
            [&](const QString &driveName) {
                QString confirm =
                    m_inputbar->getInput(QString("Do you want to unmount %1 ? (y, N)").arg(driveName));
                if (confirm == "n" || confirm.isNull() || confirm.isEmpty())
                    return;
                UnmountDrive(driveName);
            });

    // connect(m_tab_widget, &FilePanel::afterDirChange, m_file_path_widget,
    //         &FilePathWidget::setCurrentDir);

    connect(m_statusbar, &Statusbar::logMessage, this, &Navi::LogMessage);

}

// Help for HELP interactive function
void Navi::ShowHelp() noexcept {}

// Setup the commandMap HashMap with the function calls
void Navi::setupCommandMap() noexcept {

  commandMap["tab-new"] = [this](const QStringList &args) {
    if (args.isEmpty())
      AddTab();
    else
      AddTab(args.at(0));
  };

  commandMap["tab-delete"] = [this](const QStringList &args) {
      if (args.isEmpty()) {
          auto widget = m_tab_widget->currentWidget();
          int index = m_tab_widget->currentIndex();
          widget->close();
          m_tab_widget->removeTab(index);
      }
      else {
          bool ok;
          int index = args.at(0).toInt(&ok);
          if (ok)
              m_tab_widget->widget(index);
          else
              m_statusbar->Message("Index should be a number", MessageType::WARNING);
      }
  };

  commandMap["tab-next"] = [this](const QStringList &args) {
      int index = m_tab_widget->currentIndex() + 1;
      if (index < m_tab_widget->count())
          m_tab_widget->setCurrentIndex(index);
  };

  commandMap["tab-prev"] = [this](const QStringList &args) {
      int index = m_tab_widget->currentIndex() - 1;
      if (index >= 0)
          m_tab_widget->setCurrentIndex(index);
  };
  commandMap["tab-select"] = [this](const QStringList &args) {
      if (args.isEmpty())
          return;
      bool ok;
      int index = args.at(0).toInt(&ok);
      if (ok)
          m_tab_widget->setCurrentIndex(index);
      else
          m_statusbar->Message("Wrong index", MessageType::WARNING);
  };


  commandMap["execute-extended-command"] = [this](const QStringList &args) {
    ExecuteExtendedCommand();
  };

  commandMap["syntax-highlight"] = [this](const QStringList &args) {
      m_preview_panel->ToggleSyntaxHighlight();
    };

    commandMap["drives"] = [this](const QStringList &args) {
        ToggleDrivesWidget();
    };

    commandMap["get-input"] = [this](const QStringList &args) {
        if (args.isEmpty())
            return;
        auto size = args.size();
        switch (size) {
        case 1:
            m_inputbar->getInput(args.at(0));
            break;

        case 2:
            m_inputbar->getInput(args.at(0), args.at(1));
            break;

        case 3:
            m_inputbar->getInput(args.at(0), args.at(1), args.at(2));
            break;
        }

        // TODO: redirect the output somehow
    };

    commandMap["mouse-scroll"] = [this](const QStringList &args) {
        getCurrentFilePanel()->ToggleMouseScroll();
    };

    commandMap["visual-select"] = [this](const QStringList &args) {
        getCurrentFilePanel()->ToggleVisualLine();
    };

    commandMap["shortcuts-pane"] = [this](const QStringList &args) {
        ToggleShortcutsBuffer();
    };

    commandMap["up-directory"] = [this](const QStringList &args) {
        getCurrentFilePanel()->UpDirectory();
    };

    commandMap["select-item"] = [this](const QStringList &args) {
        getCurrentFilePanel()->SelectItem();
    };

    commandMap["next-item"] = [this](const QStringList &args) {
        getCurrentFilePanel()->NextItem();
    };

    commandMap["prev-item"] = [this](const QStringList &args) {
        getCurrentFilePanel()->PrevItem();
    };

    commandMap["first-item"] = [this](const QStringList &args) {
        getCurrentFilePanel()->GotoFirstItem();
    };

    commandMap["last-item"] = [this](const QStringList &args) {
        getCurrentFilePanel()->GotoLastItem();
    };

    commandMap["middle-item"] = [this](const QStringList &args) {
        getCurrentFilePanel()->GotoMiddleItem();
    };

    commandMap["echo-info"] = [this](const QStringList &args) {
        if (args.isEmpty())
            return;

        m_statusbar->Message(args.join(" "), MessageType::INFO);
    };

    commandMap["echo-warn"] = [this](const QStringList &args) {
        if (args.isEmpty())
            return;

        m_statusbar->Message(args.join(" "), MessageType::WARNING);
    };

    commandMap["echo-error"] = [this](const QStringList &args) {
        if (args.isEmpty())
            return;

        m_statusbar->Message(args.join(" "), MessageType::ERROR);
    };

    commandMap["reload-config"] = [this](const QStringList &args) {
        initConfiguration();
    };

    commandMap["sort-name"] = [this](const QStringList &args) { SortByName(); };

    commandMap["sort-date"] = [this](const QStringList &args) { SortByDate(); };

    commandMap["sort-size"] = [this](const QStringList &args) { SortBySize(); };

    commandMap["sort-name-desc"] = [this](const QStringList &args) {
        SortByName(true);
    };

    commandMap["sort-date-desc"] = [this](const QStringList &args) {
        SortByDate(true);
    };

    commandMap["sort-size-desc"] = [this](const QStringList &args) {
        SortBySize(true);
    };

    commandMap["cycle"] = [this](const QStringList &args) {
        getCurrentFilePanel()->ToggleCycle();
    };

    commandMap["header"] = [this](const QStringList &args) {
        getCurrentFilePanel()->ToggleHeaders();
    };

    commandMap["rename"] = [this](const QStringList &args) {
        getCurrentFilePanel()->RenameItem();
    };

    commandMap["rename-global"] = [this](const QStringList &args) {
        getCurrentFilePanel()->RenameItemsGlobal();
    };

    commandMap["rename-local"] = [this](const QStringList &args) {
        getCurrentFilePanel()->RenameItemsLocal();
    };

    commandMap["rename-dwim"] = [this](const QStringList &args) {
        getCurrentFilePanel()->RenameDWIM();
    };

    commandMap["help"] = [this](const QStringList &args) { ShowHelp(); };

    commandMap["copy"] = [this](const QStringList &args) {
        getCurrentFilePanel()->CopyItem();
    };

    commandMap["copy-global"] = [this](const QStringList &args) {
        getCurrentFilePanel()->CopyItemsGlobal();
    };

    commandMap["copy-local"] = [this](const QStringList &args) {
        getCurrentFilePanel()->CopyItemsLocal();
    };

    commandMap["copy-dwim"] = [this](const QStringList &args) {
        getCurrentFilePanel()->CopyDWIM();
    };

    commandMap["cut"] = [this](const QStringList &args) {
        getCurrentFilePanel()->CutItem();
    };

    commandMap["cut-global"] = [this](const QStringList &args) {
        getCurrentFilePanel()->CutItemsGlobal();
    };

    commandMap["cut-local"] = [this](const QStringList &args) {
        getCurrentFilePanel()->CutItemsLocal();
    };

    commandMap["cut-dwim"] = [this](const QStringList &args) {
        getCurrentFilePanel()->CutDWIM();
    };

    commandMap["paste"] = [this](const QStringList &args) {
        getCurrentFilePanel()->PasteItems();
    };

    commandMap["delete"] = [this](const QStringList &args) {
        getCurrentFilePanel()->DeleteItem();
    };

    commandMap["delete-global"] = [this](const QStringList &args) {
        getCurrentFilePanel()->DeleteItemsGlobal();
    };

    commandMap["delete-local"] = [this](const QStringList &args) {
        getCurrentFilePanel()->DeleteItemsLocal();
    };

    commandMap["delete-dwim"] = [this](const QStringList &args) {
        getCurrentFilePanel()->DeleteDWIM();
    };

    commandMap["mark"] = [this](const QStringList &args) {
        getCurrentFilePanel()->MarkItem();
    };

    commandMap["mark-inverse"] = [this](const QStringList &args) {
        getCurrentFilePanel()->MarkInverse();
    };

    commandMap["mark-all"] = [this](const QStringList &args) {
        getCurrentFilePanel()->MarkAllItems();
    };

    commandMap["mark-dwim"] = [this](const QStringList &args) {
        getCurrentFilePanel()->MarkDWIM();
    };

    commandMap["toggle-mark"] = [this](const QStringList &args) {
        getCurrentFilePanel()->ToggleMarkItem();
    };

    commandMap["toggle-mark-dwim"] = [this](const QStringList &args) {
        getCurrentFilePanel()->ToggleMarkDWIM();
    };

    commandMap["unmark"] = [this](const QStringList &args) {
        getCurrentFilePanel()->UnmarkItem();
    };

    commandMap["unmark-local"] = [this](const QStringList &args) {
        getCurrentFilePanel()->UnmarkItemsLocal();
    };

    commandMap["unmark-global"] = [this](const QStringList &args) {
        getCurrentFilePanel()->UnmarkItemsGlobal();
    };

    commandMap["new-file"] = [this](const QStringList &args) {
        getCurrentFilePanel()->NewFile(args);
    };

    commandMap["new-folder"] = [this](const QStringList &args) {
        getCurrentFilePanel()->NewFolder(args);
    };

    commandMap["trash"] = [this](const QStringList &args) {
        getCurrentFilePanel()->TrashItem();
    };

    commandMap["trash-local"] = [this](const QStringList &args) {
        getCurrentFilePanel()->TrashItemsLocal();
    };

    commandMap["trash-global"] = [this](const QStringList &args) {
        getCurrentFilePanel()->TrashItemsGlobal();
    };

    commandMap["trash-dwim"] = [this](const QStringList &args) {
        getCurrentFilePanel()->TrashDWIM();
    };

    commandMap["exit"] = [this](const QStringList &args) {
        QApplication::quit();
    };

    commandMap["messages-pane"] = [this](const QStringList &args) {
        ToggleMessagesBuffer();
    };

    commandMap["hidden-files"] = [this](const QStringList &args) {
        ToggleHiddenFiles();
    };

    commandMap["dot-dot"] = [this](const QStringList &args) { ToggleDotDot(); };

    commandMap["preview-pane"] = [this](const QStringList &args) {
        TogglePreviewPanel();
    };

    commandMap["menu-bar"] = [this](const QStringList &args) { ToggleMenuBar(); };

    commandMap["filter"] = [this](const QStringList &args) { Filter(); };

    commandMap["reset-filter"] = [this](const QStringList &args) {
        ResetFilter();
    };

    commandMap["refresh"] = [this](const QStringList &args) {
        getCurrentFilePanel()->ForceUpdate();
    };

    commandMap["chmod"] = [this](const QStringList &args) {
        getCurrentFilePanel()->ChmodItem();
    };

    commandMap["chmod-local"] = [this](const QStringList &args) {
        getCurrentFilePanel()->ChmodItemsLocal();
    };

    commandMap["chmod-global"] = [this](const QStringList &args) {
        getCurrentFilePanel()->ChmodItemsGlobal();
    };

    commandMap["marks-pane"] = [this](const QStringList &args) {
        ToggleMarksBuffer();
    };

    commandMap["focus-path"] = [&](const QStringList &args) {
        m_file_path_widget->FocusLineEdit();
    };

    commandMap["item-property"] = [&](const QStringList &args) {
        getCurrentFilePanel()->ItemProperty();
    };

    commandMap["bookmarks-pane"] = [&](const QStringList &args) {
        // ToggleBookmarksBuffer();
    };

    commandMap["bookmark-go"] = [&](const QStringList &args) {
        GoBookmark(args);
    };

    commandMap["bookmark-add"] = [&](const QStringList &args) {
        AddBookmark(args);
    };

    commandMap["bookmark-remove"] = [&](const QStringList &args) {
        RemoveBookmark(args);
    };

    commandMap["bookmark-edit"] = [&](const QStringList &args) {
        EditBookmark(args);
    };

    commandMap["bookmarks-save"] = [&](const QStringList &args) {
        SaveBookmarkFile();
    };

    commandMap["search"] = [&](const QStringList &args) {
        getCurrentFilePanel()->Search();
    };
    commandMap["search-next"] = [&](const QStringList &args) {
        getCurrentFilePanel()->SearchNext();
    };
    commandMap["search-prev"] = [&](const QStringList &args) {
        getCurrentFilePanel()->SearchPrev();
    };

    m_command_completion_model = new QStringListModel(m_valid_command_list);
    m_inputbar->addCompleterModel(m_command_completion_model,
                                  Inputbar::CompletionModelType::COMMAND);

    m_search_completion_model = new QStringListModel(m_search_history_list);
    m_inputbar->addCompleterModel(m_search_completion_model, Inputbar::CompletionModelType::SEARCH);
}

void Navi::EditBookmark(const QStringList &args) noexcept {

    // TODO: interactive
    if (args.isEmpty() || args.size() < 2)
        return;

    // Change can be title or path for bookmark title or
    // file path that the bookmark points to
    QString changeType = args.at(0).toLower();

    QString bookmarkName = args.at(1);

    if (changeType == "title") {
        QString newBookmarkName =
            m_inputbar->getInput("New bookmark title", bookmarkName);

        // If the bookmark title name is null, do nothing and return
        if (newBookmarkName.isEmpty() || newBookmarkName.isNull()) {
            m_statusbar->Message("Error: No bookmark title provided!",
                                 MessageType::ERROR, 5);
            return;
        }

        // If bookmark title is provided
        if (m_bookmark_manager->setBookmarkName(bookmarkName, newBookmarkName)) {
            m_statusbar->Message(QString("Bookmark title changed from %1 to %2")
                               .arg(bookmarkName)
                               .arg(newBookmarkName));
            return;
        } else {
            m_statusbar->Message("Error changing bookmark title name!",
                                 MessageType::ERROR, 5);
            return;
        }
    } else if (changeType == "path") {
        QString newBookmarkPath =
            m_inputbar->getInput(QString("New bookmark path (Default: %1)")
                                 .arg(getCurrentFilePanel()->getCurrentDir()),
                                 bookmarkName);

        // If the bookmark title name is null, do nothing and return
        if (newBookmarkPath.isEmpty() || newBookmarkPath.isNull()) {
            m_statusbar->Message("Error: No bookmark path provided!",
                                 MessageType::ERROR, 5);
            return;
        }

        // If bookmark title is provided
        QString oldBookmarkPath =
            m_bookmark_manager->getBookmarkFilePath(bookmarkName);
        if (m_bookmark_manager->setBookmarkFile(bookmarkName, newBookmarkPath)) {
            m_statusbar->Message(QString("Bookmark file path changed from %1 to %2")
                               .arg(oldBookmarkPath)
                               .arg(newBookmarkPath));
            return;
        } else {
            m_statusbar->Message("Error changing bookmark title name!",
                                 MessageType::ERROR, 5);
            return;
        }
    }
}

void Navi::AddBookmark(const QStringList &args) noexcept {

    // If no arg is supplied
    if (args.isEmpty()) {
    } else {
        QString bookmarkName = args.at(0);
        if (m_bookmark_manager->addBookmark(bookmarkName,
                                            getCurrentFilePanel()->getCurrentDir())) {
            m_statusbar->Message("Added bookmark");
        } else
            m_statusbar->Message("Error adding bookmark!", MessageType::ERROR, 5);
    }
}

void Navi::RemoveBookmark(const QStringList &args) noexcept {

    // TODO: Interactive removal
    if (args.isEmpty())
        return;

    QString bookmarkName = args.at(0);
    if (m_bookmark_manager->removeBookmark(bookmarkName))
        m_statusbar->Message(QString("Bookmark %1 removed!").arg(bookmarkName));
    else
        m_statusbar->Message(
                             QString("Error removing bookmark %1").arg(bookmarkName),
                             MessageType::ERROR, 5);
}

void Navi::LoadBookmarkFile(const QStringList &args) noexcept {

    // TODO: Interactive
    if (args.isEmpty())
        return;

    QString bookmarkFileName = args.at(0);
    if (m_bookmark_manager->loadBookmarks(bookmarkFileName))
        m_statusbar->Message(
                             QString("Bookmark loaded from %1!").arg(bookmarkFileName));
    else
        m_statusbar->Message(
                             QString("Error loading bookmark file %1").arg(bookmarkFileName),
                             MessageType::ERROR, 5);
}

void Navi::GoBookmark(const QStringList &bookmarkName) noexcept {

    // TODO: interactive
    if (bookmarkName.isEmpty())
        return;

    QString bookmarkPath = m_bookmark_manager->getBookmark(bookmarkName.at(0));
    if (bookmarkPath.isNull() || bookmarkPath.isEmpty())
        m_statusbar->Message(
                             QString("Bookmark %1 not found!").arg(bookmarkName.at(0)),
                             MessageType::ERROR, 5);
    else
        getCurrentFilePanel()->setCurrentDir(bookmarkPath, true);
}

void Navi::ToggleBookmarksBuffer() noexcept {
    if (m_bookmarks_buffer->isVisible()) {
        m_bookmarks_buffer->hide();
        delete m_bookmarks_buffer;
        m_bookmarks_buffer = nullptr;
        disconnect(m_bookmarks_buffer, &BookmarkWidget::visibilityChanged, 0, 0);
    } else {
        m_bookmarks_buffer = new BookmarkWidget(m_bookmark_manager);
        m_bookmarks_buffer->show();
        connect(m_bookmarks_buffer, &BookmarkWidget::visibilityChanged, this,
                [&](const bool &state) {
                    m_viewmenu__bookmarks_buffer->setChecked(state);
                });
    }
}

void Navi::ToggleBookmarksBuffer(const bool &state) noexcept {
    if (state) {
        m_bookmarks_buffer = new BookmarkWidget(m_bookmark_manager);
        m_bookmarks_buffer->show();
        connect(m_bookmarks_buffer, &BookmarkWidget::visibilityChanged, this,
                [&](const bool &state) {
                    m_viewmenu__bookmarks_buffer->setChecked(state);
                });
    } else {
        m_bookmarks_buffer->hide();
        delete m_bookmarks_buffer;
        m_bookmarks_buffer = nullptr;
        disconnect(m_bookmarks_buffer, &BookmarkWidget::visibilityChanged, 0, 0);
    }
}

void Navi::ToggleShortcutsBuffer() noexcept {
    if (m_shortcuts_widget && m_shortcuts_widget->isVisible()) {
        m_shortcuts_widget->close();
        delete m_shortcuts_widget;
        m_shortcuts_widget = nullptr;
        disconnect(m_shortcuts_widget, &ShortcutsWidget::visibilityChanged, 0, 0);
    } else {
        m_shortcuts_widget = new ShortcutsWidget(m_keybind_list, this);
        connect(m_shortcuts_widget, &ShortcutsWidget::visibilityChanged, this,
                [&](const bool &state) {
                    m_viewmenu__shortcuts_widget->setChecked(state);
                });
        m_shortcuts_widget->show();
    }
}

void Navi::ToggleShortcutsBuffer(const bool &state) noexcept {
    if (state)
        m_log_buffer->show();
    else
        m_log_buffer->hide();
}

void Navi::ToggleMessagesBuffer() noexcept {
    if (m_log_buffer->isVisible())
        m_log_buffer->hide();
    else
        m_log_buffer->show();
}

void Navi::ToggleMessagesBuffer(const bool &state) noexcept {
    if (state)
        m_log_buffer->show();
    else
        m_log_buffer->hide();
}

void Navi::ToggleMarksBuffer() noexcept {
    if (m_marks_buffer->isVisible())
        m_marks_buffer->hide();
    else
        m_marks_buffer->show();
}

void Navi::ToggleMarksBuffer(const bool &state) noexcept {
    if (state)
        m_marks_buffer->show();
    else
        m_marks_buffer->hide();
}

// Minibuffer process commands
void Navi::ProcessCommand(const QString &commandtext) noexcept {
    // QStringList commandlist = commandtext.split(" && ");
    QStringList commandlist =
        commandtext.split(QRegularExpression("\\s*&&\\s*"), Qt::SkipEmptyParts);

    if (commandlist.isEmpty())
        return;

    auto [isNumber, num] = utils::isNumber(commandlist.at(0));
    if (isNumber) {
        getCurrentFilePanel()->GotoItem(num);
        return;
    }

    //        COMMAND1       &&        COMMAND2
    // SUBCOMMAND1 ARG1 ARG2 && SUBCOMMAND2 ARG1 ARG2

    for (const auto &commands : commandlist) {
        QStringList command = utils::splitPreservingQuotes(commands);
        QString subcommand = command.takeFirst();
        QStringList args = command;

        if (commandMap.contains(subcommand)) {
            commandMap[subcommand](args); // Call the associated function
        } else {
            m_statusbar->Message(
                                 QString("Command %1 is not a valid interactive command")
              .arg(subcommand),
                                 MessageType::ERROR, 5);
        }
    }
}

void Navi::TogglePreviewPanel(const bool &state) noexcept {
    auto tab = getCurrentTab();
    auto previewPanel = tab->previewPanel();
    if (state) {
        previewPanel->show();
        connect(tab->filePanel(), &FilePanel::currentItemChanged, previewPanel,
                &PreviewPanel::onFileSelected);
        m_preview_panel->onFileSelected(getCurrentFile());
    } else {
        previewPanel->hide();
        disconnect(tab->filePanel(), &FilePanel::currentItemChanged, previewPanel,
                   &PreviewPanel::onFileSelected);
    }
}

void Navi::TogglePreviewPanel() noexcept {
    auto tab = getCurrentTab();
    auto previewPanel = tab->previewPanel();
    auto filePanel = tab->filePanel();
    bool visible = previewPanel->isVisible();
    if (visible) {
        previewPanel->hide();
        disconnect(filePanel, &FilePanel::currentItemChanged, previewPanel,
                   &PreviewPanel::onFileSelected);
    } else {
        previewPanel->show();
        connect(filePanel, &FilePanel::currentItemChanged, previewPanel,
                &PreviewPanel::onFileSelected);
        previewPanel->onFileSelected(getCurrentFile());
    }
}

QString Navi::getCurrentFile() noexcept {
    return getCurrentFilePanel()->getCurrentItem();
}

void Navi::initLayout() noexcept {
    m_inputbar = new Inputbar();
    m_statusbar = new Statusbar();
    m_tab_widget = new TabWidget();

    // m_tab_bar->setVisible(false);

    m_preview_panel = new PreviewPanel();
    m_file_path_widget = new FilePathWidget();
    m_log_buffer = new MessagesBuffer(this);
    m_marks_buffer = new MarksBuffer(this);

    // m_marks_buffer->setMarksSet(m_file_panel->getMarksSetPTR());

    m_inputbar->hide();

    m_file_path_widget->setContentsMargins(0, 0, 0, 0);
    this->setContentsMargins(0, 0, 0, 0);

    m_layout->addWidget(m_file_path_widget);
    m_layout->addWidget(m_tab_widget);
    m_layout->addWidget(m_inputbar);
    m_layout->addWidget(m_statusbar);

    m_widget->setLayout(m_layout);
    this->setCentralWidget(m_widget);
    this->show();
}

void Navi::initKeybinds() noexcept {

    QShortcut *kb_up_directory = new QShortcut(QKeySequence("h"), this);
    QShortcut *kb_next_item = new QShortcut(QKeySequence("j"), this);
    QShortcut *kb_prev_item = new QShortcut(QKeySequence("k"), this);
    QShortcut *kb_select_item = new QShortcut(QKeySequence("l"), this);
    QShortcut *kb_goto_first_item = new QShortcut(QKeySequence("g,g"), this);
    QShortcut *kb_goto_last_item = new QShortcut(QKeySequence("Shift+g"), this);

    QShortcut *kb_mark_item = new QShortcut(QKeySequence("Space"), this);
    QShortcut *kb_mark_inverse = new QShortcut(QKeySequence("Shift+Space"), this);
    QShortcut *kb_mark_all = new QShortcut(QKeySequence("Ctrl+a"), this);

    QShortcut *kb_command = new QShortcut(QKeySequence(":"), this);
    QShortcut *kb_rename_items = new QShortcut(QKeySequence("Shift+r"), this);
    QShortcut *kb_delete_items = new QShortcut(QKeySequence("Shift+d"), this);
    QShortcut *kb_copy_items = new QShortcut(QKeySequence("y,y"), this);
    QShortcut *kb_paste_items = new QShortcut(QKeySequence("p"), this);
    QShortcut *kb_unmark_items_local =
        new QShortcut(QKeySequence("Shift+u"), this);
    QShortcut *kb_toggle_hidden_files = new QShortcut(QKeySequence("."), this);

    QShortcut *kb_search = new QShortcut(QKeySequence("/"), this);
    QShortcut *kb_search_next = new QShortcut(QKeySequence("n"), this);
    QShortcut *kb_search_prev = new QShortcut(QKeySequence("Shift+n"), this);
    QShortcut *kb_toggle_menubar = new QShortcut(QKeySequence("Ctrl+m"), this);
    QShortcut *kb_toggle_preview_panel =
        new QShortcut(QKeySequence("Ctrl+p"), this);
    QShortcut *kb_focus_file_path_widget =
        new QShortcut(QKeySequence("Ctrl+l"), this);

    QShortcut *kb_visual_line = new QShortcut(QKeySequence("Shift+v"), this);

    connect(kb_visual_line, &QShortcut::activated, this,
            [&]() { getCurrentFilePanel()->ToggleVisualLine(); });

    connect(kb_mark_item, &QShortcut::activated, getCurrentFilePanel(),
            &FilePanel::ToggleMarkDWIM);

    connect(kb_mark_inverse, &QShortcut::activated, getCurrentFilePanel(),
            &FilePanel::MarkInverse);

    connect(kb_mark_all, &QShortcut::activated, getCurrentFilePanel(),
            &FilePanel::MarkAllItems);

    connect(kb_next_item, &QShortcut::activated, getCurrentFilePanel(),
            &FilePanel::NextItem);

    connect(kb_prev_item, &QShortcut::activated, getCurrentFilePanel(),
            &FilePanel::PrevItem);

    connect(kb_select_item, &QShortcut::activated, getCurrentFilePanel(),
            &FilePanel::SelectItem);

    connect(kb_up_directory, &QShortcut::activated, getCurrentFilePanel(),
            &FilePanel::UpDirectory);

    connect(kb_goto_last_item, &QShortcut::activated, getCurrentFilePanel(),
            &FilePanel::GotoLastItem);

    connect(kb_goto_first_item, &QShortcut::activated, getCurrentFilePanel(),
            &FilePanel::GotoFirstItem);

    connect(kb_command, &QShortcut::activated, this,
            &Navi::ExecuteExtendedCommand);

    connect(kb_rename_items, &QShortcut::activated, this,
            [&]() { getCurrentFilePanel()->RenameDWIM(); });

    connect(kb_delete_items, &QShortcut::activated, this,
            [&]() { getCurrentFilePanel()->DeleteDWIM(); });

    connect(kb_search, &QShortcut::activated, getCurrentFilePanel(),
            [&]() { Search(); });

    connect(kb_search_next, &QShortcut::activated, getCurrentFilePanel(),
            &FilePanel::SearchNext);

    connect(kb_search_prev, &QShortcut::activated, getCurrentFilePanel(),
            &FilePanel::SearchPrev);

    connect(kb_toggle_menubar, &QShortcut::activated, this,
            [this]() { ToggleMenuBar(); });

    connect(kb_toggle_preview_panel, &QShortcut::activated, this,
            [this]() { TogglePreviewPanel(); });

    connect(kb_focus_file_path_widget, &QShortcut::activated, this,
            [this]() { m_file_path_widget->FocusLineEdit(); });

    connect(kb_paste_items, &QShortcut::activated, getCurrentFilePanel(),
            &FilePanel::PasteItems);

    connect(kb_copy_items, &QShortcut::activated, getCurrentFilePanel(),
            &FilePanel::CopyItem);

    connect(kb_unmark_items_local, &QShortcut::activated, this,
            [&]() { getCurrentFilePanel()->UnmarkItemsLocal(); });

    connect(kb_toggle_hidden_files, &QShortcut::activated, this,
            [&]() { ToggleHiddenFiles(); });
}

void Navi::ExecuteExtendedCommand() noexcept {
    // m_inputbar->enableCommandCompletions();
    m_inputbar->setCurrentCompletionModel(Inputbar::CompletionModelType::COMMAND);
    QString command = m_inputbar->getInput("Command");
    ProcessCommand(command);
}

void Navi::initMenubar() noexcept {
    m_menubar = new Menubar();
    this->setMenuBar(m_menubar);

    m_filemenu = new QMenu("File");

    m_filemenu__new_window = new QAction("New Window");
    m_filemenu__new_tab = new QAction("New Tab");
    m_filemenu__create_new_menu = new QMenu("Create New");

    m_filemenu__create_new_folder = new QAction("New Folder");
    m_filemenu__create_new_file = new QAction("New File");

    m_filemenu->addAction(m_filemenu__new_window);
    m_filemenu->addAction(m_filemenu__new_tab);
    m_filemenu->addMenu(m_filemenu__create_new_menu);

    m_filemenu__create_new_menu->addAction(m_filemenu__create_new_folder);
    m_filemenu__create_new_menu->addAction(m_filemenu__create_new_file);

    m_viewmenu = new QMenu("View");

    m_viewmenu__menubar = new QAction("Menubar");
    m_viewmenu__menubar->setCheckable(true);

    m_viewmenu__statusbar = new QAction("Statusbar");
    m_viewmenu__statusbar->setCheckable(true);

    m_viewmenu__headers = new QAction("Headers");
    m_viewmenu__headers->setCheckable(true);

    m_viewmenu__preview_panel = new QAction("Preview Panel");
    m_viewmenu__preview_panel->setCheckable(true);
    m_viewmenu__preview_panel->setChecked(true);

    m_viewmenu__messages = new QAction("Messages");
    m_viewmenu__messages->setCheckable(true);

    m_viewmenu__marks_buffer = new QAction("Marks List");
    m_viewmenu__marks_buffer->setCheckable(true);

    m_viewmenu__bookmarks_buffer = new QAction("Bookmarks");
    m_viewmenu__bookmarks_buffer->setCheckable(true);

    m_viewmenu__shortcuts_widget = new QAction("Shortcuts");
    m_viewmenu__shortcuts_widget->setCheckable(true);

    m_viewmenu__drives_widget = new QAction("Drives");
    m_viewmenu__drives_widget->setCheckable(true);

    m_viewmenu__sort_menu = new QMenu("Sort by");

    m_viewmenu__sort_by_name = new QAction("Name");
    m_viewmenu__sort_by_name->setCheckable(true);
    m_viewmenu__sort_by_name->setChecked(true);

    m_viewmenu__sort_by_size = new QAction("Size");
    m_viewmenu__sort_by_size->setCheckable(true);

    m_viewmenu__sort_by_date = new QAction("Modified Date");
    m_viewmenu__sort_by_date->setCheckable(true);

    m_viewmenu__sort_by_group = new QActionGroup(m_viewmenu__sort_menu);
    m_viewmenu__sort_by_group->addAction(m_viewmenu__sort_by_name);
    m_viewmenu__sort_by_group->addAction(m_viewmenu__sort_by_size);
    m_viewmenu__sort_by_group->addAction(m_viewmenu__sort_by_date);

    m_viewmenu__sort_ascending = new QAction("Ascending");
    m_viewmenu__sort_ascending->setCheckable(true);
    m_viewmenu__sort_ascending->setChecked(true);

    m_viewmenu__sort_descending = new QAction("Descending");
    m_viewmenu__sort_descending->setCheckable(true);

    m_viewmenu__sort_asc_desc_group = new QActionGroup(m_viewmenu__sort_menu);
    m_viewmenu__sort_asc_desc_group->addAction(m_viewmenu__sort_ascending);
    m_viewmenu__sort_asc_desc_group->addAction(m_viewmenu__sort_descending);

    m_viewmenu__sort_menu->addAction(m_viewmenu__sort_ascending);
    m_viewmenu__sort_menu->addAction(m_viewmenu__sort_descending);
    m_viewmenu__sort_menu->addSeparator();
    m_viewmenu__sort_menu->addAction(m_viewmenu__sort_by_name);
    m_viewmenu__sort_menu->addAction(m_viewmenu__sort_by_size);
    m_viewmenu__sort_menu->addAction(m_viewmenu__sort_by_date);
    m_viewmenu->addMenu(m_viewmenu__sort_menu);

    m_viewmenu__sort_ascending->setCheckable(true);
    m_viewmenu__sort_ascending->setChecked(true);

    m_viewmenu__files_menu = new QMenu("Files");

    m_viewmenu__files_menu__hidden = new QAction("Hidden");
    m_viewmenu__files_menu__hidden->setCheckable(true);

    m_viewmenu__files_menu__dotdot = new QAction("DotDot");
    m_viewmenu__files_menu__dotdot->setCheckable(true);

    m_viewmenu->addMenu(m_viewmenu__files_menu);
    m_viewmenu__files_menu->addAction(m_viewmenu__files_menu__hidden);
    m_viewmenu__files_menu->addAction(m_viewmenu__files_menu__dotdot);

    m_viewmenu->addAction(m_viewmenu__headers);
    m_viewmenu->addAction(m_viewmenu__preview_panel);
    m_viewmenu->addAction(m_viewmenu__menubar);
    m_viewmenu->addAction(m_viewmenu__statusbar);
    m_viewmenu->addAction(m_viewmenu__messages);
    m_viewmenu->addAction(m_viewmenu__marks_buffer);
    m_viewmenu->addAction(m_viewmenu__shortcuts_widget);

    m_tools_menu = new QMenu("Tools");

    m_tools_menu__search = new QAction("Search");
    m_tools_menu__command_in_folder = new QAction("Command in Folder");

    m_tools_menu->addAction(m_tools_menu__search);
    m_tools_menu->addAction(m_tools_menu__command_in_folder);

    m_edit_menu = new QMenu("Edit");

    m_edit_menu__copy = new QAction("Copy");
    m_edit_menu__paste = new QAction("Paste");
    m_edit_menu__rename = new QAction("Rename");
    m_edit_menu__delete = new QAction("Delete");
    m_edit_menu__trash = new QAction("Trash");
    m_edit_menu__cut = new QAction("Cut");

    m_edit_menu->addAction(m_edit_menu__cut);
    m_edit_menu->addAction(m_edit_menu__copy);
    m_edit_menu->addAction(m_edit_menu__paste);
    m_edit_menu->addAction(m_edit_menu__rename);
    m_edit_menu->addAction(m_edit_menu__trash);
    m_edit_menu->addAction(m_edit_menu__delete);

    m_menubar->addMenu(m_filemenu);
    m_menubar->addMenu(m_edit_menu);
    m_menubar->addMenu(m_viewmenu);
    m_menubar->addMenu(m_tools_menu);

    connect(m_viewmenu__headers, &QAction::triggered, this,
            [&](const bool &state) { getCurrentFilePanel()->ToggleHeaders(state); });

    connect(m_viewmenu__sort_ascending, &QAction::triggered, this,
            [&](const bool &state) {
                if (state) {
                    if (m_sort_flags & QDir::SortFlag::Reversed) {
                        m_sort_flags &= ~QDir::SortFlag::Reversed;
                        getCurrentFilePanel()->model()->setSortBy(m_sort_flags);
                    }
                }
            });

    connect(m_viewmenu__sort_descending, &QAction::triggered, this,
            [&](const bool &state) {
                if (state) {
                    if (m_sort_flags & QDir::SortFlag::Reversed)
                        return;
                    m_sort_flags |= QDir::SortFlag::Reversed;
                    getCurrentFilePanel()->model()->setSortBy(m_sort_flags);
                }
            });

    connect(m_viewmenu__sort_by_name, &QAction::triggered, this,
            &Navi::SortByName);
    connect(m_viewmenu__sort_by_date, &QAction::triggered, this,
            &Navi::SortByDate);
    connect(m_viewmenu__sort_by_size, &QAction::triggered, this,
            &Navi::SortBySize);

    connect(m_viewmenu__marks_buffer, &QAction::triggered, this,
            [&](const bool &state) { ToggleMarksBuffer(state); });

    connect(m_viewmenu__bookmarks_buffer, &QAction::triggered, this,
            [&](const bool &state) { // ToggleBookmarksBuffer(state); });
            });

    connect(m_viewmenu__files_menu__dotdot, &QAction::triggered, this,
            [&](const bool &state) { ToggleDotDot(state); });

    connect(m_viewmenu__preview_panel, &QAction::triggered, this,
            [&](const bool &state) { TogglePreviewPanel(state); });

    connect(m_viewmenu__messages, &QAction::triggered, this,
            [&](const bool &state) { ToggleMessagesBuffer(state); });

    connect(m_filemenu__create_new_file, &QAction::triggered, this,
            [&]() { getCurrentFilePanel()->NewFile(); });

    connect(m_filemenu__create_new_folder, &QAction::triggered, this,
            [&]() { getCurrentFilePanel()->NewFolder(); });

    connect(m_tools_menu__search, &QAction::triggered, this,
            [&]() { getCurrentFilePanel()->Search(); });

    connect(m_viewmenu__files_menu__hidden, &QAction::triggered, this,
            [&](const bool &state) { ToggleHiddenFiles(state); });

    connect(m_viewmenu__menubar, &QAction::triggered, this,
            [&](const bool &state) { Navi::ToggleMenuBar(state); });

    connect(m_viewmenu__menubar, &QAction::triggered, this,
            [&](const bool &state) { Navi::ToggleStatusBar(state); });

    // Handle visibility state change to reflect in the checkbox of the menu item

    connect(m_drives_widget, &DriveWidget::visibilityChanged, this,
            [&](const bool &state) { m_viewmenu__drives_widget->setChecked(state); });

    connect(m_menubar, &Menubar::visibilityChanged, this,
            [&](const bool &state) { m_viewmenu__menubar->setChecked(state); });

    connect(m_statusbar, &Statusbar::visibilityChanged, this,
            [&](const bool &state) { m_viewmenu__statusbar->setChecked(state); });

    connect(
            m_preview_panel, &PreviewPanel::visibilityChanged, this,
            [&](const bool &state) { m_viewmenu__preview_panel->setChecked(state); });

    connect(m_log_buffer, &MessagesBuffer::visibilityChanged, this,
            [&](const bool &state) { m_viewmenu__messages->setChecked(state); });

    connect(
            m_marks_buffer, &MarksBuffer::visibilityChanged, this,
            [&](const bool &state) { m_viewmenu__marks_buffer->setChecked(state); });
}

bool Navi::createEmptyFile(const QString &filePath) noexcept {}

void Navi::ToggleMenuBar(const bool &state) noexcept {
    if (state) {
        m_menubar->show();
    } else {
        m_menubar->hide();
    }
}

void Navi::ToggleMenuBar() noexcept {
    bool visible = m_menubar->isVisible();
    if (!visible) {
        m_menubar->show();
    } else {
        m_menubar->hide();
    }
}

void Navi::ToggleStatusBar(const bool &state) noexcept {
    if (state) {
        m_statusbar->show();
    } else {
        m_statusbar->hide();
    }
}

void Navi::ToggleStatusBar() noexcept {
    bool visible = m_statusbar->isVisible();
    if (!visible) {
        m_statusbar->show();
    } else {
        m_statusbar->hide();
    }
}

void Navi::Filter() noexcept {
    QString filterString = m_inputbar->getInput("Filter String");
    if (filterString.isEmpty() || filterString.isNull() || filterString == "*") {
        ResetFilter();
        return;
    }
    getCurrentFilePanel()->Filters(filterString);
    m_statusbar->SetFilterMode(true);
}

void Navi::ResetFilter() noexcept {
    getCurrentFilePanel()->ResetFilter();
    m_statusbar->SetFilterMode(false);
}

void Navi::LogMessage(const QString &message,
                      const MessageType &type) noexcept {
    QString coloredMessage = message;
    switch (type) {

    case MessageType::INFO:
        coloredMessage = "<font color='white'>" + message + "</font>";
        break;

    case MessageType::WARNING:
        coloredMessage = "<font color='yellow'>" + message + "</font>";
        break;

    case MessageType::ERROR:
        coloredMessage = "<font color='red'>" + message + "</font>";
        break;
    }
    m_log_buffer->AppendText(coloredMessage);
}

Navi::~Navi() {

    delete m_bookmark_manager;
    delete m_tab_widget;
    delete m_inputbar;
    delete m_statusbar;
    delete m_preview_panel;
    delete m_file_path_widget;
    delete m_log_buffer;
    delete m_marks_buffer;
    delete m_menubar;
}

void Navi::chmodHelper() noexcept {
    QString permString = m_inputbar->getInput("Permission Number");
    if (permString.length() != 3 ||
        !permString.contains(QRegularExpression("^[0-7]{3}$"))) {
        m_statusbar->Message("Invalid permission string."
                             "Expected a three-digit octal string like '755'.",
                             MessageType::ERROR, 5);
        return;
    }
}

void Navi::SaveBookmarkFile() noexcept {
    if (m_bookmark_manager->saveBookmarksFile()) {
        m_statusbar->Message("Bookmark save successful!");
    } else {
        m_statusbar->Message("Error saving bookmarks!", MessageType::ERROR);
    }
}

void Navi::SortByName(const bool &reverse) noexcept {
    m_sort_by = SortBy::Name;
    m_sort_flags = QDir::SortFlag::Name | QDir::SortFlag::DirsFirst;

    if (reverse)
        m_sort_flags |= QDir::SortFlag::Reversed;

    getCurrentFilePanel()->model()->setSortBy(m_sort_flags);
}

void Navi::SortBySize(const bool &reverse) noexcept {
    m_sort_by = SortBy::Size;
    m_sort_flags = QDir::SortFlag::Size | QDir::SortFlag::DirsFirst;

    if (reverse)
        m_sort_flags |= QDir::SortFlag::Reversed;

    getCurrentFilePanel()->model()->setSortBy(m_sort_flags);
}

void Navi::SortByDate(const bool &reverse) noexcept {
    m_sort_by = SortBy::Date;
    m_sort_flags = QDir::SortFlag::Time | QDir::SortFlag::DirsFirst;

    if (reverse)
        m_sort_flags |= QDir::SortFlag::Reversed;

    getCurrentFilePanel()->model()->setSortBy(m_sort_flags);
}

void Navi::ToggleHiddenFiles(const bool &state) noexcept {
    m_viewmenu__files_menu__hidden->setChecked(state);
    getCurrentFilePanel()->ToggleHiddenFiles();
}

void Navi::ToggleHiddenFiles() noexcept {
    if (m_viewmenu__files_menu__hidden->isChecked())
        m_viewmenu__files_menu__hidden->setChecked(false);
    else
        m_viewmenu__files_menu__hidden->setChecked(true);

    getCurrentFilePanel()->ToggleHiddenFiles();
}

void Navi::ToggleDotDot(const bool &state) noexcept {
    m_viewmenu__files_menu__dotdot->setChecked(state);
    getCurrentFilePanel()->ToggleDotDot();
}

void Navi::ToggleDotDot() noexcept {
    if (m_viewmenu__files_menu__dotdot->isChecked())
        m_viewmenu__files_menu__dotdot->setChecked(false);
    else
        m_viewmenu__files_menu__dotdot->setChecked(true);

    getCurrentFilePanel()->ToggleDotDot();
}

void Navi::readArgumentParser(argparse::ArgumentParser &parser) {

    if (parser.is_used("--config")) {
        m_config_location = QString::fromStdString(parser.get<std::string>("--config"));
    }

    if (parser.is_used("--quick")) {
        m_load_config = false;
    }

    if (parser.is_used("--bookmark-file")) {
        m_bookmark_manager->loadBookmarks(
                                          QString::fromStdString(parser.get<std::string>("--bookmark-file")));
    }

    if (parser.is_used("files")) {
        auto paths = parser.get<std::vector<std::string>>("files");

        for (const auto &path : paths) {
            m_default_location_list.append(QString::fromStdString(path));
        }
    }

}

void Navi::ToggleDrivesWidget() noexcept {
    if (m_drives_widget->isVisible())
        m_drives_widget->hide();
    else
        m_drives_widget->show();
}

void Navi::ToggleDrivesWidget(const bool &state) noexcept {
    m_drives_widget->setVisible(state);
}

void Navi::MountDrive(const QString &driveName) noexcept {
    if (driveName.isEmpty() || driveName.isNull()) {
        m_statusbar->Message("Drive name empty!", MessageType::ERROR);
        return;
    }

    QProcess process;

    // Assemble the command and arguments
    QString program = "udisksctl";
    QStringList arguments;
    arguments << "mount" << "-b" << driveName;

    // Start the process
    process.start(program, arguments);

    // Wait for the process to finish
    if (!process.waitForFinished()) {
        m_statusbar->Message(QString("Failed to mount device: %1").arg(process.errorString()), MessageType::ERROR);
        return;
    }

    // Get the output of the command
    QString output = process.readAllStandardOutput();
    QString error = process.readAllStandardError();

    if (!output.isEmpty()) {
        m_statusbar->Message(
                             QString("Mount Successful (%1)").arg(output.trimmed()));
        return;
    }

    if (!error.isEmpty()) {
        m_statusbar->Message(QString("Mount error (%1)").arg(error.trimmed()), MessageType::ERROR);
    }
}

void Navi::UnmountDrive(const QString &driveName) noexcept {
    if (driveName.isEmpty() || driveName.isNull()) {
        m_statusbar->Message("Drive name empty!", MessageType::ERROR);
        return;
    }

    QProcess process;

    // Assemble the command and arguments
    QString program = "udisksctl";
    QStringList arguments;
    arguments << "unmount" << "-b" << driveName;

    // Start the process
    process.start(program, arguments);

    // Wait for the process to finish
    if (!process.waitForFinished()) {
        m_statusbar->Message(QString("Failed to unmount device: %1").arg(process.errorString()), MessageType::ERROR);
        return;
    }

    // Get the output of the command
    QString output = process.readAllStandardOutput();
    QString error = process.readAllStandardError();

    if (!output.isEmpty()) {
        m_statusbar->Message(QString("Unmount Successful (%1)").arg(output.trimmed()));
        return;
    }

    if (!error.isEmpty()) {
        m_statusbar->Message(QString("Unmount error (%1)").arg(error.trimmed()), MessageType::ERROR);
    }
}

void Navi::Search() noexcept {
    m_inputbar->setCurrentCompletionModel(Inputbar::CompletionModelType::SEARCH);
    QString searchText = m_inputbar->getInput("Search");
    m_search_history_list.append(searchText);
    getCurrentFilePanel()->Search(searchText);
}