#include "Navi.hpp"

Navi::Navi(QWidget *parent) : QMainWindow(parent) {}

// Run the function if there is any error due to ``reason``.
void Navi::Error(const QString &reason) noexcept {
    m_statusbar->Message(reason);
    m_table_delegate->set_symlink_font(font().family());
    initKeybinds();
}

void Navi::initThings() noexcept {

    initLayout();       // init layout
    initBookmarks();
    initMenubar(); // init menubar
    initToolbar();
    setupCommandMap();
    initSignalsSlots(); // init signals and slots
    init_default_options();

    m_file_panel->tableView()->setIconSize(QSize(64, 64));

    /*if (m_load_config)*/
    /*    initConfiguration();*/
    /*else {*/
    /*    initKeybinds();*/
    /*}*/



    if (m_default_dir == ".")
        m_default_dir = QDir::currentPath();
    else if (m_default_dir.isNull() || m_default_dir.isEmpty())
        m_default_dir = QDir::homePath();

    m_file_panel->setCurrentDir(m_default_dir, true);
    m_thumbnail_cache_future_watcher->setFuture(m_thumbnail_cache_future);

    auto table = m_file_panel->tableView();
    m_table_delegate = new FilePanelDelegate(table);
    table->setItemDelegateForColumn(m_file_panel->model()->fileNameColumnIndex(), m_table_delegate);
    this->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
}

void Navi::initConfiguration() noexcept {

    /*lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::string,*/
    /*                   sol::lib::os, sol::lib::jit, sol::lib::package);*/

    // Lua's package.path modification

    /*update_lua_package_path(CONFIG_DIR_PATH);*/

    /*try {*/
    /*    lua.script_file(m_config_location.toStdString(), sol::load_mode::any);*/
    /*} catch (const sol::error &e) {*/
    /*    m_statusbar->Message(QString::fromStdString(e.what()));*/
    /*    initKeybinds();*/
    /*    return;*/
    /*}*/

    /*
    auto model = m_file_panel->model();

    // Read the SETTINGS table
    sol::optional<sol::table> settings_table_opt = lua["SETTINGS"];

    if (settings_table_opt) {
        sol::table settings_table = settings_table_opt.value();

        auto default_directory = settings_table["default_directory"].get_or<std::string>("~");

        if (m_set_default_working_dir)
            m_default_dir = QString::fromStdString(default_directory);

        // TERMINAL SETTINGS
        auto terminal = settings_table["terminal"].get_or<std::string>("kitty");
        Set_terminal(terminal);

        auto copy_path_str = settings_table["copy_path_separator"].get_or<std::string>("\n");
        Set_copy_path_separator(copy_path_str);

        // BOOKMARKS SETTINGS
        sol::optional<sol::table> bookmarks_table_opt =
            settings_table["bookmarks"];

        if (bookmarks_table_opt.has_value()) {
            auto bookmarks_table = bookmarks_table_opt.value();
            auto auto_save = bookmarks_table["auto_save"].get_or(false);
            Set_auto_save_bookmarks(auto_save);
        }

        // UI TABLE
        sol::optional<sol::table> ui_table_opt = settings_table["ui"];
        if (ui_table_opt) {
            sol::table ui_table = ui_table_opt.value();

            sol::optional<sol::table> toolbar_table_opt = ui_table["toolbar"];

            if (toolbar_table_opt) {
                auto toolbar_table = toolbar_table_opt.value();

                auto shown = toolbar_table["shown"].get_or(true);
                m_toolbar->setVisible(shown);

                auto icons_only = toolbar_table["icons_only"].get_or(false);


                if (icons_only) {
                    Set_toolbar_icons_only();
                } else {
                    Set_toolbar_text_only();
                }

                sol::optional<sol::table> layout_opt = toolbar_table["layout"];

                if (layout_opt) {
                    sol::table layout = layout_opt.value();
                    Set_toolbar_layout(layout);
                }

                initToolbar();
            }

            // Preview pane
            sol::optional<sol::table> preview_pane_table = ui_table["preview_pane"];

            if (preview_pane_table) {
                auto preview_pane = preview_pane_table.value();
                auto shown = preview_pane["shown"].get_or(true);
                TogglePreviewPanel(shown);

                auto fraction = preview_pane["fraction"].get_or(0.5);
                Set_preview_pane_fraction(fraction);

                auto max_file_size = QString::fromStdString(
                    preview_pane["max_size"].get_or<std::string>("10M"));

                auto max_file_bytes = utils::parseFileSize(max_file_size);
                m_preview_panel->SetMaxPreviewThreshold(max_file_bytes);

                // auto syntax_highlighting =
                //     preview_pane["syntax_highlight"].get_or(true);
                // m_preview_panel->SetSyntaxHighlighting(syntax_highlighting);

                sol::optional<sol::table> preview_dimension_opt =
                    preview_pane["dimension"];
                if (preview_dimension_opt) {
                    auto dimension_table = preview_dimension_opt.value();
                    auto height = dimension_table["height"].get_or(200);
                    auto width = dimension_table["width"].get_or(200);

                    m_preview_panel->SetPreviewDimension(width, height);
                }
            }

            // Menu bar
            sol::optional<sol::table> menu_bar_table = ui_table["menubar"];

            if (menu_bar_table) {
                auto menu_bar = menu_bar_table.value();
                auto shown = menu_bar["shown"].get_or(true);
                ToggleMenuBar(shown);
                auto icons = menu_bar["icons"].get_or(false);
                if (icons) {
                    Set_menubar_icons();
                }
            }

            // Path bar
            sol::optional<sol::table> path_bar_table = ui_table["pathbar"];

            if (path_bar_table) {
                auto path_bar = path_bar_table.value();
                auto shown = path_bar["shown"].get_or(true);
                auto foreground = QString::fromStdString(path_bar["foreground"].get_or<std::string>(""));
                auto background = QString::fromStdString(path_bar["background"].get_or<std::string>(""));
                auto font =
                    QString::fromStdString(path_bar["font"].get_or<std::string>(""));
                auto italic = path_bar["italic"].get_or(false);
                auto bold = path_bar["bold"].get_or(false);
                TogglePathWidget(shown);

                if (!foreground.isEmpty())
                    m_file_path_widget->setForegroundColor(foreground);

                if (!background.isEmpty())
                    m_file_path_widget->setBackgroundColor(background);

                m_file_path_widget->setBold(bold);
                m_file_path_widget->setItalic(italic);

                if (!font.isEmpty())
                    m_file_path_widget->setFont(font);
            }

            // Status bar
            sol::optional<sol::table> status_bar_table = ui_table["statusbar"];

            if (status_bar_table) {
                auto status_bar = status_bar_table.value();
                auto shown = status_bar["shown"].get_or(true);
                ToggleStatusBar(shown);

                sol::table modules =
                    status_bar["modules"].get_or<sol::table>({});

                if (modules.valid()) {
                    for (const auto &module : modules) {
                        m_statusbar->addModule(QString::fromStdString(module.second.as<std::string>()));
                    }
                }

                sol::optional<sol::table> visual_line_mode_table =
                    status_bar["visual_line_mode"];
                if (visual_line_mode_table) {
                    auto visual_line_mode = visual_line_mode_table.value();
                    auto bg = QString::fromStdString(
                        visual_line_mode["background"].get_or<std::string>(""));
                    auto fg = QString::fromStdString(
                        visual_line_mode["foreground"].get_or<std::string>(""));
                    auto bold = visual_line_mode["bold"].get_or(false);
                    auto italic = visual_line_mode["italic"].get_or(false);
                    auto padding = QString::fromStdString(
                        visual_line_mode["padding"].get_or<std::string>("2px"));
                    auto text = QString::fromStdString(
                        visual_line_mode["text"].get_or<std::string>(""));

                    if (!fg.isEmpty())
                        m_statusbar->SetVisualLineModeForeground(fg);

                    if (!bg.isEmpty())
                        m_statusbar->SetVisualLineModeBackground(bg);

                    m_statusbar->SetVisualLineModeItalic(italic);
                    m_statusbar->SetVisualLineModeBold(bold);
                    m_statusbar->SetVisualLineModePadding(padding);

                    m_statusbar->SetVisualLineModeText(text);
                }

                sol::optional<sol::table> macro_mode_table = status_bar["macro_mode"];
                if (macro_mode_table) {
                    auto macro_mode = macro_mode_table.value();
                    auto bg = QString::fromStdString(
                        macro_mode["background"].get_or<std::string>(""));
                    auto fg = QString::fromStdString(
                        macro_mode["foreground"].get_or<std::string>(""));
                    auto bold = macro_mode["bold"].get_or(false);
                    auto italic = macro_mode["italic"].get_or(false);
                    auto padding = QString::fromStdString(
                        macro_mode["padding"].get_or<std::string>("2px"));
                    auto text = QString::fromStdString(
                        macro_mode["text"].get_or<std::string>(""));

                    if (!fg.isEmpty())
                        m_statusbar->SetMacroModeForeground(fg);

                    if (!bg.isEmpty())
                        m_statusbar->SetMacroModeBackground(bg);

                    m_statusbar->SetMacroModeItalic(italic);
                    m_statusbar->SetMacroModeBold(bold);
                    m_statusbar->SetMacroModePadding(padding);

                    m_statusbar->SetMacroModeText(text);
                }
            }

            // File Pane
            sol::table file_pane_table = ui_table["file_pane"];

            if (file_pane_table.valid()) {
                sol::optional<sol::table> columns_table = file_pane_table["columns"];
                if (columns_table.has_value()) {
                    QList<FileSystemModel::Column> columnList; // List to store column configuration
                    FileSystemModel::Column column;
                    bool file_name_type_check = false;
                    sol::table columns = columns_table.value();
                    for (std::size_t i = 1; i <= columns.size(); i++) {
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
                    m_file_panel->ToggleHeaders(headers_visible.value());
                }

                // cycle
                sol::optional<bool> cycle = file_pane_table["cycle"];
                if (cycle) {
                    m_file_panel->SetCycle(cycle.value());
                }

                // symlink
                sol::optional<sol::table> symlink_table = file_pane_table["symlink"];
                if (symlink_table) {
                    auto symlink = symlink_table.value();

                    auto shown = symlink["shown"].get_or(true);
                    auto foreground = QString::fromStdString(symlink["foreground"].get_or<std::string>(""));
                    auto separator = QString::fromStdString(symlink["separator"].get_or<std::string>("->"));

                    model->setSymlinkVisible(shown);
                    model->setSymlinkSeparator(separator);
                    model->setSymlinkForeground(foreground);
                }

                // highlight
                sol::optional<sol::table> highlight_table =
                    file_pane_table["highlight"];
                if (highlight_table) {
                    auto highlight = highlight_table.value();

                    auto foreground = QString::fromStdString(highlight["foreground"].get_or<std::string>(""));
                    auto background = QString::fromStdString(highlight["background"].get_or<std::string>(""));

                    if (!(foreground.isNull() || foreground.isEmpty())) {
                        m_file_panel->setCurrentForeground(foreground);
                    }

                    if (!(background.isNull() || background.isEmpty())) {
                        m_file_panel->setCurrentBackground(background);
                    }
                }

                // marks
                sol::optional<sol::table> mark_table = file_pane_table["mark"];
                if (mark_table) {
                    auto mark = mark_table.value();
                    auto markBackground = QString::fromStdString(mark["background"].get_or<std::string>(""));
                    auto markForeground = QString::fromStdString(mark["foreground"].get_or<std::string>(""));
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
                        model->setMarkBackgroundColor(m_file_panel->tableView()
                                                      ->palette()
                                                      .brush(QWidget::backgroundRole())
                                                      .color()
                                                      .name());

                    if (!(markForeground.isNull() || markForeground.isEmpty()))
                        model->setMarkForegroundColor(markForeground);
                    else
                        model->setMarkForegroundColor(m_file_panel->tableView()
                                                      ->palette()
                                                      .brush(QWidget::backgroundRole())
                                                      .color()
                                                      .name());

                    // header
                    sol::optional<sol::table> header_table = mark["header"];

                    if (header_table) {
                        auto header = header_table.value();

                        auto markHeaderBackground = QString::fromStdString(header["background"].get_or<std::string>(""));
                        auto markHeaderForeground = QString::fromStdString(header["foreground"].get_or<std::string>(""));

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
                            model->setMarkHeaderBackgroundColor(m_file_panel->tableView()
                                                                ->palette()
                                                                .brush(QWidget::backgroundRole())
                                                                .color()
                                                                .name());

                        if (!(markHeaderForeground.isNull() ||
                            markHeaderForeground.isEmpty()))
                            model->setMarkHeaderForegroundColor(markHeaderForeground);
                        else
                            model->setMarkHeaderForegroundColor(m_file_panel->tableView()
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

                auto foregroundColor = QString::fromStdString(input_bar["foreground"].get_or<std::string>(""));
                auto backgroundColor = QString::fromStdString(input_bar["background"].get_or<std::string>(""));
                auto font =
                    QString::fromStdString(input_bar["font"].get_or<std::string>(""));

                // TODO: background unable to apply
                m_inputbar->setForeground(foregroundColor);
                m_inputbar->setBackground(backgroundColor);
                m_inputbar->setFontFamily(font);
            }
        }

        // bulk rename
        sol::optional<sol::table> bulk_rename_table_opt =
            settings_table["bulk_rename"];

        if (bulk_rename_table_opt) {
            auto bulk_rename_table = bulk_rename_table_opt.value();

            auto file_threshold = bulk_rename_table["file_threshold"].get_or(5);
            m_file_panel->SetBulkRenameThreshold(file_threshold);

            auto editor = bulk_rename_table["editor"].get_or<std::string>("nvim");
            m_file_panel->SetBulkRenameEditor(QString::fromStdString(editor));
            m_file_panel->SetTerminal(m_terminal);

            auto with_terminal = bulk_rename_table["terminal"].get_or(false);
            m_file_panel->SetBulkRenameWithTerminal(with_terminal);
        }
    }

    */

}

// Function to get all global Lua function names using Sol2
/*QStringList Navi::getLuaFunctionNames() noexcept {*/
/**/
/*    // Access the global Lua table*/
/*    sol::table function_table = lua.globals();*/
/**/
/*    if (!function_table.empty()) {*/
/*        QStringList luaFunctions;*/
/*        for (const auto &[key, value] : function_table) {*/
/*            if (value.get_type() == sol::type::function) {*/
/*                luaFunctions << QString::fromStdString(key.as<std::string>());*/
/*            }*/
/*        }*/
/*        return luaFunctions;*/
/*    } else*/
/*    return {};*/
/*}*/

// Function to create Qt keybindings from list of ‘Keybinds’ struct
void Navi::generateKeybinds() noexcept {
    for (const auto &keybind : m_keybind_list) {
        QShortcut *shortcut = new QShortcut(QKeySequence(keybind.key), this);
        connect(shortcut, &QShortcut::activated, this,
                [&]() { ProcessCommand(keybind.command); });
    }
}

void Navi::initBookmarks() noexcept {

    m_bookmark_manager = new BookmarkManager(this);

    try {
        m_bookmarks_state.script_file(BOOKMARK_FILE_PATH.toStdString(), sol::load_mode::any);
    } catch (const sol::error &e) {
        m_statusbar->Message(QString::fromStdString(e.what()));
        return;
    }

    // Read the BOOKMARKS table
    sol::optional<sol::table> bookmarks_table_opt = m_bookmarks_state["BOOKMARKS"];

    if (bookmarks_table_opt) {
        sol::table bookmarks_table = bookmarks_table_opt.value();
        QHash <QString, BookmarkManager::Bookmark> bookmarks_hash;
        for (const auto &pair : bookmarks_table) {
            auto name = QString::fromStdString(pair.first.as<std::string>());
            auto bookmark = pair.second.as<sol::table>();
            BookmarkManager::Bookmark item {
                .file_path =
                QString::fromStdString(bookmark["path"].get<std::string>()),
                .highlight_only = bookmark["highlight_only"].get_or(false),
            };
            bookmarks_hash.insert(name, item);
        }
        m_bookmark_manager->setBookmarks(bookmarks_hash);
    }

    m_bookmarks_buffer = new BookmarkWidget(m_bookmark_manager, this);
}

// Handle signals and slots
void Navi::initSignalsSlots() noexcept {

    connect(m_hook_manager, &HookManager::triggerError, this,
            [&](const QString &error) {
            m_statusbar->Message(error, MessageType::ERROR);
            });

    connect(m_thumbnail_cache_future_watcher, &QFutureWatcher<void>::finished,
            this, [&]() {
            LogMessage("Thumbnail caching finished");
            });

    connect(m_file_panel, &FilePanel::currentItemChanged, this,
            [&]() { m_hook_manager->triggerHook("item_changed"); });

    connect(m_bookmark_manager, &BookmarkManager::bookmarksChanged, this,
            [&]() { m_bookmarks_buffer->loadBookmarks(); });

    connect(m_bookmarks_buffer, &BookmarkWidget::bookmarkGoRequested, this,
            [&](const QString &name) {
            GoBookmark(name);
            });

    connect(m_drives_widget, &DriveWidget::driveLoadRequested, this,
            [&](const QString &mountPoint) {
            m_file_panel->setCurrentDir(mountPoint);
            });

    connect(m_drives_widget, &DriveWidget::driveMountRequested, this,
            [&](const QString &driveName) {
                QString confirm = m_inputbar->getInput(QString("Do you want to mount %1 ? (y, N)").arg(driveName));
                if (confirm == "n" || confirm.isNull() || confirm.isEmpty())
                    return;
                MountDrive(driveName);
            });

    connect(m_drives_widget, &DriveWidget::driveUnmountRequested, this,
            [&](const QString &driveName) {
                QString confirm = m_inputbar->getInput(QString("Do you want to unmount %1 ? (y, N)").arg(driveName));
                if (confirm == "n" || confirm.isNull() || confirm.isEmpty())
                    return;
                UnmountDrive(driveName);
            });

    connect(m_file_panel, &FilePanel::currentItemChanged, m_preview_panel,
            &PreviewPanel::onFileSelected);

    connect(m_file_panel, &FilePanel::currentItemChanged, m_statusbar,
            &Statusbar::SetFile);

    connect(m_file_panel, &FilePanel::afterDirChange, this,
            [&](const QString &path) {
            m_file_path_widget->setCurrentDir(path);
            m_hook_manager->triggerHook("directory_changed");
            m_preview_panel->clearImageCache();
            cacheThumbnails();
            });

    connect(m_file_path_widget, &FilePathWidget::directoryChangeRequested,
            m_file_panel, [&](const QString &dirName) {
            m_file_panel->setCurrentDir(dirName, true);
            });

    connect(m_file_panel, &FilePanel::fileOperationDone, this,
            [&](const bool &state, const QString &reason) {
            if (state)
            m_statusbar->Message("Operation Successful");
            else
            m_statusbar->Message(QString("Error during file operation! (%1)").arg(reason),
                                 MessageType::ERROR, 5);
            });

    connect(m_statusbar, &Statusbar::logMessage, this, &Navi::LogMessage);

    connect(m_file_panel->model(), &FileSystemModel::marksListChanged,
            m_marks_buffer, &MarksBuffer::refreshMarksList);
}

// Help for HELP interactive function
void Navi::ShowHelp() noexcept {}

// Setup the commandMap HashMap with the function calls
void Navi::setupCommandMap() noexcept {
    commandMap["execute-extended-command"] = [this](const QStringList &args) {
        UNUSED(args);
        ExecuteExtendedCommand();
    };

    commandMap["about"] = [this](const QStringList &args) {
        UNUSED(args);
        ShowAbout();
    };

    commandMap["goto-symlink-target"] = [this](const QStringList &args) {
        UNUSED(args);
        m_file_panel->goto_symlink_target();
    };

    commandMap["fullscreen"] = [this](const QStringList &args) {
        UNUSED(args);
        FullScreen();
    };

    commandMap["copy-to"] = [this](const QStringList &args) {
        UNUSED(args);
        copy_to();
    };

    commandMap["move-to"] = [this](const QStringList &args) {
        UNUSED(args);
        move_to();
    };

    commandMap["copy-path"] = [this](const QStringList &args) {
        if (args.isEmpty())
            CopyPath();
        else
            CopyPath(args.at(0));
    };

    commandMap["exit"] = [this](const QStringList &args) {
        UNUSED(args);
        Exit();
    };

    commandMap["new-window"] = [this](const QStringList &args) {
        UNUSED(args);
        LaunchNewInstance();
    };

    commandMap["folder-property"] = [this](const QStringList &args) {
        UNUSED(args);
        ShowFolderProperty();
    };

    commandMap["cd"] = [this](const QStringList &args) {
        UNUSED(args);
        ChangeDirectory();
    };

    commandMap["macro-record"] = [this](const QStringList &args) {
        UNUSED(args);
        ToggleRecordMacro();
    };

    commandMap["macro-play"] = [this](const QStringList &args) {
        UNUSED(args);
        PlayMacro();
    };

    commandMap["register"] = [this](const QStringList &args) {
        UNUSED(args);
        ToggleRegisterWidget();
    };

    commandMap["tasks"] = [this](const QStringList &args) {
        UNUSED(args);
        ToggleTasksWidget();
    };

    commandMap["lua"] = [this](const QStringList &args) {
        execute_lua_code(args.join(" ").toStdString());
    };

    commandMap["shell"] = [this](const QStringList &args) {
        UNUSED(args);
        ShellCommandAsync();
    };

    commandMap["drives"] = [this](const QStringList &args) {
        UNUSED(args);
        ToggleDrivesWidget();
    };

    commandMap["mouse-scroll"] = [this](const QStringList &args) {
        UNUSED(args);
        ToggleMouseScroll();
    };

    commandMap["visual-select"] = [this](const QStringList &args) {
        UNUSED(args);
        ToggleVisualLine();
    };

    commandMap["shortcuts-pane"] = [this](const QStringList &args) {
        UNUSED(args);
        ToggleShortcutsBuffer();
    };

    commandMap["up-directory"] = [this](const QStringList &args) {
        UNUSED(args);
        UpDirectory();
    };

    commandMap["select-item"] = [this](const QStringList &args) {
        UNUSED(args);
        SelectItem();
    };

    commandMap["next-item"] = [this](const QStringList &args) {
        UNUSED(args);
        NextItem();
    };

    commandMap["prev-item"] = [this](const QStringList &args) {
        UNUSED(args);
        PrevItem();
    };

    commandMap["first-item"] = [this](const QStringList &args) {
        UNUSED(args);
        GotoFirstItem();
    };

    commandMap["last-item"] = [this](const QStringList &args) {
        UNUSED(args);
        GotoLastItem();
    };

    commandMap["middle-item"] = [this](const QStringList &args) {
        UNUSED(args);
        GotoMiddleItem();
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

    /*
    commandMap["reload-config"] = [this](const QStringList &args) {
        initConfiguration();
    };
    */

    commandMap["sort-name"] = [this](const QStringList &args) {
        UNUSED(args);
        SortByName();
    };

    commandMap["sort-date"] = [this](const QStringList &args) {
        UNUSED(args);
        SortByDate();
    };

    commandMap["sort-size"] = [this](const QStringList &args) {
        UNUSED(args);
        SortBySize();
    };

    commandMap["sort-name-desc"] = [this](const QStringList &args) {
        UNUSED(args);
        SortByName(true);
    };

    commandMap["sort-date-desc"] = [this](const QStringList &args) {
        UNUSED(args);
        SortByDate(true);
    };

    commandMap["sort-size-desc"] = [this](const QStringList &args) {
        UNUSED(args);
        SortBySize(true);
    };

    commandMap["cycle"] = [this](const QStringList &args) {
        UNUSED(args);
        ToggleCycle();
    };

    commandMap["header"] = [this](const QStringList &args) {
        UNUSED(args);
        ToggleHeaders();
    };

    commandMap["rename"] = [this](const QStringList &args) {
        UNUSED(args);
        RenameItem();
    };

    commandMap["rename-global"] = [this](const QStringList &args) {
        UNUSED(args);
        RenameItemsGlobal();
    };

    commandMap["rename-local"] = [this](const QStringList &args) {
        UNUSED(args);
        RenameItemsLocal();
    };

    commandMap["rename-dwim"] = [this](const QStringList &args) {
        UNUSED(args);
        RenameDWIM();
    };

    commandMap["help"] = [this](const QStringList &args) {
        UNUSED(args);
        ShowHelp();
    };

    commandMap["copy"] = [this](const QStringList &args) {
        UNUSED(args);
        CopyItem();
    };

    commandMap["copy-global"] = [this](const QStringList &args) {
        UNUSED(args);
        CopyItemsGlobal();
    };

    commandMap["copy-local"] = [this](const QStringList &args) {
        UNUSED(args);
        CopyItemsLocal();
    };

    commandMap["copy-dwim"] = [this](const QStringList &args) {
        UNUSED(args);
        CopyDWIM();
    };

    commandMap["cut"] = [this](const QStringList &args) {
        UNUSED(args);
        CutItem();
    };

    commandMap["cut-global"] = [this](const QStringList &args) {
        UNUSED(args);
        CutItemsGlobal();
    };

    commandMap["cut-local"] = [this](const QStringList &args) {
        UNUSED(args);
        CutItemsLocal();
    };

    commandMap["cut-dwim"] = [this](const QStringList &args) {
        UNUSED(args);
        CutDWIM();
    };

    commandMap["paste"] = [this](const QStringList &args) {
        UNUSED(args);
        PasteItems();
    };

    commandMap["delete"] = [this](const QStringList &args) {
        UNUSED(args);
        DeleteItem();
    };

    commandMap["delete-global"] = [this](const QStringList &args) {
        UNUSED(args);
        DeleteItemsGlobal();
    };

    commandMap["delete-local"] = [this](const QStringList &args) {
        UNUSED(args);
        DeleteItemsLocal();
    };

    commandMap["delete-dwim"] = [this](const QStringList &args) {
        UNUSED(args);
        DeleteDWIM();
    };

    commandMap["mark"] = [this](const QStringList &args) {
        UNUSED(args);
        MarkItem();
    };

    commandMap["mark-regex"] = [this](const QStringList &args) {
        UNUSED(args);
        MarkRegex();
    };

    commandMap["mark-inverse"] = [this](const QStringList &args) {
        UNUSED(args);
        MarkInverse();
    };

    commandMap["mark-all"] = [this](const QStringList &args) {
        UNUSED(args);
        MarkAllItems();
    };

    commandMap["mark-dwim"] = [this](const QStringList &args) {
        UNUSED(args);
        MarkDWIM();
    };

    commandMap["toggle-mark"] = [this](const QStringList &args) {
        UNUSED(args);
        ToggleMarkItem();
    };

    commandMap["toggle-mark-dwim"] = [this](const QStringList &args) {
        UNUSED(args);
        ToggleMarkDWIM();
    };

    commandMap["unmark"] = [this](const QStringList &args) {
        UNUSED(args);
        UnmarkItem();
    };

    commandMap["unmark-local"] = [this](const QStringList &args) {
        UNUSED(args);
        UnmarkItemsLocal();
    };

    commandMap["unmark-global"] = [this](const QStringList &args) {
        UNUSED(args);
        UnmarkItemsGlobal();
    };

    commandMap["unmark-regex"] = [this](const QStringList &args) {
        UNUSED(args);
        UnmarkRegex();
    };

    commandMap["new-file"] = [this](const QStringList &args) {
        NewFile(args);
    };

    commandMap["new-folder"] = [this](const QStringList &args) {
        NewFolder(args);
    };

    commandMap["trash"] = [this](const QStringList &args) {
        UNUSED(args);
        TrashItem();
    };

    commandMap["trash-local"] = [this](const QStringList &args) {
        UNUSED(args);
        TrashItemsLocal();
    };

    commandMap["trash-global"] = [this](const QStringList &args) {
        UNUSED(args);
        TrashItemsGlobal();
    };

    commandMap["trash-dwim"] = [this](const QStringList &args) {
        UNUSED(args);
        TrashDWIM();
    };

    commandMap["messages-pane"] = [this](const QStringList &args) {
        UNUSED(args);
        ToggleMessagesBuffer();
    };

    commandMap["hidden-files"] = [this](const QStringList &args) {
        UNUSED(args);
        ToggleHiddenFiles();
    };

    commandMap["dot-dot"] = [this](const QStringList &args) {
        UNUSED(args);
        ToggleDotDot();
    };

    commandMap["preview-pane"] = [this](const QStringList &args) {
        UNUSED(args);
        TogglePreviewPanel();
    };

    commandMap["menu-bar"] = [this](const QStringList &args) {
        UNUSED(args);
        ToggleMenuBar();
    };

    commandMap["filter"] = [this](const QStringList &args) {
        UNUSED(args);
        Filter();
    };

    commandMap["reset-filter"] = [this](const QStringList &args) {
        UNUSED(args);
        ResetFilter();
    };

    commandMap["refresh"] = [this](const QStringList &args) {
        UNUSED(args);
        ForceUpdate();
    };

    commandMap["chmod"] = [this](const QStringList &args) {
        UNUSED(args);
        ChmodItem();
    };

    commandMap["chmod-local"] = [this](const QStringList &args) {
        UNUSED(args);
        ChmodItemsLocal();
    };

    commandMap["chmod-global"] = [this](const QStringList &args) {
        UNUSED(args);
        ChmodItemsGlobal();
    };

    commandMap["chmod-dwim"] = [this](const QStringList &args) {
        UNUSED(args);
        ChmodDWIM();
    };

    commandMap["marks-pane"] = [this](const QStringList &args) {
        UNUSED(args);
        ToggleMarksBuffer();
    };

    commandMap["focus-path"] = [&](const QStringList &args) {
        UNUSED(args);
        m_file_path_widget->FocusLineEdit();
    };

    commandMap["item-property"] = [&](const QStringList &args) {
        UNUSED(args);
        ShowItemPropertyWidget();
    };

    commandMap["bookmarks-pane"] = [&](const QStringList &args) {
        UNUSED(args);
        ToggleBookmarksBuffer();
    };

    commandMap["bookmark-go"] = [&](const QStringList &args) {
        if (args.isEmpty())
            GoBookmark();
        else
            GoBookmark(args.at(0));
    };

    commandMap["bookmark-add"] = [&](const QStringList &args) {
        AddBookmark(args);
    };

    commandMap["bookmark-remove"] = [&](const QStringList &args) {
        RemoveBookmark(args);
    };

    commandMap["bookmark-edit-name"] = [&](const QStringList &args) {
        EditBookmarkName(args);
    };

    commandMap["bookmark-edit-path"] = [&](const QStringList &args) {
        EditBookmarkFile(args);
    };

    commandMap["bookmarks-save"] = [&](const QStringList &args) {
        UNUSED(args);
        SaveBookmarkFile();
    };

    commandMap["search"] = [&](const QStringList &args) {
        UNUSED(args);
        Search(QString());
    };

    commandMap["search-regex"] = [&](const QStringList &args) {
        UNUSED(args);
        SearchRegex();
    };

    commandMap["search-next"] = [&](const QStringList &args) {
        UNUSED(args);
        SearchNext();
    };

    commandMap["search-prev"] = [&](const QStringList &args) {
        UNUSED(args);
        SearchPrev();
    };

    m_inputbar->addCompletionStringList(Inputbar::CompletionModelType::COMMAND,
                                        m_valid_command_list);
}

void Navi::EditBookmarkName(const QStringList &args) noexcept {

    QString bookmarkName;
    if (args.isEmpty()) {
        bookmarkName = m_inputbar->getInput("(Add bookmark) Bookmark name");
    } else
    bookmarkName = args.at(0);

    if (bookmarkName.isEmpty()) {
        m_statusbar->Message("Bookmark name cannot be empty");
        return;
    }

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
                             MessageType::ERROR);
        return;
    }
}

void Navi::EditBookmarkFile(const QStringList &args) noexcept {

    QString bookmarkName;
    if (args.isEmpty()) {
        bookmarkName = m_inputbar->getInput("(Add bookmark) Bookmark name");
    } else
    bookmarkName = args.at(0);

    if (bookmarkName.isEmpty()) {
        m_statusbar->Message("Bookmark name cannot be empty");
        return;
    }

    QString newBookmarkPath =
        m_inputbar->getInput(QString("New bookmark path (Default: %1)")
                             .arg(m_file_panel->getCurrentDir()),
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
    if (m_bookmark_manager->setBookmarkFile(bookmarkName, newBookmarkPath, false)) {
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


void Navi::AddBookmark(const QStringList &args) noexcept {
    QString bookmarkName;
    if (args.isEmpty()) {
        bookmarkName = m_inputbar->getInput("(Add bookmark) Bookmark name");
    } else
    bookmarkName = args.at(0);

    if (bookmarkName.isEmpty()) {
        m_statusbar->Message("Bookmark name cannot be empty");
        return;
    }

    bool highlight = false;
    QString highlight_string =
        m_inputbar->getInput("(Add bookmark) Highlight ? (y/n)").toLower();

    if (highlight_string == "y" || highlight_string == "yes") {
        highlight = true;
    }

    if (m_bookmark_manager->addBookmark(bookmarkName,
                                        m_file_panel->getCurrentDir(), highlight)) {
        m_statusbar->Message("Added bookmark");
    } else
    m_statusbar->Message("Error adding bookmark!", MessageType::ERROR, 5);
}

void Navi::RemoveBookmark(const QStringList &args) noexcept {
    QString bookmarkName;
    if (args.isEmpty()) {
        bookmarkName = m_inputbar->getInput("(Remove bookmark) Bookmark name");
    } else
    bookmarkName = args.at(0);

    if (bookmarkName.isEmpty()) {
        m_statusbar->Message("Bookmark name cannot be empty");
        return;
    }

    if (m_bookmark_manager->removeBookmark(bookmarkName)) {
        m_statusbar->Message(QString("Bookmark %1 removed!").arg(bookmarkName));
    }
    else
    m_statusbar->Message(QString("Error removing bookmark %1").arg(bookmarkName),
                         MessageType::ERROR, 5);
}

void Navi::GoBookmark(const QString &bookmarkName) noexcept {

    BookmarkManager::Bookmark bookmark =
        m_bookmark_manager->getBookmark(bookmarkName);

    auto bookmarkPath = bookmark.file_path;

    if (bookmarkPath.isNull() || bookmarkPath.isEmpty()) {
        m_statusbar->Message(QString("Bookmark %1 not found!").arg(bookmarkName),
                             MessageType::ERROR, 5);
    } else {
        if (bookmark.highlight_only) {
            // Get the parent directory path
            int lastSlashIndex = bookmarkPath.lastIndexOf('/'); // Find the last slash
            QString parentDirPath = (lastSlashIndex != -1)
                ? bookmarkPath.left(lastSlashIndex)
                : QString();

            QFileInfo f(bookmarkPath);
            m_file_panel->setCurrentDir(parentDirPath);
            m_file_panel->HighlightItemWithBaseName(f.fileName());
        } else
        m_file_panel->setCurrentDir(bookmarkPath, true);
    }
}

void Navi::ToggleBookmarksBuffer() noexcept {
    if (m_bookmarks_buffer->isVisible()) {
        m_bookmarks_buffer->hide();
    } else {
        m_bookmarks_buffer->show();
    }
}

void Navi::ToggleBookmarksBuffer(const bool &state) noexcept {
    if (state) {
        m_bookmarks_buffer->show();
    } else {
        m_bookmarks_buffer->hide();
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
        if (m_macro_mode)
            addCommandToMacroRegister(QString::number(num));
        GotoItem(num);
        return;
    }

    //        COMMAND1       &&        COMMAND2
    // SUBCOMMAND1 ARG1 ARG2 && SUBCOMMAND2 ARG1 ARG2

    for (const auto &commands : commandlist) {
        QStringList command = utils::splitPreservingQuotes(commands);
        QString subcommand = command.takeFirst();
        QStringList args = command;

        if (commandMap.contains(subcommand)) {
            if (m_macro_mode && subcommand != "macro-record") {
                if (!args.isEmpty())
                    addCommandToMacroRegister(subcommand + " " + args.join(" "));
                else
                    addCommandToMacroRegister(subcommand);
            }

            commandMap[subcommand](args); // Call the associated function

        } else {
            m_statusbar->Message(
                QString("Command %1 is not a valid interactive command")
                .arg(subcommand),
                MessageType::ERROR);
        }
    }
}

void Navi::TogglePreviewPanel(const bool &state) noexcept {
    if (state) {
        m_preview_panel->show();
        connect(m_file_panel, &FilePanel::currentItemChanged, m_preview_panel,
                &PreviewPanel::onFileSelected);
        m_preview_panel->onFileSelected(getCurrentFile());
    } else {
        m_preview_panel->hide();
        disconnect(m_file_panel, &FilePanel::currentItemChanged, m_preview_panel,
                   &PreviewPanel::onFileSelected);
    }
}

void Navi::TogglePreviewPanel() noexcept {
    bool visible = m_preview_panel->isVisible();
    if (visible) {
        m_preview_panel->hide();
        disconnect(m_file_panel, &FilePanel::currentItemChanged, m_preview_panel,
                   &PreviewPanel::onFileSelected);
    } else {
        m_preview_panel->show();
        connect(m_file_panel, &FilePanel::currentItemChanged, m_preview_panel,
                &PreviewPanel::onFileSelected);
        m_preview_panel->onFileSelected(getCurrentFile());
    }
}

QString Navi::getCurrentFile() noexcept {
    return m_file_panel->getCurrentItem();
}

void Navi::initLayout() noexcept {

    m_toolbar = new QToolBar(this);
    addToolBar(m_toolbar);
    m_tasks_widget = new TasksWidget(m_task_manager, this);
    m_hook_manager = new HookManager();
    m_inputbar = new Inputbar(this);
    m_statusbar = new Statusbar(this);
    m_file_panel = new FilePanel(m_inputbar, m_statusbar, m_hook_manager, m_task_manager, this);

    m_preview_panel = new PreviewPanel(this);
    m_file_path_widget = new FilePathWidget(this);
    m_log_buffer = new MessagesBuffer(this);
    m_marks_buffer = new MarksBuffer(this);

    m_marks_buffer->setMarksSet(m_file_panel->getMarksSetPTR());

    m_file_panel->setFocus();
    m_inputbar->hide();

    m_file_path_widget->setContentsMargins(0, 0, 0, 0);
    this->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_file_path_widget);
    m_layout->addWidget(m_splitter);
    m_layout->addWidget(m_inputbar);
    m_layout->addWidget(m_statusbar);
    m_splitter->addWidget(m_file_panel);
    m_splitter->addWidget(m_preview_panel);
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
            [&]() { ToggleVisualLine(); });

    connect(kb_mark_item, &QShortcut::activated, this,
            [&]() { ToggleMarkDWIM(); });

    connect(kb_mark_inverse, &QShortcut::activated, this,
            [&]() { MarkInverse(); });

    connect(kb_mark_all, &QShortcut::activated, this, [&]() { MarkAllItems(); });

    connect(kb_next_item, &QShortcut::activated, this, [&]() { NextItem(); });

    connect(kb_prev_item, &QShortcut::activated, this, [&]() { PrevItem(); });

    connect(kb_select_item, &QShortcut::activated, this, [&]() { SelectItem(); });

    connect(kb_up_directory, &QShortcut::activated, this,
            [&]() { UpDirectory(); });

    connect(kb_goto_last_item, &QShortcut::activated, this,
            [&]() { GotoLastItem(); });

    connect(kb_goto_first_item, &QShortcut::activated, this,
            [&]() { GotoFirstItem(); });

    connect(kb_command, &QShortcut::activated, this,
            &Navi::ExecuteExtendedCommand);

    connect(kb_rename_items, &QShortcut::activated, this,
            [&]() { RenameDWIM(); });

    connect(kb_delete_items, &QShortcut::activated, this,
            [&]() { DeleteDWIM(); });

    connect(kb_search, &QShortcut::activated, this, [&]() { Search(QString()); });

    connect(kb_search_next, &QShortcut::activated, this, [&]() { SearchNext(); });

    connect(kb_search_prev, &QShortcut::activated, this, [&]() { SearchPrev(); });

    connect(kb_toggle_menubar, &QShortcut::activated, this,
            [this]() { ToggleMenuBar(); });

    connect(kb_toggle_preview_panel, &QShortcut::activated, this,
            [this]() { TogglePreviewPanel(); });

    connect(kb_focus_file_path_widget, &QShortcut::activated, this,
            [this]() { m_file_path_widget->FocusLineEdit(); });

    connect(kb_paste_items, &QShortcut::activated, this, [&]() { PasteItems(); });

    connect(kb_copy_items, &QShortcut::activated, this, [&]() { CopyItem(); });

    connect(kb_unmark_items_local, &QShortcut::activated, this,
            [&]() { m_file_panel->UnmarkItemsLocal(); });

    connect(kb_toggle_hidden_files, &QShortcut::activated, this,
            [&]() { ToggleHiddenFiles(); });
}

void Navi::ExecuteExtendedCommand() noexcept {
    m_inputbar->enableCommandCompletions();
    m_inputbar->currentCompletionStringList(
        Inputbar::CompletionModelType::COMMAND);
    QString command = m_inputbar->getInput("Command");
    ProcessCommand(command);
}

void Navi::initMenubar() noexcept {
    m_menubar = new Menubar(this);
    this->setMenuBar(m_menubar);

    m_filemenu = new QMenu("&File");

    m_filemenu__new_window = new QAction("New Window");


    m_filemenu__create_new_menu = new QMenu("Create New");

    m_filemenu__create_new_folder = new QAction("New Folder");
    m_filemenu__create_new_file = new QAction("New File");

    m_filemenu__close_window = new QAction("Close Window");
    m_filemenu__folder_properties = new QAction("Folder Properties");


    m_filemenu->addAction(m_filemenu__new_window);
    m_filemenu->addMenu(m_filemenu__create_new_menu);

    m_filemenu__create_new_menu->addAction(m_filemenu__create_new_folder);
    m_filemenu__create_new_menu->addAction(m_filemenu__create_new_file);

    m_filemenu->addAction(m_filemenu__folder_properties);
    m_filemenu->addAction(m_filemenu__close_window);

    m_viewmenu = new QMenu("&View");

    m_viewmenu__refresh = new QAction("Refresh Folder");
    m_viewmenu__filter = new QAction("Filter");
    m_viewmenu__filter->setCheckable(true);

    m_viewmenu__fullscreen = new QAction("Fullscreen");
    m_viewmenu__fullscreen->setCheckable(true);

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

    m_viewmenu__tasks_widget = new QAction("Tasks");
    m_viewmenu__tasks_widget->setCheckable(true);

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

    m_viewmenu__files_menu = new QMenu("&Files");

    m_viewmenu__files_menu__hidden = new QAction("Hidden");
    m_viewmenu__files_menu__hidden->setCheckable(true);

    m_viewmenu__files_menu__dotdot = new QAction("DotDot");
    m_viewmenu__files_menu__dotdot->setCheckable(true);

    m_viewmenu->addMenu(m_viewmenu__files_menu);
    m_viewmenu__files_menu->addAction(m_viewmenu__files_menu__hidden);
    m_viewmenu__files_menu->addAction(m_viewmenu__files_menu__dotdot);

    m_viewmenu->addAction(m_viewmenu__refresh);
    m_viewmenu->addAction(m_viewmenu__filter);
    m_viewmenu->addAction(m_viewmenu__fullscreen);
    m_viewmenu->addSeparator();
    m_viewmenu->addAction(m_viewmenu__headers);
    m_viewmenu->addAction(m_viewmenu__preview_panel);
    m_viewmenu->addAction(m_viewmenu__menubar);
    m_viewmenu->addAction(m_viewmenu__statusbar);
    m_viewmenu->addAction(m_viewmenu__messages);
    m_viewmenu->addAction(m_viewmenu__marks_buffer);
    m_viewmenu->addAction(m_viewmenu__shortcuts_widget);
    m_viewmenu->addAction(m_viewmenu__tasks_widget);

    m_bookmarks_menu = new QMenu("&Bookmarks");

    m_bookmarks_menu__add = new QAction("Add bookmark");
    m_bookmarks_menu__remove = new QAction("Remove bookmark");


    m_bookmarks_menu__bookmarks_list_menu = new QMenu("Bookmarks");
    m_bookmarks_menu->addMenu(m_bookmarks_menu__bookmarks_list_menu);
    m_bookmarks_menu->addAction(m_bookmarks_menu__add);
    m_bookmarks_menu->addAction(m_bookmarks_menu__remove);



    auto bookmarks = m_bookmark_manager->getBookmarkNames();
    if (!bookmarks.empty()) {
        for (const auto &bookmark : bookmarks) {
            QAction *action = new QAction(bookmark);
            m_bookmarks_menu__bookmarks_list_menu->addAction(action);
        }

        connect(m_bookmarks_menu__bookmarks_list_menu, &QMenu::triggered, this,
                [&](QAction *action) { GoBookmark(action->text()); });
    }

    connect(m_bookmarks_menu__add, &QAction::triggered, this,
            [&]() { AddBookmark(); });

    connect(m_bookmarks_menu__remove, &QAction::triggered, this, [&]() {
        RemoveBookmark();
    });

    m_tools_menu = new QMenu("&Tools");

    m_tools_menu__search = new QAction("Search");
    m_tools_menu__find_files = new QAction("Find Files");

    m_tools_menu->addAction(m_tools_menu__find_files);
    m_tools_menu->addAction(m_tools_menu__search);



    m_edit_menu = new QMenu("&Edit");

    m_edit_menu__open = new QAction("Open");
    m_edit_menu__copy = new QAction("Copy");
    m_edit_menu__paste = new QAction("Paste");
    m_edit_menu__rename = new QAction("Rename");
    m_edit_menu__delete = new QAction("Delete");
    m_edit_menu__trash = new QAction("Trash");
    m_edit_menu__copy_path = new QAction("Copy Path(s)");
    m_edit_menu__copy_to = new QAction("Copy to");
    m_edit_menu__move_to = new QAction("Move to");
    m_edit_menu__link_to = new QAction("Create Link...");
    m_edit_menu__item_property = new QAction("Properties");
    m_edit_menu__cut = new QAction("Cut");
    m_edit_menu__select_all = new QAction("Select All");
    m_edit_menu__select_inverse = new QAction("Select Inverse");
    // m_viewmenu__filter->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::View));

    m_edit_menu->addAction(m_edit_menu__open);
    m_edit_menu->addAction(m_edit_menu__copy_path);
    m_edit_menu->addAction(m_edit_menu__item_property);
    m_edit_menu->addAction(m_edit_menu__cut);
    m_edit_menu->addAction(m_edit_menu__copy);
    m_edit_menu->addAction(m_edit_menu__paste);
    m_edit_menu->addAction(m_edit_menu__copy_to);
    m_edit_menu->addAction(m_edit_menu__move_to);
    m_edit_menu->addAction(m_edit_menu__link_to);
    m_edit_menu->addAction(m_edit_menu__rename);
    m_edit_menu->addAction(m_edit_menu__trash);
    m_edit_menu->addAction(m_edit_menu__delete);
    m_edit_menu->addAction(m_edit_menu__select_all);
    m_edit_menu->addAction(m_edit_menu__select_inverse);

    connect(m_edit_menu__open, &QAction::triggered, this, &Navi::SelectItem);
    connect(m_edit_menu__copy_path, &QAction::triggered, this, [&]() {
        CopyPath();
    });
    connect(m_edit_menu__copy, &QAction::triggered, this, &Navi::CopyDWIM);
    connect(m_edit_menu__cut, &QAction::triggered, this, &Navi::CutDWIM);
    connect(m_edit_menu__paste, &QAction::triggered, this, &Navi::PasteItems);
    connect(m_edit_menu__rename, &QAction::triggered, this, &Navi::RenameDWIM);
    connect(m_edit_menu__trash, &QAction::triggered, this, &Navi::TrashDWIM);
    connect(m_edit_menu__delete, &QAction::triggered, this, &Navi::DeleteDWIM);
    connect(m_edit_menu__item_property, &QAction::triggered, this,
            &Navi::ShowItemPropertyWidget);
    connect(m_edit_menu__select_all, &QAction::triggered, this, &Navi::SelectAllItems);
    connect(m_edit_menu__select_inverse, &QAction::triggered, this,
            &Navi::SelectInverse);
    connect(m_edit_menu__copy_to, &QAction::triggered, this, &Navi::copy_to);
    connect(m_edit_menu__move_to, &QAction::triggered, this, &Navi::move_to);

    m_go_menu = new QMenu("&Go");

    m_go_menu__home_folder = new QAction("Home Folder");
    m_go_menu__parent_folder = new QAction("Parent Folder");
    m_go_menu__previous_folder = new QAction("Previous Folder");
    m_go_menu__next_folder = new QAction("Next Folder");
    m_go_menu__connect_to_server = new QAction("Connect to Server");


    m_go_menu->addActions({
        m_go_menu__previous_folder,
        m_go_menu__next_folder,
        m_go_menu__parent_folder,
        m_go_menu__home_folder,
        m_go_menu__connect_to_server});

    connect(m_go_menu__previous_folder, &QAction::triggered, m_file_panel, &FilePanel::PreviousDirectory);
    connect(m_go_menu__home_folder, &QAction::triggered, m_file_panel, &FilePanel::HomeDirectory);
    connect(m_go_menu__parent_folder, &QAction::triggered, m_file_panel,
            &FilePanel::UpDirectory);

    m_help_menu = new QMenu("&Help");

    m_help_menu__about = new QAction("About");
    m_help_menu->addAction(m_help_menu__about);

    connect(m_help_menu__about, &QAction::triggered, this, [&]() {
        ShowAbout();
    });

    m_menubar->addMenu(m_filemenu);
    m_menubar->addMenu(m_edit_menu);
    m_menubar->addMenu(m_viewmenu);
    m_menubar->addMenu(m_bookmarks_menu);
    m_menubar->addMenu(m_go_menu);
    m_menubar->addMenu(m_tools_menu);
    m_menubar->addMenu(m_help_menu);

    connect(m_viewmenu__refresh, &QAction::triggered, this,
            [&](const bool &state) { ForceUpdate(); });

    connect(m_viewmenu__fullscreen, &QAction::triggered, this,
            [&](const bool &state) { FullScreen(state); });

    connect(m_viewmenu__filter, &QAction::triggered, this,
            [&](const bool &state) {
            if (state)
            Filter();
            else
            ResetFilter();
            });

    connect(m_viewmenu__tasks_widget, &QAction::triggered, this,
            [&](const bool &state) { ToggleTasksWidget(state); });

    connect(m_viewmenu__headers, &QAction::triggered, this,
            [&](const bool &state) { m_file_panel->ToggleHeaders(state); });

    connect(m_viewmenu__sort_ascending, &QAction::triggered, this, &Navi::SortAscending);

    connect(m_viewmenu__sort_descending, &QAction::triggered, this, &Navi::SortDescending);

    connect(m_viewmenu__sort_by_name, &QAction::triggered, this,
            &Navi::SortByName);

    connect(m_viewmenu__sort_by_date, &QAction::triggered, this,
            &Navi::SortByDate);
    connect(m_viewmenu__sort_by_size, &QAction::triggered, this,
            &Navi::SortBySize);

    connect(m_viewmenu__marks_buffer, &QAction::triggered, this,
            [&](const bool &state) { ToggleMarksBuffer(state); });

    connect(m_viewmenu__bookmarks_buffer, &QAction::triggered, this,
            [&](const bool &state) { ToggleBookmarksBuffer(state); });

    connect(m_viewmenu__files_menu__dotdot, &QAction::triggered, this,
            [&](const bool &state) { ToggleDotDot(state); });

    connect(m_viewmenu__preview_panel, &QAction::triggered, this,
            [&](const bool &state) { TogglePreviewPanel(state); });

    connect(m_viewmenu__messages, &QAction::triggered, this,
            [&](const bool &state) { ToggleMessagesBuffer(state); });

    connect(m_filemenu__new_window, &QAction::triggered, this, &Navi::LaunchNewInstance);

    connect(m_filemenu__create_new_file, &QAction::triggered, this,
            [&]() { m_file_panel->NewFile(); });

    connect(m_filemenu__create_new_folder, &QAction::triggered, this,
            [&]() { m_file_panel->NewFolder(); });

    connect(m_filemenu__close_window, &QAction::triggered, this, &Navi::Exit);

    connect(m_filemenu__folder_properties, &QAction::triggered, this, &Navi::ShowFolderProperty);

    connect(m_tools_menu__search, &QAction::triggered, this,
            [&]() { m_file_panel->Search(); });

    connect(m_viewmenu__files_menu__hidden, &QAction::triggered, this,
            [&](const bool &state) { ToggleHiddenFiles(state); });

    connect(m_viewmenu__menubar, &QAction::triggered, this,
            [&](const bool &state) { Navi::ToggleMenuBar(state); });

    connect(m_viewmenu__menubar, &QAction::triggered, this,
            [&](const bool &state) { Navi::ToggleStatusBar(state); });

    // Handle visibility state change to reflect in the checkbox of the menu
    // item

    connect(m_bookmarks_buffer, &BookmarkWidget::visibilityChanged, this,
            [&](const bool &state) {
            m_viewmenu__bookmarks_buffer->setChecked(state);
            });

    connect(m_tasks_widget, &TasksWidget::visibilityChanged, this,
            [&](const bool &state) {
            m_viewmenu__tasks_widget->setChecked(state);
            });

    connect(m_drives_widget, &DriveWidget::visibilityChanged, this,
            [&](const bool &state) { m_viewmenu__drives_widget->setChecked(state); });

    connect(m_menubar, &Menubar::visibilityChanged, this,
            [&](const bool &state) { m_viewmenu__menubar->setChecked(state); });

    connect(m_statusbar, &Statusbar::visibilityChanged, this,
            [&](const bool &state) { m_viewmenu__statusbar->setChecked(state); });

    connect(m_preview_panel, &PreviewPanel::visibilityChanged, this,
            [&](const bool &state) { m_viewmenu__preview_panel->setChecked(state); });

    connect(m_log_buffer, &MessagesBuffer::visibilityChanged, this,
            [&](const bool &state) { m_viewmenu__messages->setChecked(state); });

    connect(m_marks_buffer, &MarksBuffer::visibilityChanged, this,
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

void Navi::Filter(const QString &filter_string) noexcept {
    if (filter_string.isEmpty() || filter_string.isNull()) {
        QString filterString = m_inputbar->getInput("Filter String");
        if (filterString.isEmpty() || filterString.isNull() || filterString == "*") {
            ResetFilter();
        } else {
            m_file_panel->Filters(filterString);
            m_statusbar->SetFilterMode(true);
            m_viewmenu__filter->setChecked(true);
        }
    } else {
        m_file_panel->Filters(filter_string);
        m_statusbar->SetFilterMode(true);
        m_viewmenu__filter->setChecked(true);
    }
}

void Navi::filter(const std::string &_filter_string) noexcept {
    Filter(QString::fromStdString(_filter_string));
}

void Navi::ResetFilter() noexcept {
    m_file_panel->ResetFilter();
    m_statusbar->SetFilterMode(false);
    m_viewmenu__filter->setChecked(false);
}

// Similar to Statusbar Message function, but this logs message only without
// printing to the statusbar. The messages can be seen in the messages pane
// similar to the `m_statusbar->Message()` function.
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
    if (m_auto_save_bookmarks) {
        SaveBookmarkFile();
    }
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
    // FIX: Fix this no pointer issue somehow
    if (!m_bookmark_manager) return;

    if (!m_bookmark_manager->hasUnsavedBookmarks())
        return;
    auto bookmarks_hash = m_bookmark_manager->getBookmarks();

    QFile bookmark_file(BOOKMARK_FILE_PATH);

    if (!bookmark_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_statusbar->Message("Unable to save bookmarks!", MessageType::ERROR);
        return;
    }

    QTextStream out(&bookmark_file);

    QString bookmarks_str = "BOOKMARKS = {\n";

    for (const auto &key : bookmarks_hash.keys()) {
        BookmarkManager::Bookmark bookmark = bookmarks_hash[key];
        if (bookmark.highlight_only) {
            bookmarks_str += QString("\t%1 = { path = \"%2\",\n\thighlight_only = true,\n\t},\n")
                .arg(key)
                .arg(bookmark.file_path);
        } else {
            bookmarks_str += QString("\t%1 = { path = \"%2\" },\n")
                .arg(key)
                .arg(bookmark.file_path);
        }
    }

    bookmarks_str += "}";

    out << bookmarks_str;

    bookmark_file.close();

    m_bookmark_manager->savedChanges(true);
}

void Navi::SortByName(const bool &reverse) noexcept {
    m_sort_by = SortBy::Name;
    m_sort_flags = QDir::SortFlag::Name | QDir::SortFlag::DirsFirst;

    if (reverse)
        m_sort_flags |= QDir::SortFlag::Reversed;

    m_file_panel->model()->setSortBy(m_sort_flags);
}

void Navi::SortBySize(const bool &reverse) noexcept {
    m_sort_by = SortBy::Size;
    m_sort_flags = QDir::SortFlag::Size | QDir::SortFlag::DirsFirst;

    if (reverse)
        m_sort_flags |= QDir::SortFlag::Reversed;

    m_file_panel->model()->setSortBy(m_sort_flags);
}

void Navi::SortByDate(const bool &reverse) noexcept {
    m_sort_by = SortBy::Date;
    m_sort_flags = QDir::SortFlag::Time | QDir::SortFlag::DirsFirst;

    if (reverse)
        m_sort_flags |= QDir::SortFlag::Reversed;

    m_file_panel->model()->setSortBy(m_sort_flags);
}

void Navi::ToggleHiddenFiles(const bool &state) noexcept {
    m_viewmenu__files_menu__hidden->setChecked(state);
    m_file_panel->ToggleHiddenFiles();
}

void Navi::ToggleHiddenFiles() noexcept {
    if (m_viewmenu__files_menu__hidden->isChecked())
        m_viewmenu__files_menu__hidden->setChecked(false);
    else
        m_viewmenu__files_menu__hidden->setChecked(true);

    m_file_panel->ToggleHiddenFiles();
}

void Navi::ToggleDotDot(const bool &state) noexcept {
    m_viewmenu__files_menu__dotdot->setChecked(state);
    m_file_panel->ToggleDotDot();
}

void Navi::ToggleDotDot() noexcept {
    if (m_viewmenu__files_menu__dotdot->isChecked())
        m_viewmenu__files_menu__dotdot->setChecked(false);
    else
        m_viewmenu__files_menu__dotdot->setChecked(true);

    m_file_panel->ToggleDotDot();
}

void Navi::readArgumentParser(argparse::ArgumentParser &parser) {

    if (parser.is_used("--config")) {
        m_config_location =
            QString::fromStdString(parser.get<std::string>("--config"));
    }

    if (parser.is_used("--quick")) {
        m_load_config = false;
    }

    try {
        Set_default_directory(QString::fromStdString(parser.get<std::vector<std::string>>("files").at(0)));
    } catch (std::logic_error &e) {
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

void Navi::ToggleRecordMacro() noexcept {
    m_macro_mode = !m_macro_mode;

    if (m_macro_mode) {
        m_macro_register.clear();
        QString macro_key = m_inputbar->getInput("Enter the macro key");

        if (m_macro_hash.contains(macro_key)) {
            auto reply =
                QMessageBox::question(this, "Macro exists",
                                      QString("A macro with the key %1 already "
                                              "exists. Do you want to overwrite ?")
                                      .arg(macro_key));
            if (reply != QMessageBox::Yes) {
                return;
            }
        }

        m_macro_register.append(macro_key);

        m_statusbar->SetMacroMode(true);
        m_statusbar->Message(QString("Recording macro for key %1").arg(macro_key));
    } else {
        m_macro_hash.insert(m_macro_register.takeFirst(), m_macro_register);
        m_statusbar->SetMacroMode(false);
        m_statusbar->Message("Macro recording done");
    }
}

void Navi::DeleteMacro() noexcept {
    QString macro_key = m_inputbar->getInput("Enter the macro key to delete");
    if (m_macro_hash.contains(macro_key))
        m_macro_hash.remove(macro_key);
}
void Navi::EditMacro() noexcept {}

void Navi::ListMacro() noexcept {
    for (const auto &macro : m_macro_hash.values()) {
    }
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
        m_statusbar->Message(
            QString("Failed to mount device: %1").arg(process.errorString()),
            MessageType::ERROR);
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
        m_statusbar->Message(QString("Mount error (%1)").arg(error.trimmed()),
                             MessageType::ERROR);
    }

    m_hook_manager->triggerHook("drive_mounted");
}

// Lua api for mounting drive
bool Navi::mount_drive(const std::string &drive_name) noexcept {

    if (drive_name.empty())
        return false;

    QProcess process;

    // Assemble the command and arguments
    QString program = "udisksctl";
    QStringList arguments;
    arguments << "mount" << "-b" << QString::fromStdString(drive_name);

    // Start the process
    process.start(program, arguments);

    // Wait for the process to finish
    if (!process.waitForFinished()) {
        return false;
    }

    // Get the output of the command
    QString output = process.readAllStandardOutput();
    QString error = process.readAllStandardError();

    if (!output.isEmpty()) {
        m_hook_manager->triggerHook("drive_mounted");
        return true;
    }

    if (!error.isEmpty()) {
        return false;
    }

    return false;
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
        m_statusbar->Message(
            QString("Failed to unmount device: %1").arg(process.errorString()),
            MessageType::ERROR);
        return;
    }

    // Get the output of the command
    QString output = process.readAllStandardOutput();
    QString error = process.readAllStandardError();

    if (!output.isEmpty()) {
        m_statusbar->Message(
            QString("Unmount Successful (%1)").arg(output.trimmed()));
        m_hook_manager->triggerHook("drive_unmounted");
        return;
    }

    if (!error.isEmpty()) {
        m_statusbar->Message(QString("Unmount error (%1)").arg(error.trimmed()),
                             MessageType::ERROR);
    }
}


bool Navi::unmount_drive(const std::string &drive_name) noexcept {
    if (drive_name.empty())
        return false;

    QProcess process;

    // Assemble the command and arguments
    QString program = "udisksctl";
    QStringList arguments;
    arguments << "unmount" << "-b" << QString::fromStdString(drive_name);

    // Start the process
    process.start(program, arguments);

    // Wait for the process to finish
    if (!process.waitForFinished()) {
        return false;
    }

    // Get the output of the command
    QString output = process.readAllStandardOutput();
    QString error = process.readAllStandardError();

    if (!output.isEmpty()) {
        m_hook_manager->triggerHook("drive_unmounted");
        return true;
    }

    if (!error.isEmpty()) {
        m_statusbar->Message(QString("Unmount error (%1)").arg(error.trimmed()),
                             MessageType::ERROR);
        return false;
    }

    return false;
}

void Navi::Search(const QString &term) noexcept {
    m_inputbar->disableCommandCompletions();
    if (!(term.isEmpty() || term.isNull())) {
        m_file_panel->Search(term);
        m_search_history_list.append(term);
    } else {
        QString searchText = m_inputbar->getInput("Search");
        m_file_panel->Search(searchText);
        m_search_history_list.append(searchText);
    }
    m_inputbar->enableCommandCompletions();
}

void Navi::Search(const std::string &_term) noexcept {
    m_inputbar->disableCommandCompletions();
    QString term = QString::fromStdString(_term);
    if (!(term.isNull() || term.isEmpty())) {
        m_file_panel->Search(term);
        m_search_history_list.append(term);
    } else {
        QString searchText = m_inputbar->getInput("Search");
        m_file_panel->Search(searchText);
        m_search_history_list.append(searchText);
    }
    m_inputbar->enableCommandCompletions();
}

void Navi::SearchRegex() noexcept {
    m_inputbar->disableCommandCompletions();
    QString searchText = m_inputbar->getInput("Search");
    m_file_panel->Search(searchText, true);
    m_search_history_list.append(searchText);
}

void Navi::ShellCommandAsync(const QString &com) noexcept {
    QString commandStr;
    if (com.isEmpty()) {
        commandStr = m_inputbar->getInput("Command");

        if (commandStr.isEmpty()) {
            m_statusbar->Message("Empty command!", MessageType::WARNING);
            return;
        }
    } else {
        commandStr = com;
    }

    Task *task = new Task();
    task->setTaskType(Task::TaskType::COMMAND);
    auto split = commandStr.split(" ");
    auto args = split.mid(1);
    auto command = split.at(0);
    task->setCommandString(command, args);
    m_task_manager->addTask(task);
}

void Navi::ToggleTasksWidget() noexcept {
    if (m_tasks_widget->isVisible()) {
        m_tasks_widget->hide();
    } else {
        m_tasks_widget->show();
    }
}

void Navi::ToggleTasksWidget(const bool &state) noexcept {
    m_tasks_widget->setVisible(state);
}

void Navi::TogglePathWidget() noexcept {
    if (m_file_path_widget->isVisible()) {
        m_file_path_widget->hide();
    } else {
        m_file_path_widget->show();
    }
}

void Navi::TogglePathWidget(const bool &state) noexcept {
    m_file_path_widget->setVisible(state);
}

bool Navi::event(QEvent *event) {
    if (event->type() == QEvent::Close) {
        if (m_task_manager->taskCount() != 0) {
            QMessageBox msgBox;
            msgBox.setText("There are tasks executing in the background, "
                           "do you really want to quit navi ?");
            msgBox.setWindowTitle("Tasks");
            msgBox.setStandardButtons(QMessageBox::StandardButton::Yes |
                                      QMessageBox::StandardButton::No);
            QAbstractButton *showTasksBtn =
                msgBox.addButton(tr("Open Tasks"), QMessageBox::ActionRole);
            auto btn_click_role = msgBox.exec();

            if (btn_click_role == QMessageBox::No) {
                event->ignore();
                return true;
            } else if (msgBox.clickedButton() == showTasksBtn) {
                event->ignore();
                ToggleTasksWidget(true);
                return true;
            }
        }
    }

    return QWidget::event(event);
}

void Navi::ToggleRegisterWidget() noexcept {
    if (m_register_widget->isVisible())
        m_register_widget->hide();
    else
        m_register_widget->show();
}

void Navi::ToggleRegisterWidget(const bool &state) noexcept {
    if (state)
        m_register_widget->show();
    else
        m_register_widget->hide();
}

std::string Navi::Lua__Input(const std::string &prompt,
                             const std::string &def_value,
                             const std::string &selection) noexcept {
    return m_inputbar->getInput(QString::fromStdString(prompt),
                                QString::fromStdString(def_value),
                                QString::fromStdString(selection)).toStdString();
}

/*void Navi::ExecuteLuaFunction(const QStringList &args) {*/
/*    if (args.isEmpty()) {*/
/*        QString funcName = m_inputbar->getInput("Enter the lua function name");*/
/*        try {*/
/*            lua[funcName.toStdString().c_str()]();*/
/*        } catch (const sol::error &e) {*/
/*            m_statusbar->Message(QString("Lua: %1").arg(e.what()),*/
/*                                 MessageType::ERROR);*/
/*        }*/
/*    } else {*/
/*        std::string luaFunc = args.at(0).toStdString();*/
/*        std::vector<std::string> _args = utils::convertToStdVector(args.mid(1));*/
/*        try {*/
/*            lua[luaFunc](_args);*/
/*        } catch (const sol::error &e) {*/
/*            m_statusbar->Message(QString("Lua: %1").arg(e.what()),*/
/*                                 MessageType::ERROR);*/
/*        }*/
/*    }*/
/*}*/

void Navi::execute_lua_code(const std::string &code) noexcept {
    // TODO: Adding literal ""
    (*m_lua).script(code);
}

QString Navi::GetInput(const QString &prompt,
                       const QString &title,
                       const QString &default_text) noexcept {
    bool ok;
    QString result = QInputDialog::getText(this, title, prompt, QLineEdit::Normal, default_text, &ok);
    if (ok) {
        return result;
    }

    return QString();
}

QString Navi::GetInput(const QString &prompt,
                       const QString &title,
                 const QStringList &choice,
                 const int &default_choice) noexcept {

    bool ok;
    QString result = QInputDialog::getItem(this, title, prompt, choice, default_choice, false, &ok);

    if (ok)
        return result;

    return QString();
}

void Navi::Lua__CreateFolders(const std::vector<std::string> &paths) noexcept {
    if (paths.empty())
        return;

    QStringList folders;
    for (const auto &path : paths)
    folders << QString::fromStdString(path);

    m_file_panel->NewFolder(folders);
}

void Navi::ToggleCycle() noexcept { m_file_panel->ToggleCycle(); }

void Navi::ToggleHeaders() noexcept { m_file_panel->ToggleHeaders(); }

void Navi::RenameItem() noexcept { m_file_panel->RenameItem(); }

void Navi::RenameItemsGlobal() noexcept { m_file_panel->RenameItemsGlobal(); }

void Navi::RenameItemsLocal() noexcept { m_file_panel->RenameItemsLocal(); }

void Navi::RenameDWIM() noexcept { m_file_panel->RenameDWIM(); }

void Navi::CopyItem() noexcept { m_file_panel->CopyItem(); }

void Navi::CopyItemsGlobal() noexcept { m_file_panel->CopyItemsGlobal(); }

void Navi::CopyItemsLocal() noexcept { m_file_panel->CopyItemsLocal(); }

void Navi::CopyDWIM() noexcept { m_file_panel->CopyDWIM(); }

void Navi::CutItem() noexcept { m_file_panel->CutItem(); }

void Navi::CutItemsGlobal() noexcept { m_file_panel->CutItemsGlobal(); }

void Navi::CutItemsLocal() noexcept { m_file_panel->CutItemsLocal(); }

void Navi::CutDWIM() noexcept { m_file_panel->CutDWIM(); }

void Navi::DeleteItem() noexcept { m_file_panel->DeleteItem(); }

void Navi::DeleteItemsGlobal() noexcept { m_file_panel->DeleteItemsGlobal(); }

void Navi::DeleteItemsLocal() noexcept { m_file_panel->DeleteItemsLocal(); }

void Navi::DeleteDWIM() noexcept { m_file_panel->DeleteDWIM(); }

void Navi::TrashItem() noexcept { m_file_panel->TrashItem(); }

void Navi::TrashItemsGlobal() noexcept { m_file_panel->TrashItemsGlobal(); }

void Navi::TrashItemsLocal() noexcept { m_file_panel->TrashItemsLocal(); }

void Navi::TrashDWIM() noexcept { m_file_panel->TrashDWIM(); }

void Navi::MarkItem() noexcept { m_file_panel->MarkItem(); }

void Navi::MarkInverse() noexcept { m_file_panel->MarkInverse(); }

void Navi::MarkAllItems() noexcept { m_file_panel->MarkAllItems(); }

void Navi::MarkDWIM() noexcept { m_file_panel->MarkDWIM(); }

void Navi::ToggleMarkItem() noexcept { m_file_panel->ToggleMarkItem(); }

void Navi::ToggleMarkDWIM() noexcept { m_file_panel->ToggleMarkDWIM(); }

void Navi::UnmarkItem() noexcept { m_file_panel->UnmarkItem(); }

void Navi::UnmarkItemsLocal() noexcept { m_file_panel->UnmarkItemsLocal(); }

void Navi::UnmarkItemsGlobal() noexcept { m_file_panel->UnmarkItemsGlobal(); }

void Navi::UnmarkDWIM() noexcept { m_file_panel->UnmarkDWIM(); }

void Navi::ForceUpdate() noexcept { m_file_panel->ForceUpdate(); }

void Navi::ChmodItem() noexcept { m_file_panel->ChmodItem(); }

void Navi::ChmodItemsLocal() noexcept { m_file_panel->ChmodItemsLocal(); }

void Navi::ChmodItemsGlobal() noexcept { m_file_panel->ChmodItemsGlobal(); }

void Navi::ChmodDWIM() noexcept {
    // m_file_panel->ChmodDWIM();
}

void Navi::ShowItemPropertyWidget() noexcept {
    m_file_panel->ShowItemPropertyWidget();
}

void Navi::UpDirectory() noexcept { m_file_panel->UpDirectory(); }

void Navi::SelectItem() noexcept { m_file_panel->SelectItem(); }

void Navi::NextItem() noexcept { m_file_panel->NextItem(); }

void Navi::PrevItem() noexcept { m_file_panel->PrevItem(); }

void Navi::GotoFirstItem() noexcept { m_file_panel->GotoFirstItem(); }

void Navi::GotoLastItem() noexcept { m_file_panel->GotoLastItem(); }

void Navi::GotoMiddleItem() noexcept { m_file_panel->GotoMiddleItem(); }

void Navi::ToggleMouseScroll() noexcept { m_file_panel->ToggleMouseScroll(); }

void Navi::ToggleVisualLine() noexcept { m_file_panel->ToggleVisualLine(); }

void Navi::SearchPrev() noexcept { m_file_panel->SearchPrev(); }

void Navi::SearchNext() noexcept { m_file_panel->SearchNext(); }

void Navi::NewFolder(const QStringList &names) noexcept {
    m_file_panel->NewFolder(names);
}

void Navi::new_folder(const std::vector<std::string> &_folders) noexcept {
    QStringList folders = utils::stringListFromVector(_folders);
    m_file_panel->NewFolder(folders);
}

void Navi::NewFile(const QStringList &names) noexcept {
    m_file_panel->NewFile(names);
}

void Navi::new_file(const std::vector<std::string> &_files) noexcept {
    QStringList files = utils::stringListFromVector(_files);
    m_file_panel->NewFile(files);
}

void Navi::PasteItems() noexcept { m_file_panel->PasteItems(); }

void Navi::addCommandToMacroRegister(const QStringList &commandlist) noexcept {
    m_macro_register.append(commandlist.join(" "));
}

void Navi::addCommandToMacroRegister(const QString &command) noexcept {
    m_macro_register.append(command);
}

void Navi::GotoItem(const int &num) noexcept { m_file_panel->GotoItem(num); }

void Navi::PlayMacro() noexcept {
    QString macro_key = m_inputbar->getInput("Enter macro key");

    if (m_macro_hash.contains(macro_key)) {
        for (const auto &com : m_macro_register) {
            ProcessCommand(com);
        }
    } else {
        m_statusbar->Message("No macro recorded under that key",
                             MessageType::WARNING);
    }
}

void Navi::MarkRegex() noexcept { m_file_panel->MarkRegex(); }

void Navi::mark_regex(const std::string &regex) noexcept {
    m_file_panel->MarkRegex(QString::fromStdString(regex));
}

void Navi::highlight(const std::string &item_name) noexcept {
    m_file_panel->HighlightItemWithBaseName(QString::fromStdString(item_name));
}

void Navi::UnmarkRegex() noexcept { m_file_panel->UnmarkRegex(); }

void Navi::unmark_regex(const std::string &term) noexcept {
    m_file_panel->UnmarkRegex(QString::fromStdString(term));
}

void Navi::ChangeDirectory(const QString &path) noexcept {
    if (path.isEmpty()) {
        QString path = m_inputbar->getInput("Enter path to cd");
        if (!path.isEmpty())
            m_file_panel->setCurrentDir(path);
    } else {
        m_file_panel->setCurrentDir(path);
    }
}

void Navi::change_directory(const std::string &path) noexcept {
    if (path.empty()) {
        QString path = m_inputbar->getInput("Enter path to cd");
        if (!path.isEmpty())
            m_file_panel->setCurrentDir(path);
    } else {
        m_file_panel->setCurrentDir(QString::fromStdString(path));
    }
}

void Navi::SpawnProcess(const QString &command,
                        const QStringList &args) noexcept {}


void Navi::cacheThumbnails() noexcept {
    if (m_thumbnail_cache_future_watcher->isRunning()) {
        m_thumbnail_cache_future_watcher->cancel();
        m_thumbnail_cache_future_watcher->waitForFinished();
    }

    QStringList files = m_file_panel->model()->files();
    m_thumbnail_cache_future =
        QtConcurrent::run(&Thumbnailer::generate_thumbnails,
                          m_preview_panel->thumbnailer(), files);

}

void Navi::LaunchNewInstance() noexcept {
    // Get the path to the current application executable
    QString appPath = QCoreApplication::applicationFilePath();

    // Launch a new instance
    QProcess::startDetached(appPath);
}

void Navi::ShowFolderProperty() noexcept {
    FolderPropertyWidget *f = new FolderPropertyWidget(m_file_panel->getCurrentDir(), this);
}

void Navi::Exit() noexcept { QApplication::quit(); }

void Navi::CopyPath(const QString &separator) noexcept {
    auto selectionModel = m_file_panel->tableView()->selectionModel();
    if (selectionModel->hasSelection()) {
        auto current_dir = m_file_panel->getCurrentDir();
        auto indexes = selectionModel->selectedIndexes();
        QStringList itemPathList =
            m_file_panel->model()->getFilePathsFromIndexList(indexes);

        // TODO: index list returns other columns

        if (separator.isEmpty())
            m_clipboard->setText(itemPathList.join(m_copy_path_join_str));
        else
            m_clipboard->setText(itemPathList.join(separator));
    }
}

void Navi::SelectAllItems() noexcept {
    m_file_panel->tableView()->selectAll();
}

void Navi::SelectInverse() noexcept {
    auto table = m_file_panel->tableView();
    auto rows = table->model()->rowCount();
    auto selectionModel = table->selectionModel();
    auto model = m_file_panel->model();

    for (int row = 0; row < rows; row++) {
        QModelIndex index = model->index(row, 0);
        if (selectionModel->isRowSelected(row))
            selectionModel->select(index, QItemSelectionModel::Rows |
                                   QItemSelectionModel::Deselect);
        else
            selectionModel->select(index, QItemSelectionModel::Rows |
                                   QItemSelectionModel::Select);
    }
}

Navi::MenuItem Navi::Lua__parseMenuItem(const sol::table &table) noexcept {
    MenuItem item;

    // Extract the label
    item.label = table["label"];

    // Extract the action
    if (table["action"].valid()) {
        sol::function luaAction = table["action"];
        item.action = [luaAction]() { luaAction(); };
    }

    // Extract submenus recursively
    if (table["submenu"].valid()) {
        sol::table submenuTable = table["submenu"];
        for (const auto& kvp : submenuTable) {
            if (kvp.second.is<sol::table>()) {
                item.submenu.push_back(Lua__parseMenuItem(kvp.second.as<sol::table>()));
            }
        }
    }

    return item;
}


void Navi::Lua__AddMenu(const sol::table& menuTable) noexcept {
    Navi::MenuItem menu = Lua__parseMenuItem(menuTable);
    QMenu *qmenu = new QMenu(QString::fromStdString(menu.label));

    if (!menu.submenu.empty()) {
        for (const auto &subitem : menu.submenu) {
            // If no submenu, add as an action
            if (subitem.submenu.empty()) {
                QAction *action =
                    new QAction(QString::fromStdString(subitem.label));
                qmenu->addAction(action);
                connect(action, &QAction::triggered, this, [subitem]() {
                    subitem.action();
                });
            } else { // else, add it as a menu
                QMenu *submenu = new QMenu(QString::fromStdString(subitem.label));
                qmenu->addMenu(submenu);
                for (const auto &subitem : subitem.submenu) {
                    QAction *action =
                        new QAction(QString::fromStdString(subitem.label));
                    submenu->addAction(action);
                    connect(action, &QAction::triggered, this, [subitem]() {
                        subitem.action();
                    });
                }
            }
        }
    }

    m_menubar->addMenu(qmenu);
}

void Navi::FullScreen(const bool &state) noexcept {
    if (state)
        showFullScreen();
    else
        showNormal();
}

void Navi::FullScreen() noexcept {
    if (isFullScreen())
        showNormal();
    else
        showFullScreen();
}

void Navi::Lua__AddContextMenu(const sol::table &table) noexcept {
    // m_file_panel->setContextMenu(cmenu);
}

Navi::ToolbarItem Navi::Lua__parseToolbarItem(const sol::table &table) noexcept {
    ToolbarItem item;

    // Extract the label
    item.label = table["label"];

    // Extract the action
    if (table["action"].valid()) {
        sol::function luaAction = table["action"];
        item.action = [luaAction]() { luaAction(); };
    }

    item.icon = table["icon"];

    item.position = table["position"];

    return item;
}

void Navi::Lua__AddToolbarButton(const sol::object &object) noexcept {

    auto toolbar_item = object.as<ToolbarItem>();

    QPushButton *widget = new QPushButton(QString::fromStdString(toolbar_item.label));
    QIcon icon = QIcon::fromTheme(QString::fromStdString(toolbar_item.icon));
    QString tooltip = QString::fromStdString(toolbar_item.tooltip);

    auto temp = [&]() {
        widget->setText("");
        widget->setIcon(icon);
        widget->setToolTip(tooltip);
    };

    if (icon.isNull()) {
        auto iconpath = QString::fromStdString(toolbar_item.icon);
        iconpath = iconpath.replace("~", QDir::homePath());
        QFileInfo file(iconpath);
        if (file.exists()) {
            QIcon icon(iconpath);
            temp();
        }
    } else {
        temp();
    }

    connect(widget, &QPushButton::clicked, this, toolbar_item.action);

    if (toolbar_item.position != -1) {
        QAction* before = m_toolbar->actions().at(toolbar_item.position - 1);
        m_toolbar->insertWidget(before, widget);
    } else {
        m_toolbar->addWidget(widget);
    }

}

void Navi::Lua__AddToolbarButton(const ToolbarItem &toolbar_item) noexcept {

    if (toolbar_item.label.empty() && toolbar_item.icon.empty()) {
        qWarning() << "Label and icon is empty";
        return;
    }

    QPushButton *widget = new QPushButton(QString::fromStdString(toolbar_item.label));
    QIcon icon = QIcon::fromTheme(QString::fromStdString(toolbar_item.icon));
    QString tooltip = QString::fromStdString(toolbar_item.tooltip);

    auto temp = [&]() {
        widget->setText("");
        widget->setIcon(icon);
        widget->setToolTip(tooltip);
    };

    if (icon.isNull()) {
        auto iconpath = QString::fromStdString(toolbar_item.icon);
        iconpath = iconpath.replace("~", QDir::homePath());
        QFileInfo file(iconpath);
        if (file.exists()) {
            QIcon icon(iconpath);
            temp();
        }
    } else {
        temp();
    }

    if (toolbar_item.action) {
        connect(widget, &QPushButton::clicked, this, toolbar_item.action);
    }

    if (toolbar_item.position != -1) {
        QAction* before = m_toolbar->actions().at(toolbar_item.position - 1);
        m_toolbar->insertWidget(before, widget);
    } else {
        m_toolbar->addWidget(widget);
    }

}

void Navi::initToolbar() noexcept {

    auto style = this->style();
    m_toolbar__home_btn = new QPushButton(QIcon::fromTheme(QIcon::ThemeIcon::GoHome), "");
    m_toolbar__next_btn = new QPushButton(style->standardIcon(QStyle::SP_ArrowForward), "");
    m_toolbar__prev_btn = new QPushButton(style->standardIcon(QStyle::SP_ArrowBack), "");
    m_toolbar__parent_btn = new QPushButton(QIcon::fromTheme(QIcon::ThemeIcon::GoUp), "");
    m_toolbar__refresh_btn = new QPushButton(QIcon::fromTheme(QIcon::ThemeIcon::ViewRefresh), "");

    m_toolbar__home_btn->setToolTip("Go to Home folder");
    m_toolbar__next_btn->setToolTip("Go to Next folder");
    m_toolbar__prev_btn->setToolTip("Go to Previous folder");
    m_toolbar__parent_btn->setToolTip("Go to Parent folder");
    m_toolbar__refresh_btn->setToolTip("Refresh folder");

    connect(m_toolbar__prev_btn, &QPushButton::clicked, m_file_panel,
            &FilePanel::PreviousDirectory);

    connect(m_toolbar__next_btn, &QPushButton::clicked, this, [&]() {
        qDebug() << "TODO: Next Location";
    });

    connect(m_toolbar__home_btn, &QPushButton::clicked, m_file_panel, &FilePanel::HomeDirectory);
    connect(m_toolbar__parent_btn, &QPushButton::clicked, m_file_panel,
            &FilePanel::UpDirectory);

    connect(m_toolbar__refresh_btn, &QPushButton::clicked, m_file_panel, &FilePanel::ForceUpdate);
}

void Navi::ShowAbout() noexcept {
    AboutWidget *about = new AboutWidget(this);
}

void Navi::SortAscending(const bool &state) noexcept {
    if (state) {
        if (m_sort_flags & QDir::SortFlag::Reversed) {
            QString itemName = m_file_panel->getCurrentItem();
            m_sort_flags &= ~QDir::SortFlag::Reversed;
            m_file_panel->model()->setSortBy(m_sort_flags);
            m_file_panel->HighlightItem(itemName);
        }
    }
}

void Navi::SortDescending(const bool &state) noexcept {
    if (state) {
        if (m_sort_flags & QDir::SortFlag::Reversed)
            return;
        QString itemName = m_file_panel->getCurrentItem();
        m_sort_flags |= QDir::SortFlag::Reversed;
        m_file_panel->model()->setSortBy(m_sort_flags);
        m_file_panel->HighlightItem(itemName);
    }
}

void Navi::Lua__SetToolbarItems(const sol::table &table) noexcept {

    m_toolbar->clear();

    for (size_t i=1; i <= table.size(); i++) {
        auto temp = table[i];
        if (temp.is<std::string>()) {
            auto item_name = temp.get<std::string>();

            if (item_name == "previous_dir")
                m_toolbar->addWidget(m_toolbar__prev_btn);
            else if (item_name == "next_dir")
                m_toolbar->addWidget(m_toolbar__next_btn);
            else if (item_name == "parent_dir")
                m_toolbar->addWidget(m_toolbar__parent_btn);
            else if (item_name == "home")
                m_toolbar->addWidget(m_toolbar__home_btn);
            else if (item_name == "refresh")
                m_toolbar->addWidget(m_toolbar__refresh_btn);
        } else if (temp.is<Navi::ToolbarItem>()) {
            Lua__AddToolbarButton(temp.get<Navi::ToolbarItem>());
        }
    }
}


Navi::ToolbarItem Navi::Lua__CreateToolbarButton(const std::string &name,
                                                 const sol::table &table) noexcept {
    Navi::ToolbarItem item;

    item.name = name;
    item.icon = table["icon"].get_or<std::string>("");
    item.label = table["label"].get_or<std::string>("");
    item.tooltip = table["tooltip"].get_or<std::string>("");

    // Extract the action
    if (table["action"].valid()) {
        sol::function luaAction = table["action"];
        item.action = [luaAction]() { luaAction(); };
    }

    item.position = table["position"].get_or(-1);

    return item;
}

sol::table Navi::list_runtime_paths(sol::this_state L) noexcept {
    sol::state_view lua(L);
    sol::table table = lua.create_table();
    auto rtps = m_runtime_path.values();

    for (int i=0; i < rtps.size(); i++)
        table[i + 1] = rtps.at(i).toStdString();

    return table;
}

void Navi::update_lua_package_path(sol::state &lua) noexcept {
    QStringList visitedDirs;  // To track directories already added to package.path

    lua.script(R"(
               package = package or {}
               package.path = package.path or ""
               )");

    // Create a QDirIterator to iterate through all subdirectories and files
    QDirIterator it(CONFIG_DIR_PATH, QStringList() << "*.lua", QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QString filePath = it.next();
        QString parentDir = QFileInfo(filePath).absolutePath();  // Get the parent directory

        // Add to package.path only if this directory hasn't been added yet
        if (!visitedDirs.contains(parentDir)) {
            m_runtime_path.insert(parentDir);
            lua.script("package.path = package.path .. ';" + parentDir.toStdString() + "/?.lua'");
            visitedDirs.append(parentDir);  // Mark the directory as visited
        }
    }
}

void Navi::Lua__keymap_set_from_table(const sol::table &table) noexcept {

    if (!table["key"]) {
        m_statusbar->Message("No key found in the keybinding table", MessageType::WARNING);
        return;
    }

    if (!table["command"]) {
        m_statusbar->Message("No command found in the keybinding table", MessageType::WARNING);
        return;
    }

    Keybind kb {
        .key = QString::fromStdString(table["key"].get_or<std::string>("")),
        .command = QString::fromStdString(table["command"].get_or<std::string>("")),
        .desc = QString::fromStdString(table["desc"].get_or<std::string>(""))
    };
    m_keybind_list.append(kb);

    QShortcut *shortcut = new QShortcut(QKeySequence(kb.key), this);
    connect(shortcut, &QShortcut::activated, this,
            [&, kb]() { ProcessCommand(kb.command); });

}

void Navi::Lua__keymap_set(const std::string &key,
                           const std::string &command,
                           const std::string &desc) noexcept {

    Keybind kb {
        .key = QString::fromStdString(key),
        .command = QString::fromStdString(command),
        .desc = QString::fromStdString(desc)
    };

    m_keybind_list.append(kb);

    QShortcut *shortcut = new QShortcut(QKeySequence(kb.key), this);
    connect(shortcut, &QShortcut::activated, this,
            [&, kb]() {
            ProcessCommand(kb.command);
            });
}


void Navi::Lua__register_user_function(const std::string &name, const sol::function &func) noexcept {
    commandMap[QString::fromStdString(name)] = [func](const QStringList &args) {
        if (!args.isEmpty()) {
            auto lua_args = utils::convertToStdVector(args);
            func(sol::as_args(lua_args));
        }
        else
            func();
    };
}

void Navi::Lua__unregister_user_function(const std::string &name) noexcept {
    commandMap.remove(QString::fromStdString(name));
}

void Navi::Set_default_directory(const QString &dir) noexcept {
    m_default_dir = dir;
}

void Navi::Set_default_directory(const std::string &dir) noexcept {
    m_default_dir = QString::fromStdString(dir);
}

void Navi::init_default_options() noexcept {
    /*set_menubar_icons(true);*/
}

void Navi::Set_auto_save_bookmarks(const bool &state) noexcept {
    m_auto_save_bookmarks = state;
}

void Navi::Set_toolbar_icons_only() noexcept {
    m_toolbar_icons_only = true;
    auto style = this->style();
    m_toolbar__prev_btn = new QPushButton(style->standardIcon(QStyle::SP_ArrowBack), "");
    m_toolbar__next_btn = new QPushButton(style->standardIcon(QStyle::SP_ArrowForward), "");
    m_toolbar__home_btn = new QPushButton(QIcon::fromTheme(QIcon::ThemeIcon::GoHome), "");
    m_toolbar__parent_btn = new QPushButton(QIcon::fromTheme(QIcon::ThemeIcon::GoUp), "");
    m_toolbar__refresh_btn = new QPushButton(QIcon::fromTheme(QIcon::ThemeIcon::ViewRefresh), "");
}

void Navi::Set_toolbar_text_only() noexcept {
    m_toolbar_icons_only = false;
    m_toolbar__prev_btn = new QPushButton("Previous");
    m_toolbar__next_btn = new QPushButton("Next");
    m_toolbar__home_btn = new QPushButton("Home");
    m_toolbar__parent_btn = new QPushButton("Parent");
    m_toolbar__refresh_btn = new QPushButton("Refresh");
}

void Navi::set_toolbar_layout(const sol::table &layout) noexcept {

    m_toolbar->clear();
    m_toolbar_layout.clear();
    m_toolbar_layout.reserve(layout.size());

    for (size_t i=1; i <= layout.size(); i++) {
        auto tmp = layout[i].get_type();

        switch(tmp) {

            case (sol::type::string): {
                auto item = layout[i].get<std::string>();

                if (item == "previous_directory") {
                    m_toolbar->addWidget(m_toolbar__prev_btn);
                    m_toolbar_layout.push_back("previous_dir");
                }

                else if (item == "next_directory") {
                    m_toolbar->addWidget(m_toolbar__next_btn);
                    m_toolbar_layout.push_back("next_dir");
                }

                else if (item == "parent_directory") {
                    m_toolbar->addWidget(m_toolbar__parent_btn);
                    m_toolbar_layout.push_back("parent_dir");
                }

                else if (item == "home") {
                    m_toolbar->addWidget(m_toolbar__home_btn);
                    m_toolbar_layout.push_back("home");
                }

                else if (item == "refresh") {
                    m_toolbar->addWidget(m_toolbar__refresh_btn);
                    m_toolbar_layout.push_back("refresh");
                }
            }
                break;

            case sol::type::userdata : {
                auto item = layout[i].get<ToolbarItem>();
                Lua__AddToolbarButton(item);
            }
                break;

            default:
                qWarning() << "Not handled toolbar type";
        }

    }
}

void Navi::set_menubar_icons(const bool &state) noexcept {
    m_menubar_icons = state;
    if (state) {
        m_filemenu__new_window->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::WindowNew));
        m_filemenu__create_new_folder->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::FolderNew));
        m_filemenu__create_new_file->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew));
        m_filemenu__close_window->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::WindowClose));
        m_filemenu__folder_properties->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentProperties));
        m_viewmenu__refresh->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::ViewRefresh));
        m_viewmenu__fullscreen->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::ViewFullscreen));
        m_tools_menu__search->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::SystemSearch));
        m_tools_menu__find_files->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditFind));
        m_edit_menu__open->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen));
        m_edit_menu__copy->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditCopy));
        m_edit_menu__paste->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditPaste));
        m_edit_menu__delete->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditDelete));
        m_edit_menu__item_property->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentProperties));
        m_edit_menu__cut->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditCut));
        m_edit_menu__select_all->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditSelectAll));
        m_viewmenu__sort_ascending->setIcon(QIcon::fromTheme("view-sort-ascending"));
        m_viewmenu__sort_descending->setIcon(QIcon::fromTheme("view-sort-descending"));
        m_edit_menu__trash->setIcon(QIcon::fromTheme("user-trash"));
        m_bookmarks_menu__bookmarks_list_menu->setIcon(QIcon::fromTheme("user-bookmarks"));
        m_bookmarks_menu__add->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::AddressBookNew));
        m_bookmarks_menu__remove->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::ListRemove));
        m_go_menu__connect_to_server->setIcon(QIcon::fromTheme("network-server"));
        m_go_menu__home_folder->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::GoHome));
        m_go_menu__parent_folder->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::GoUp));
        m_go_menu__previous_folder->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::GoPrevious));
        m_go_menu__next_folder->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::GoNext));
        m_viewmenu__filter->setIcon(QIcon::fromTheme("view-filter"));
        m_edit_menu__rename->setIcon(QIcon(":resources/images/pencil-icon.svg"));
        m_edit_menu__copy_path->setIcon(QIcon(":resources/images/location.svg"));
        m_filemenu__create_new_menu->setIcon(QIcon(":resources/images/plus.svg"));
        m_edit_menu__select_inverse->setIcon(QIcon(":resources/images/reverse.svg"));
        m_viewmenu__sort_menu->setIcon(QIcon(":resources/images/sort.svg"));
        m_viewmenu__messages->setIcon(QIcon(":resources/images/messages.svg"));
        m_viewmenu__messages->setIcon(QIcon(":resources/images/messages.svg"));
        m_viewmenu__tasks_widget->setIcon(QIcon(":resources/images/clock.svg"));
        m_viewmenu__menubar->setIcon(QIcon(":resources/images/menu.svg"));
        m_viewmenu__statusbar->setIcon(QIcon(":resources/images/statusbar.svg"));
        m_viewmenu__preview_panel->setIcon(
            QIcon(":resources/images/preview.svg"));
        m_help_menu__about->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout));
    } else {
        m_filemenu__new_window->setIconVisibleInMenu(false);
        m_filemenu__create_new_folder->setIconVisibleInMenu(false);
        m_filemenu__create_new_file->setIconVisibleInMenu(false);
        m_filemenu__close_window->setIconVisibleInMenu(false);
        m_filemenu__folder_properties->setIconVisibleInMenu(false);
        m_viewmenu__refresh->setIconVisibleInMenu(false);
        m_viewmenu__fullscreen->setIconVisibleInMenu(false);
        m_tools_menu__search->setIconVisibleInMenu(false);
        m_tools_menu__find_files->setIconVisibleInMenu(false);
        m_edit_menu__open->setIconVisibleInMenu(false);
        m_edit_menu__copy->setIconVisibleInMenu(false);
        m_edit_menu__paste->setIconVisibleInMenu(false);
        m_edit_menu__delete->setIconVisibleInMenu(false);
        m_edit_menu__item_property->setIconVisibleInMenu(false);
        m_edit_menu__cut->setIconVisibleInMenu(false);
        m_edit_menu__select_all->setIconVisibleInMenu(false);
        m_viewmenu__sort_ascending->setIconVisibleInMenu(false);
        m_viewmenu__sort_descending->setIconVisibleInMenu(false);
        m_edit_menu__trash->setIconVisibleInMenu(false);
        m_bookmarks_menu__bookmarks_list_menu->setIcon(QIcon());
        m_bookmarks_menu__add->setIconVisibleInMenu(false);
        m_bookmarks_menu__remove->setIconVisibleInMenu(false);
        m_go_menu__connect_to_server->setIconVisibleInMenu(false);
        m_go_menu__home_folder->setIconVisibleInMenu(false);
        m_go_menu__parent_folder->setIconVisibleInMenu(false);
        m_go_menu__previous_folder->setIconVisibleInMenu(false);
        m_go_menu__next_folder->setIconVisibleInMenu(false);
        m_viewmenu__filter->setIconVisibleInMenu(false);
        m_edit_menu__rename->setIconVisibleInMenu(false);
        m_edit_menu__copy_path->setIconVisibleInMenu(false);
        m_filemenu__create_new_menu->setIcon(QIcon());
        m_edit_menu__select_inverse->setIconVisibleInMenu(false);
        m_viewmenu__sort_menu->setIcon(QIcon());
        m_viewmenu__messages->setIconVisibleInMenu(false);
        m_viewmenu__messages->setIconVisibleInMenu(false);
        m_viewmenu__tasks_widget->setIconVisibleInMenu(false);
        m_viewmenu__menubar->setIconVisibleInMenu(false);
        m_viewmenu__statusbar->setIconVisibleInMenu(false);
        m_viewmenu__preview_panel->setIconVisibleInMenu(false);
        m_help_menu__about->setIconVisibleInMenu(false);
    }
}

void Navi::Set_preview_pane_fraction(const double &fraction) noexcept {
    auto totalSize = m_splitter->width();
    m_preview_pane_fraction = fraction;
    QList<int> sizes = {static_cast<int>(totalSize * (1 - fraction)),
        static_cast<int>(totalSize * fraction)};
    m_splitter->setSizes(sizes);
}

double Navi::Preview_pane_fraction() noexcept {
    return m_preview_pane_fraction;
}

void Navi::update_runtime_paths(const std::string &_rtps) noexcept {
    QString rtps = QString::fromStdString(_rtps);
    QStringList rtps_list = rtps.split(";");
    m_runtime_path = QSet<QString>(rtps_list.constBegin(), rtps_list.constEnd());
}

void Navi::execute_shell_command(const std::string &_command,
                           std::vector<std::string> &_args) noexcept {
    QStringList args = utils::stringListFromVector(_args);
    SpawnProcess(QString::fromStdString(_command), args);
}


void Navi::set_inputbar_props(const sol::table &table) noexcept {

    if (m_inputbar) {
        if (table["font_size"].valid())
            m_inputbar->set_font_size(table["font_size"].get<int>());

        if (table["foreground"].valid())
            m_inputbar->setForeground(table["foreground"].get<std::string>());

        if (table["background"].valid())
            m_inputbar->setBackground(table["background"].get<std::string>());

        if (table["font"].valid())
            m_inputbar->setFontFamily(table["font"].get<std::string>());
    }
}

void Navi::set_header_columns(const sol::table &columns) noexcept {
    QList<FileSystemModel::Column> columnList; // List to store column configuration
    columnList.reserve(columns.size());
    FileSystemModel::Column column;
    bool file_name_type_check = false;
    for (std::size_t i = 1; i <= columns.size(); i++) {
        auto col = columns[i];
        auto name = QString::fromStdString(col["name"].get_or<std::string>(""));
        auto type = QString::fromStdString(col["type"].get_or<std::string>(""));

        if (type.isEmpty() || type.isNull()) {
            m_statusbar->Message(
                "*type* key is mandatory for columns."
                "Consider adding it to get the columns working",
                MessageType::ERROR);
            continue;
        }

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
        return;
    }

    m_file_panel->model()->setColumns(columnList);
}


sol::table Navi::get_preview_panel_props(sol::this_state L) const noexcept {
    sol::state_view lua(L);
    sol::table table = lua.create_table();

    table["image_dimension"] = m_preview_panel->get_preview_dimension();
    table["fraction"] = m_preview_pane_fraction;
    table["visible"] = m_preview_panel->isVisible();
    table["max_file_size"] = m_preview_panel->max_preview_threshold();

    return table;
}

void Navi::set_pathbar_props(const sol::table &table) noexcept {
// TODO:
}

void Navi::set_api_list(const QStringList &list) noexcept {
    m_navi_lua_api_list = list;
}


void Navi::set_toolbar_props(const sol::table &table) noexcept {

    if (table["visible"])
        set_toolbar_visible(table["visible"].get<bool>());

    if (table["icons_only"])
        set_toolbar_icons_only(table["icons_only"].get<bool>());

}


void Navi::set_symlink_props(const sol::table &table) noexcept {
    if (table["font"])
        set_symlink_font(table["font"].get<std::string>());

    if (table["font_size"])
        set_symlink_font_size(table["font_size"].get<int>());

    if (table["foreground"])
        set_symlink_foreground(table["foreground"].get<std::string>());

    if (table["background"])
        set_symlink_background(table["background"].get<std::string>());

    if (table["visible"])
        set_symlink_visible(table["visible"].get<bool>());

    if (table["bold"])
        set_symlink_bold(table["bold"].get<bool>());

    if (table["italic"])
        set_symlink_italic(table["italic"].get<bool>());

    if (table["underline"])
        set_symlink_underline(table["underline"].get<bool>());

    if (table["separator"])
        set_symlink_separator(table["separator"].get<std::string>());
}

sol::table Navi::get_symlink_props() noexcept {
    sol::table table = m_lua->create_table();

    table["underline"] = get_symlink_underline();
    table["italic"] = get_symlink_italic();
    table["font"] = get_symlink_font();
    table["font_size"] = get_symlink_font_size();
    table["bold"] = get_symlink_bold();
    table["foreground"] = get_symlink_foreground();
    table["background"] = get_symlink_background();
    table["visible"] = get_symlink_visible();

    return table;
}

void Navi::copy_to() noexcept {
    m_file_panel->copy_to();
}

void Navi::move_to() noexcept {
    m_file_panel->move_to();
}
