#include "Navi.hpp"

// Run the function if there is any error due to ``reason``.

void Navi::Error(const QString &reason) noexcept {
    m_statusbar->Message(reason);
    m_table_delegate->set_symlink_font(font().family());
}

void Navi::initThings() noexcept {
    initValidCommandsList();
    initLayout();
    initCompletion();
    initBookmarks();
    initMenubar();
    initToolbar();
    setupCommandMap();
    initSignalsSlots();
    init_default_options();

    m_file_panel->tableView()->setIconSize(QSize(64, 64));

    if (m_default_dir == ".")
        m_default_dir = QDir::currentPath();
    else if (m_default_dir.isNull() || m_default_dir.isEmpty())
        m_default_dir = QDir::homePath();

    m_default_dir = QFileInfo(m_default_dir).absoluteFilePath();
    m_file_panel->setCurrentDir(m_default_dir, true);
    m_thumbnail_cache_future_watcher->setFuture(m_thumbnail_cache_future);

    m_file_panel->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
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
    m_bookmark_manager = new BookmarkManager(this);

    try {
        m_bookmarks_state.script_file(BOOKMARK_FILE_PATH.toStdString(),
                                      sol::load_mode::any);
    } catch (const sol::error &e) {
        m_statusbar->Message(QString::fromStdString(e.what()));
        return;
    }

    // Read the BOOKMARKS table
    sol::optional<sol::table> bookmarks_table_opt =
        m_bookmarks_state["BOOKMARKS"];

    if (bookmarks_table_opt) {
        sol::table bookmarks_table = bookmarks_table_opt.value();
        QHash<QString, BookmarkManager::Bookmark> bookmarks_hash;
        for (const auto &pair : bookmarks_table) {
            auto name = QString::fromStdString(pair.first.as<std::string>());
            auto bookmark = pair.second.as<sol::table>();
            BookmarkManager::Bookmark item{
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
            this, [&]() { LogMessage("Thumbnail caching finished"); });

    connect(m_file_panel, &FilePanel::currentItemChanged, this,
            [&]() { m_hook_manager->triggerHook("item_changed"); });

    connect(m_bookmark_manager, &BookmarkManager::bookmarksChanged, this,
            [&]() { m_bookmarks_buffer->loadBookmarks(); });

    connect(m_bookmarks_buffer, &BookmarkWidget::bookmarkGoRequested, this,
            [&](const QString &name) { GoBookmark(name); });

    connect(m_drives_widget, &DriveWidget::driveLoadRequested, this,
            [&](const QString &mountPoint) {
            m_file_panel->setCurrentDir(mountPoint);
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
            QString confirm = m_inputbar->getInput(
            QString("Do you want to unmount %1 ? (y, N)").arg(driveName));
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
            m_preview_panel->clearPreview();
            m_hook_manager->triggerHook("directory_changed");
            m_preview_panel->clearImageCache();
            // TODO: Fix thumbnail caching
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
            m_statusbar->Message(
            QString("Error during file operation! (%1)").arg(reason),
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

    commandMap["notify"] = [this](const QStringList &args) {
        if (args.size() == 2) {
            NotificationDialog::NotificationType type;
            auto type_str = args.at(1).toLower();

            if (type_str == "info")
                type = NotificationDialog::NotificationType::INFO;

            if (type_str == "error")
                type = NotificationDialog::NotificationType::ERROR;

            if (type_str == "warning")
                type = NotificationDialog::NotificationType::WARNING;

            notify(type, args.at(0));
        }
    };

    commandMap["update"] = [this](const QStringList &args) {
        UNUSED(args);
        CheckForUpdates();
    };

    commandMap["execute-extended-command"] = [this](const QStringList &args) {
        UNUSED(args);
        ExecuteExtendedCommand();
    };

    commandMap["scroll-down"] = [this](const QStringList &args) noexcept {
        UNUSED(args);
        m_file_panel->NextPage();
    };

    commandMap["scroll-up"] = [this](const QStringList &args) noexcept {
        UNUSED(args);
        m_file_panel->PrevPage();
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

    commandMap["screenshot"] = [this](const QStringList &args) {
        UNUSED(args);
        screenshot();
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
        ShortcutsBuffer();
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

    commandMap["new-file"] = [this](const QStringList &args) { NewFile(args); };

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
    if (m_bookmark_manager->setBookmarkFile(bookmarkName, newBookmarkPath,
                                            false)) {
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

    if (m_bookmark_manager->addBookmark(
        bookmarkName, m_file_panel->getCurrentDir(), highlight)) {
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
    } else
    m_statusbar->Message(
        QString("Error removing bookmark %1").arg(bookmarkName),
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

void Navi::ShortcutsBuffer() noexcept {
    if (m_widget_hash.contains(Widget::Shortcuts)) {
        auto widget = m_widget_hash[Widget::Shortcuts];
        widget->show();
    } else {
        auto widget = new ShortcutsWidget(m_keybind_list, this);
        widget->show();
        m_widget_hash[Widget::Shortcuts] = widget;
    }
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
        connect(m_file_panel, &FilePanel::currentItemChanged, m_preview_panel,
                &PreviewPanel::onFileSelected);
        m_preview_panel->onFileSelected(getCurrentFile());
    } else {
        disconnect(m_file_panel, &FilePanel::currentItemChanged, m_preview_panel,
                   &PreviewPanel::onFileSelected);
    }
    m_preview_panel_dock->toggleAction();
}

void Navi::TogglePreviewPanel() noexcept {
    bool visible = m_preview_panel_dock->isVisible();
    if (visible) {
        disconnect(m_file_panel, &FilePanel::currentItemChanged, m_preview_panel,
                   &PreviewPanel::onFileSelected);
    } else {
        connect(m_file_panel, &FilePanel::currentItemChanged, m_preview_panel,
                &PreviewPanel::onFileSelected);
        m_preview_panel->onFileSelected(getCurrentFile());
    }
    m_preview_panel_dock->toggleAction()->trigger();
}

QString Navi::getCurrentFile() noexcept {
    return m_file_panel->getCurrentItem();
}

void Navi::initLayout() noexcept {

    m_toolbar = new QToolBar();
    addToolBar(m_toolbar);
    m_tasks_widget = new TasksWidget(m_task_manager);
    m_hook_manager = new HookManager();
    m_inputbar = new Inputbar();
    m_statusbar = new Statusbar();
    m_file_panel = new FilePanel(m_inputbar, m_statusbar, m_hook_manager,
                                 m_task_manager, this);

    auto table = m_file_panel->tableView();
    m_table_delegate = new FilePanelDelegate(table);
    table->setItemDelegate(m_table_delegate);

    m_preview_panel = new PreviewPanel();
    m_file_path_widget = new FilePathWidget();
    m_log_buffer = new MessagesBuffer();
    m_marks_buffer = new MarksBuffer();

    m_preview_panel_dock->setWidget(m_preview_panel);
    m_preview_panel->setContentsMargins(0, 0, 0, 0);

    m_marks_buffer->setMarksSet(m_file_panel->getMarksSetPTR());

    m_file_panel->setFocus();
    m_inputbar->hide();

    m_dock_container->setContentsMargins(0, 0, 0, 0);

    m_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_layout->addWidget(m_file_path_widget);
    m_layout->addWidget(m_dock_container);
    /*m_layout->addWidget(m_file_panel);*/
    m_layout->addWidget(m_inputbar);
    m_layout->addWidget(m_statusbar);
    m_widget->setLayout(m_layout);

    m_file_panel_dock->setContentsMargins(0, 0, 0, 0);
    m_file_panel_dock->setWidget(m_file_panel);
    m_preview_panel_dock->setSizePolicy(QSizePolicy::Maximum,
                                        QSizePolicy::Expanding);

    m_dock_container->addDockWidget(m_file_panel_dock,
                                    KDDockWidgets::Location::Location_OnLeft);

    const QSize s(QSize(400, 0));
    m_dock_container->addDockWidget(m_preview_panel_dock,
                                    KDDockWidgets::Location::Location_OnRight,
                                    nullptr, s);

    this->setPersistentCentralWidget(m_widget);
    this->show();
}

void Navi::initKeybinds() noexcept {

    Keybind kb_up = {"h", "up-directory", "Go to the parent directory"};
    Keybind kb_down = {"j", "next-item", "Go to the next item"};
    Keybind kb_prev = {"k", "prev-item", "Go to the previous item"};
    Keybind kb_select = {"l", "select-item", "Select item"};
    Keybind kb_filter = {"f", "filter", "Filter item visibility"};
    Keybind kb_first_item = {"g,g", "first-item", "Go to the first item"};
    Keybind kb_middle_item = {"z,z", "middle-item", "Go to middle item"};
    Keybind kb_last_item = {"Shift+g", "last-item", "Go to the last item"};
    Keybind kb_rename = {"Shift+r", "rename-dwim", "Rename item(s)"};
    Keybind kb_delete = {"Shift+d", "delete-dwim", "Delete item(s)"};
    Keybind kb_mark = {"Space", "toggle-mark-dwim", "Mark item(s)"};
    Keybind kb_refresh = {"F5", "refresh", "Refresh current directory"};
    Keybind kb_inverse_mark = {"Shift+Space", "mark-inverse",
        "Mark inverse item(s)"};
    Keybind kb_visual_mode = {"Shift+v", "visual-select",
        "Visual selection mode"};
    Keybind kb_extended_command = {":", "execute-extended-command",
        "Execute extended command"};
    Keybind kb_copy = {"y,y", "copy-dwim", "Copy item(s)"};
    Keybind kb_paste = {"p", "paste", "Paste item(s)"};
    Keybind kb_unmark_all = {"Shift+u", "unmark-local", "Unmark all item(s)"};
    Keybind kb_search_regex = {"/", "search", "Search (regex)"};
    Keybind kb_search_next = {"n", "search-next", "Search next"};
    Keybind kb_search_prev = {"Shift+n", "search-prev", "Search previous"};
    Keybind kb_toggle_menubar = {"Ctrl+m", "menu-bar", "Toggle menu bar"};
    Keybind kb_toggle_preview_pane = {"Ctrl+p", "preview-pane",
        "Toggle preview pane"};
    Keybind kb_focus_path_bar = {"Ctrl+l", "focus-path", "Focus path bar"};
    Keybind kb_trash = {"Shift+t", "trash-dwim", "Trash item(s)"};
    Keybind kb_toggle_hidden_files = {".", "hidden-files", "Toggle hidden items"};
    Keybind kb_record_macro = {"q", "macro-record",
        "Record or Finish recording macro"};
    Keybind kb_copy_path = {"Shift+c", "copy-path", "Copy path(s)"};
    Keybind kb_scroll_up_page = {"Ctrl+u", "scroll-up", "Scroll up a page"};
    Keybind kb_scroll_down_page = {"Ctrl+d", "scroll-down", "Scroll down a page"};

    m_keybind_list = {
        kb_up,
        kb_down,
        kb_prev,
        kb_select,
        kb_filter,
        kb_first_item,
        kb_middle_item,
        kb_last_item,
        kb_rename,
        kb_delete,
        kb_mark,
        kb_refresh,
        kb_inverse_mark,
        kb_visual_mode,
        kb_extended_command,
        kb_copy,
        kb_paste,
        kb_unmark_all,
        kb_search_regex,
        kb_search_next,
        kb_search_prev,
        kb_toggle_menubar,
        kb_toggle_preview_pane,
        kb_focus_path_bar,
        kb_trash,
        kb_toggle_hidden_files,
        kb_record_macro,
        kb_copy_path,
        kb_scroll_up_page,
        kb_scroll_down_page,
    };

    generateKeybinds();
}

void Navi::ExecuteExtendedCommand() noexcept {
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
    m_viewmenu__messages = new QAction("Messages");
    m_viewmenu__marks_buffer = new QAction("Marks List");
    m_viewmenu__bookmarks_buffer = new QAction("Bookmarks");
    m_viewmenu__shortcuts_widget = new QAction("Shortcuts");
    m_viewmenu__drives_widget = new QAction("Drives");
    m_viewmenu__tasks_widget = new QAction("Tasks");
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

    m_layoutmenu = new QMenu("Layout");

    m_layoutmenu__load_layout = new QAction("Load Layout");
    m_layoutmenu__save_layout = new QAction("Save Layout");
    m_layoutmenu__delete_layout = new QAction("Delete Layout");

    m_layoutmenu->addActions({m_layoutmenu__load_layout,
        m_layoutmenu__save_layout,
        m_layoutmenu__delete_layout});

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

    connect(m_bookmarks_menu__remove, &QAction::triggered, this,
            [&]() { RemoveBookmark(); });

    m_tools_menu = new QMenu("&Tools");

    m_tools_menu__search = new QAction("Search");
    m_tools_menu__find_files = new QAction("Find Files");
    m_tools_menu__screenshot = new QAction("Screenshot");

    m_tools_menu->addAction(m_tools_menu__find_files);
    m_tools_menu->addAction(m_tools_menu__search);
    m_tools_menu->addAction(m_tools_menu__screenshot);

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

    connect(m_tools_menu__screenshot, &QAction::triggered, this,
            [&]() { this->screenshot(); });

    connect(m_edit_menu__link_to, &QAction::triggered, this, &Navi::link_to);
    connect(m_edit_menu__open, &QAction::triggered, this, &Navi::SelectItem);
    connect(m_edit_menu__copy_path, &QAction::triggered, this,
            [&]() { CopyPath(); });
    connect(m_edit_menu__copy, &QAction::triggered, this, &Navi::CopyDWIM);
    connect(m_edit_menu__cut, &QAction::triggered, this, &Navi::CutDWIM);
    connect(m_edit_menu__paste, &QAction::triggered, this, &Navi::PasteItems);
    connect(m_edit_menu__rename, &QAction::triggered, this, &Navi::RenameDWIM);
    connect(m_edit_menu__trash, &QAction::triggered, this, &Navi::TrashDWIM);
    connect(m_edit_menu__delete, &QAction::triggered, this, &Navi::DeleteDWIM);
    connect(m_edit_menu__item_property, &QAction::triggered, this,
            &Navi::ShowItemPropertyWidget);
    connect(m_edit_menu__select_all, &QAction::triggered, this,
            &Navi::SelectAllItems);
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

    m_go_menu->addActions({m_go_menu__previous_folder, m_go_menu__next_folder,
        m_go_menu__parent_folder, m_go_menu__home_folder,
        m_go_menu__connect_to_server});

    connect(m_go_menu__previous_folder, &QAction::triggered, m_file_panel,
            &FilePanel::PreviousDirectory);
    connect(m_go_menu__home_folder, &QAction::triggered, m_file_panel,
            &FilePanel::HomeDirectory);
    connect(m_go_menu__parent_folder, &QAction::triggered, m_file_panel,
            &FilePanel::UpDirectory);

    m_help_menu = new QMenu("&Help");

    m_help_menu__about = new QAction("About");
    m_help_menu->addAction(m_help_menu__about);

    m_help_menu__check_for_updates = new QAction("Check for Updates");
    m_help_menu->addAction(m_help_menu__check_for_updates);

    connect(m_help_menu__check_for_updates, &QAction::triggered, this,
            &Navi::CheckForUpdates);

    connect(m_help_menu__about, &QAction::triggered, this,
            [&]() { ShowAbout(); });

    m_menubar->addMenu(m_filemenu);
    m_menubar->addMenu(m_edit_menu);
    m_menubar->addMenu(m_viewmenu);
    m_menubar->addMenu(m_layoutmenu);
    m_menubar->addMenu(m_bookmarks_menu);
    m_menubar->addMenu(m_go_menu);
    m_menubar->addMenu(m_tools_menu);
    m_menubar->addMenu(m_help_menu);

    connect(m_layoutmenu__save_layout, &QAction::triggered, this,
            [&]() { saveLayout(); });

    connect(m_layoutmenu__load_layout, &QAction::triggered, this,
            [&]() { loadLayout(); });

    connect(m_layoutmenu__delete_layout, &QAction::triggered, this,
            [&]() { deleteLayout(); });

    connect(m_viewmenu__refresh, &QAction::triggered, this,
            [&](const bool &state) { ForceUpdate(); });

    connect(m_viewmenu__fullscreen, &QAction::triggered, this,
            [&](const bool &state) { FullScreen(state); });

    connect(m_viewmenu__filter, &QAction::triggered, this,
            [&](const bool &state) {
            if (state) {
            Filter();
            } else {
            ResetFilter();
            }
            });

    connect(m_viewmenu__tasks_widget, &QAction::triggered, this,
            [&](const bool &state) { ToggleTasksWidget(state); });

    connect(m_viewmenu__headers, &QAction::triggered, this,
            [&](const bool &state) { m_file_panel->ToggleHeaders(state); });

    connect(m_viewmenu__shortcuts_widget, &QAction::triggered,
            [&]() { ShortcutsBuffer(); });

    connect(m_viewmenu__sort_ascending, &QAction::triggered, this,
            &Navi::SortAscending);

    connect(m_viewmenu__sort_descending, &QAction::triggered, this,
            &Navi::SortDescending);

    connect(m_viewmenu__sort_by_name, &QAction::triggered, this,
            &Navi::SortByName);

    connect(m_viewmenu__sort_by_date, &QAction::triggered, this,
            &Navi::SortByDate);
    connect(m_viewmenu__sort_by_size, &QAction::triggered, this,
            &Navi::SortBySize);

    connect(m_viewmenu__marks_buffer, &QAction::triggered, this,
            [&]() { ToggleMarksBuffer(); });

    connect(m_viewmenu__bookmarks_buffer, &QAction::triggered, this,
            [&]() { ToggleBookmarksBuffer(); });

    connect(m_viewmenu__files_menu__dotdot, &QAction::triggered, this,
            [&](const bool &state) { ToggleDotDot(state); });

    connect(m_viewmenu__preview_panel, &QAction::triggered, this,
            [&]() { TogglePreviewPanel(); });

    connect(m_viewmenu__messages, &QAction::triggered, this,
            [&]() { ToggleMessagesBuffer(); });

    connect(m_filemenu__new_window, &QAction::triggered, this,
            &Navi::LaunchNewInstance);

    connect(m_filemenu__create_new_file, &QAction::triggered, this,
            [&]() { m_file_panel->NewFile(); });

    connect(m_filemenu__create_new_folder, &QAction::triggered, this,
            [&]() { m_file_panel->NewFolder(); });

    connect(m_filemenu__close_window, &QAction::triggered, this, &Navi::Exit);

    connect(m_filemenu__folder_properties, &QAction::triggered, this,
            &Navi::ShowFolderProperty);

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
}

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
        bool ok;
        QString filterString =
            utils::getInput(this, "Filter", "Enter filter string", ok);

        if (!ok) {
            return;
        }

        qDebug() << "DD";

        if (filterString.isEmpty() || filterString.isNull() ||
            filterString == "*") {
            ResetFilter();
            m_viewmenu__filter->setChecked(false);
            return;
        } else {
            m_file_panel->Filters(filterString);
        }
    }
    m_statusbar->SetFilterMode(true);
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
    if (!m_bookmark_manager)
        return;

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
            bookmarks_str +=
                QString("\t%1 = { path = \"%2\",\n\thighlight_only = true,\n\t},\n")
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
    m_file_panel->ToggleHiddenFiles(state);
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
        Set_default_directory(QString::fromStdString(
            parser.get<std::vector<std::string>>("files").at(0)));
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
    if (!(term.isEmpty() || term.isNull())) {
        m_file_panel->Search(term);
        m_search_history_list.append(term);
    } else {
        QString searchText = m_inputbar->getInput("Search");
        m_file_panel->Search(searchText);
        m_search_history_list.append(searchText);
    }
}

void Navi::Search(const std::string &_term) noexcept {
    QString term = QString::fromStdString(_term);
    if (!(term.isNull() || term.isEmpty())) {
        m_file_panel->Search(term);
        m_search_history_list.append(term);
    } else {
        QString searchText = m_inputbar->getInput("Search");
        m_file_panel->Search(searchText);
        m_search_history_list.append(searchText);
    }
}

void Navi::SearchRegex() noexcept {
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
    return m_inputbar
    ->getInput(QString::fromStdString(prompt),
               QString::fromStdString(def_value),
               QString::fromStdString(selection))
    .toStdString();
}

/*void Navi::ExecuteLuaFunction(const QStringList &args) {*/
/*    if (args.isEmpty()) {*/
/*        QString funcName = m_inputbar->getInput("Enter the lua function
 * name");*/
/*        try {*/
/*            lua[funcName.toStdString().c_str()]();*/
/*        } catch (const sol::error &e) {*/
/*            m_statusbar->Message(QString("Lua: %1").arg(e.what()),*/
/*                                 MessageType::ERROR);*/
/*        }*/
/*    } else {*/
/*        std::string luaFunc = args.at(0).toStdString();*/
/*        std::vector<std::string> _args =
 * utils::convertToStdVector(args.mid(1));*/
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

QString Navi::GetInput(const QString &prompt, const QString &title,
                       const QString &default_text) noexcept {
    bool ok;
    QString result = QInputDialog::getText(this, title, prompt, QLineEdit::Normal,
                                           default_text, &ok);
    if (ok) {
        return result;
    }

    return QString();
}

QString Navi::GetInput(const QString &prompt, const QString &title,
                       const QStringList &choice,
                       const int &default_choice) noexcept {

    bool ok;
    QString result = QInputDialog::getItem(this, title, prompt, choice,
                                           default_choice, false, &ok);

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
        if (!path.isEmpty() && path != m_file_panel->getCurrentDir())
            m_file_panel->setCurrentDir(path);
    } else {
        m_file_panel->setCurrentDir(path);
    }
}

void Navi::change_directory(const std::string &path) noexcept {
    ChangeDirectory(QString::fromStdString(path));
}

void Navi::SpawnProcess(const QString &command,
                        const QStringList &args) noexcept {}

void Navi::cacheThumbnails() noexcept {

    QStringList files = m_file_panel->model()->files();
    m_thumbnailer->generate_thumbnails(files);
}

void Navi::LaunchNewInstance() noexcept {
    // Get the path to the current application executable
    QString appPath = QCoreApplication::applicationFilePath();

    // Launch a new instance
    QProcess::startDetached(appPath);
}

void Navi::ShowFolderProperty() noexcept {
    FolderPropertyWidget *f =
        new FolderPropertyWidget(m_file_panel->getCurrentDir(), this);
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

void Navi::SelectAllItems() noexcept { m_file_panel->tableView()->selectAll(); }

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
        for (const auto &kvp : submenuTable) {
            if (kvp.second.is<sol::table>()) {
                item.submenu.push_back(Lua__parseMenuItem(kvp.second.as<sol::table>()));
            }
        }
    }

    return item;
}

void Navi::Lua__AddMenu(const sol::table &menuTable) noexcept {
    Navi::MenuItem menu = Lua__parseMenuItem(menuTable);
    QMenu *qmenu = new QMenu(QString::fromStdString(menu.label));

    if (!menu.submenu.empty()) {
        for (const auto &subitem : menu.submenu) {
            // If no submenu, add as an action
            if (subitem.submenu.empty()) {
                QAction *action = new QAction(QString::fromStdString(subitem.label));
                qmenu->addAction(action);
                connect(action, &QAction::triggered, this,
                        [subitem]() { subitem.action(); });
            } else { // else, add it as a menu
                QMenu *submenu = new QMenu(QString::fromStdString(subitem.label));
                qmenu->addMenu(submenu);
                for (const auto &subitem : subitem.submenu) {
                    QAction *action = new QAction(QString::fromStdString(subitem.label));
                    submenu->addAction(action);
                    connect(action, &QAction::triggered, this,
                            [subitem]() { subitem.action(); });
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

Navi::ToolbarItem
Navi::Lua__parseToolbarItem(const sol::table &table) noexcept {
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

    QPushButton *widget =
        new QPushButton(QString::fromStdString(toolbar_item.label));
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
        QAction *before = m_toolbar->actions().at(toolbar_item.position - 1);
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

    QPushButton *widget =
        new QPushButton(QString::fromStdString(toolbar_item.label));
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
        QAction *before = m_toolbar->actions().at(toolbar_item.position - 1);
        m_toolbar->insertWidget(before, widget);
    } else {
        m_toolbar->addWidget(widget);
    }
}

void Navi::initToolbar() noexcept {

    auto style = this->style();
    m_toolbar__home_btn =
        new QPushButton(QIcon::fromTheme(QIcon::ThemeIcon::GoHome), "");
    m_toolbar__next_btn =
        new QPushButton(style->standardIcon(QStyle::SP_ArrowForward), "");
    m_toolbar__prev_btn =
        new QPushButton(style->standardIcon(QStyle::SP_ArrowBack), "");
    m_toolbar__parent_btn =
        new QPushButton(QIcon::fromTheme(QIcon::ThemeIcon::GoUp), "");
    m_toolbar__refresh_btn =
        new QPushButton(QIcon::fromTheme(QIcon::ThemeIcon::ViewRefresh), "");

    m_toolbar__home_btn->setToolTip("Go to Home folder");
    m_toolbar__next_btn->setToolTip("Go to Next folder");
    m_toolbar__prev_btn->setToolTip("Go to Previous folder");
    m_toolbar__parent_btn->setToolTip("Go to Parent folder");
    m_toolbar__refresh_btn->setToolTip("Refresh folder");

    connect(m_toolbar__prev_btn, &QPushButton::clicked, m_file_panel,
            &FilePanel::PreviousDirectory);

    connect(m_toolbar__next_btn, &QPushButton::clicked, this,
            [&]() { qDebug() << "TODO: Next Location"; });

    connect(m_toolbar__home_btn, &QPushButton::clicked, m_file_panel,
            &FilePanel::HomeDirectory);
    connect(m_toolbar__parent_btn, &QPushButton::clicked, m_file_panel,
            &FilePanel::UpDirectory);

    connect(m_toolbar__refresh_btn, &QPushButton::clicked, m_file_panel,
            &FilePanel::ForceUpdate);
}

void Navi::ShowAbout() noexcept { AboutWidget *about = new AboutWidget(this); }

void Navi::SortAscending(const bool &state) noexcept {
    if (state) {
        if (m_sort_flags & QDir::SortFlag::Reversed)
            return;
        QString itemName = m_file_panel->getCurrentItem();
        m_sort_flags |= QDir::SortFlag::Reversed;
        m_file_panel->model()->setSortBy(m_sort_flags);
        m_file_panel->HighlightItem(itemName);
    }
}

void Navi::SortDescending(const bool &state) noexcept {
    if (state) {
        if (m_sort_flags & QDir::SortFlag::Reversed) {
            QString itemName = m_file_panel->getCurrentItem();
            m_sort_flags &= ~QDir::SortFlag::Reversed;
            m_file_panel->model()->setSortBy(m_sort_flags);
            m_file_panel->HighlightItem(itemName);
        }
    }
}

void Navi::Lua__SetToolbarItems(const sol::table &table) noexcept {

    m_toolbar->clear();

    for (size_t i = 1; i <= table.size(); i++) {
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

Navi::ToolbarItem
Navi::Lua__CreateToolbarButton(const std::string &name,
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

    for (int i = 0; i < rtps.size(); i++)
        table[i + 1] = rtps.at(i).toStdString();

    return table;
}

void Navi::update_lua_package_path(sol::state &lua) noexcept {
    QStringList visitedDirs; // To track directories already added to package.path

    lua.script(R"(
               package = package or {}
               package.path = package.path or ""
               )");

    // Create a QDirIterator to iterate through all subdirectories and files
    QDirIterator it(CONFIG_DIR_PATH, QStringList() << "*.lua", QDir::Files,
                    QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QString filePath = it.next();
        QString parentDir =
            QFileInfo(filePath).absolutePath(); // Get the parent directory

        // Add to package.path only if this directory hasn't been added yet
        if (!visitedDirs.contains(parentDir)) {
            m_runtime_path.insert(parentDir);
            lua.script("package.path = package.path .. ';" + parentDir.toStdString() +
                       "/?.lua'");
            visitedDirs.append(parentDir); // Mark the directory as visited
        }
    }
}

void Navi::Lua__keymap_set_from_table(const sol::table &table) noexcept {

    if (!table["key"]) {
        m_statusbar->Message("No key found in the keybinding table",
                             MessageType::WARNING);
        return;
    }

    if (!table["command"]) {
        m_statusbar->Message("No command found in the keybinding table",
                             MessageType::WARNING);
        return;
    }

    Keybind kb{
        .key = QString::fromStdString(table["key"].get_or<std::string>("")),
        .command =
        QString::fromStdString(table["command"].get_or<std::string>("")),
        .desc = QString::fromStdString(table["desc"].get_or<std::string>(""))};
    m_keybind_list.append(kb);

    QShortcut *shortcut = new QShortcut(QKeySequence(kb.key), this);
    connect(shortcut, &QShortcut::activated, this,
            [&, kb]() { ProcessCommand(kb.command); });
}

void Navi::Lua__keymap_set(const std::string &key, const std::string &command,
                           const std::string &desc) noexcept {

    Keybind kb{.key = QString::fromStdString(key),
        .command = QString::fromStdString(command),
        .desc = QString::fromStdString(desc)};

    m_keybind_list.append(kb);

    QShortcut *shortcut = new QShortcut(QKeySequence(kb.key), this);
    connect(shortcut, &QShortcut::activated, this,
            [&, kb]() { ProcessCommand(kb.command); });
}

void Navi::Lua__keymap_set_for_function(const std::string &key,
                                        const sol::function &func,
                                        const std::string &desc) noexcept {

    Keybind kb{.key = QString::fromStdString(key),
        .command = "lua function",
        .desc = QString::fromStdString(desc)};

    m_keybind_list.append(kb);

    QShortcut *shortcut = new QShortcut(QKeySequence(kb.key), this);
    connect(shortcut, &QShortcut::activated, this, [=]() { func(); });
}

void Navi::Lua__register_user_function(const std::string &name,
                                       const sol::function &func) noexcept {
    commandMap[QString::fromStdString(name)] = [func](const QStringList &args) {
        if (!args.isEmpty()) {
            auto lua_args = utils::convertToStdVector(args);
            func(sol::as_args(lua_args));
        } else
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

void Navi::init_default_options() noexcept { /*set_menubar_icons(true);*/ }

void Navi::Set_auto_save_bookmarks(const bool &state) noexcept {
    m_auto_save_bookmarks = state;
}

void Navi::Set_toolbar_icons_only() noexcept {
    m_toolbar_icons_only = true;
    auto style = this->style();
    m_toolbar__prev_btn =
        new QPushButton(style->standardIcon(QStyle::SP_ArrowBack), "");
    m_toolbar__next_btn =
        new QPushButton(style->standardIcon(QStyle::SP_ArrowForward), "");
    m_toolbar__home_btn =
        new QPushButton(QIcon::fromTheme(QIcon::ThemeIcon::GoHome), "");
    m_toolbar__parent_btn =
        new QPushButton(QIcon::fromTheme(QIcon::ThemeIcon::GoUp), "");
    m_toolbar__refresh_btn =
        new QPushButton(QIcon::fromTheme(QIcon::ThemeIcon::ViewRefresh), "");
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

    for (size_t i = 1; i <= layout.size(); i++) {
        auto tmp = layout[i].get_type();

        switch (tmp) {

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
            } break;

            case sol::type::userdata: {
                auto item = layout[i].get<ToolbarItem>();
                Lua__AddToolbarButton(item);
            } break;

            default:
                qWarning() << "Not handled toolbar type";
        }
    }
}

void Navi::set_menubar_icons(const bool &state) noexcept {
    m_menubar_icons = state;
    if (state) {
        m_filemenu__new_window->setIcon(
            QIcon::fromTheme(QIcon::ThemeIcon::WindowNew));
        m_filemenu__create_new_folder->setIcon(
            QIcon::fromTheme(QIcon::ThemeIcon::FolderNew));
        m_filemenu__create_new_file->setIcon(
            QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew));
        m_filemenu__close_window->setIcon(
            QIcon::fromTheme(QIcon::ThemeIcon::WindowClose));
        m_filemenu__folder_properties->setIcon(
            QIcon::fromTheme(QIcon::ThemeIcon::DocumentProperties));
        m_viewmenu__refresh->setIcon(
            QIcon::fromTheme(QIcon::ThemeIcon::ViewRefresh));
        m_viewmenu__fullscreen->setIcon(
            QIcon::fromTheme(QIcon::ThemeIcon::ViewFullscreen));
        m_tools_menu__search->setIcon(
            QIcon::fromTheme(QIcon::ThemeIcon::SystemSearch));
        m_tools_menu__find_files->setIcon(
            QIcon::fromTheme(QIcon::ThemeIcon::EditFind));
        m_edit_menu__open->setIcon(
            QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen));
        m_edit_menu__copy->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditCopy));
        m_edit_menu__paste->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditPaste));
        m_edit_menu__delete->setIcon(
            QIcon::fromTheme(QIcon::ThemeIcon::EditDelete));
        m_edit_menu__item_property->setIcon(
            QIcon::fromTheme(QIcon::ThemeIcon::DocumentProperties));
        m_edit_menu__cut->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditCut));
        m_edit_menu__select_all->setIcon(
            QIcon::fromTheme(QIcon::ThemeIcon::EditSelectAll));
        m_viewmenu__sort_ascending->setIcon(
            QIcon::fromTheme("view-sort-ascending"));
        m_viewmenu__sort_descending->setIcon(
            QIcon::fromTheme("view-sort-descending"));
        m_edit_menu__trash->setIcon(QIcon::fromTheme("user-trash"));
        m_bookmarks_menu__bookmarks_list_menu->setIcon(
            QIcon::fromTheme("user-bookmarks"));
        m_bookmarks_menu__add->setIcon(
            QIcon::fromTheme(QIcon::ThemeIcon::AddressBookNew));
        m_bookmarks_menu__remove->setIcon(
            QIcon::fromTheme(QIcon::ThemeIcon::ListRemove));
        m_go_menu__connect_to_server->setIcon(QIcon::fromTheme("network-server"));
        m_go_menu__home_folder->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::GoHome));
        m_go_menu__parent_folder->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::GoUp));
        m_go_menu__previous_folder->setIcon(
            QIcon::fromTheme(QIcon::ThemeIcon::GoPrevious));
        m_go_menu__next_folder->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::GoNext));
        m_viewmenu__filter->setIcon(QIcon::fromTheme("view-filter"));
        m_edit_menu__rename->setIcon(QIcon(":resources/images/pencil-icon.svg"));
        m_edit_menu__copy_path->setIcon(QIcon(":resources/images/location.svg"));
        m_filemenu__create_new_menu->setIcon(QIcon(":resources/images/plus.svg"));
        m_edit_menu__select_inverse->setIcon(
            QIcon(":resources/images/reverse.svg"));
        m_viewmenu__sort_menu->setIcon(QIcon(":resources/images/sort.svg"));
        m_viewmenu__messages->setIcon(QIcon(":resources/images/messages.svg"));
        m_viewmenu__messages->setIcon(QIcon(":resources/images/messages.svg"));
        m_viewmenu__tasks_widget->setIcon(QIcon(":resources/images/clock.svg"));
        m_viewmenu__menubar->setIcon(QIcon(":resources/images/menu.svg"));
        m_viewmenu__statusbar->setIcon(QIcon(":resources/images/statusbar.svg"));
        m_viewmenu__preview_panel->setIcon(QIcon(":resources/images/preview.svg"));
        m_help_menu__about->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout));
        m_help_menu__check_for_updates->setIcon(
            QIcon(":resources/images/update.svg"));
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
        m_help_menu__check_for_updates->setIconVisibleInMenu(false);
    }
}

void Navi::Set_preview_pane_fraction(const double &fraction) noexcept {
    auto totalSize = m_dock_container->width();
    m_preview_pane_fraction = fraction;
    auto width = static_cast<int>(totalSize * fraction);
    m_preview_panel_dock->setSize(QSize(width, 0));
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
    QList<FileSystemModel::Column>
    columnList; // List to store column configuration
    columnList.reserve(columns.size());
    FileSystemModel::Column column;
    bool file_name_type_check = false;
    for (std::size_t i = 1; i <= columns.size(); i++) {
        auto col = columns[i];
        auto name = QString::fromStdString(col["name"].get_or<std::string>(""));
        auto type = QString::fromStdString(col["type"].get_or<std::string>(""));

        if (type.isEmpty() || type.isNull()) {
            m_statusbar->Message("*type* key is mandatory for columns."
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
        m_statusbar->Message("*file_name* key is mandatory in the columns table."
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
    if (table["visible"].valid())
        m_file_path_widget->setVisible(table["visible"].get<bool>());

    if (table["background"].valid())
        m_file_path_widget->set_background(table["background"].get<std::string>());

    if (table["foreground"].valid())
        m_file_path_widget->set_foreground(table["foreground"].get<std::string>());

    if (table["font"].valid())
        m_file_path_widget->set_font_family(
            QString::fromStdString(table["font"].get<std::string>()));

    if (table["font_size"].valid())
        m_file_path_widget->set_font_size(table["font_size"].get<int>());

    if (table["bold"].valid())
        m_file_path_widget->set_bold(table["bold"].get<bool>());

    if (table["italic"].valid())
        m_file_path_widget->set_italic(table["italic"].get<bool>());

    if (table["underline"].valid())
        m_file_path_widget->set_underline(table["underline"].get<bool>());
}

sol::table Navi::get_pathbar_props() noexcept {

    sol::table table = m_lua->create_table();

    table["visible"] = m_file_path_widget->isVisible();
    table["background"] = m_file_path_widget->background();
    table["foreground"] = m_file_path_widget->foreground();
    table["font"] = m_file_path_widget->font_family();
    table["font_size"] = m_file_path_widget->font_size();
    table["italic"] = m_file_path_widget->italic();
    table["bold"] = m_file_path_widget->bold();
    table["underline"] = m_file_path_widget->underline();

    return table;
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
    if (table["font"].valid())
        set_symlink_font(table["font"].get<std::string>());

    if (table["font_size"].valid())
        set_symlink_font_size(table["font_size"].get<int>());

    if (table["foreground"].valid())
        set_symlink_foreground(table["foreground"].get<std::string>());

    if (table["background"].valid())
        set_symlink_background(table["background"].get<std::string>());

    if (table["visible"].valid())
        set_symlink_visible(table["visible"].get<bool>());

    if (table["bold"].valid())
        set_symlink_bold(table["bold"].get<bool>());

    if (table["italic"].valid())
        set_symlink_italic(table["italic"].get<bool>());

    if (table["underline"].valid())
        set_symlink_underline(table["underline"].get<bool>());

    if (table["separator"].valid())
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

void Navi::set_cursor_props(const sol::table &table) noexcept {

    if (table["font"].valid())
        set_cursor_font(table["font"].get<std::string>());

    if (table["foreground"].valid())
        set_cursor_foreground(table["foreground"].get<std::string>());

    if (table["background"].valid())
        set_cursor_background(table["background"].get<std::string>());

    if (table["bold"].valid())
        set_cursor_bold(table["bold"].get<bool>());

    if (table["italic"].valid())
        set_cursor_italic(table["italic"].get<bool>());

    if (table["underline"].valid())
        set_cursor_underline(table["underline"].get<bool>());
}

sol::table Navi::get_cursor_props() noexcept {
    sol::table table = m_lua->create_table();
    table["underline"] = get_cursor_underline();
    table["italic"] = get_cursor_italic();
    table["font"] = get_cursor_font();
    table["bold"] = get_cursor_bold();
    table["foreground"] = get_cursor_foreground();
    table["background"] = get_cursor_background();
    return table;
}

void Navi::CheckForUpdates() noexcept {
    UpdateDialog *dialog = new UpdateDialog(m_version, this);
    dialog->exec();
}

void Navi::set_menubar_props(const sol::table &table) noexcept {
    if (table["icons"])
        set_menubar_icons(table["icons"].get<bool>());

    if (table["visible"])
        m_menubar->setVisible(table["visible"].get<bool>());
}

sol::table Navi::get_menubar_props() noexcept {
    sol::table table = m_lua->create_table();
    table["icons"] = m_menubar_icons;
    table["visible"] = m_menubar->isVisible();
    return table;
}

void Navi::set_file_panel_props(const sol::table &table) noexcept {
    if (table["font"].valid())
        m_file_panel->set_font_family(
            QString::fromStdString(table["font"].get<std::string>()));

    if (table["font_size"].valid())
        m_file_panel->set_font_size(table["font_size"].get<int>());

    if (table["icons"].valid())
        m_file_panel->model()->icons_enabled = table["icons"].get<bool>();

    if (table["grid"].valid()) {
        set_file_panel_grid(table["grid"].get<bool>());
    }

    if (table["gridstyle"].valid())
        set_file_panel_grid_style(table["gridstyle"].get<std::string>());
}

sol::table Navi::get_file_panel_props() noexcept {

    sol::table table = m_lua->create_table();
    table["font"] = m_file_panel->get_font_family().toStdString();
    table["font_size"] = m_file_panel->get_font_size();
    table["icons"] = m_file_panel->model()->icons_enabled;
    table["grid"] = get_file_panel_grid();
    table["gridstyle"] = get_file_panel_grid_style();

    return table;
}

void Navi::set_vheader_props(const sol::table &table) noexcept {
    auto vheader = m_file_panel->tableView()->vheader();

    if (table["visible"].valid())
        vheader->setVisible(table["visible"].get<bool>());

    if (table["border"].valid())
        vheader->set_border(table["border"].get<bool>());

    if (table["foreground"].valid())
        vheader->set_foreground(
            QString::fromStdString(table["foreground"].get<std::string>()));

    if (table["background"].valid())
        vheader->set_background(
            QString::fromStdString(table["background"].get<std::string>()));
}

sol::table Navi::get_vheader_props() noexcept {
    sol::table table = m_lua->create_table();

    auto vheader = m_file_panel->tableView()->vheader();

    table["visible"] = vheader->isVisible();
    table["border"] = vheader->hasBorder();
    table["foreground"] = vheader->get_foreground();
    table["background"] = vheader->get_background();

    return table;
}

void Navi::loadLayout(const std::string &layoutName) noexcept {
    QStringList savedLayouts = utils::savedLayouts();

    if (savedLayouts.empty()) {
        QMessageBox::information(this, "Load layout", "No saved layouts detected. Please save a layout first to load them");
        return;
    }

    qDebug() << savedLayouts;

    auto loadLayoutFunc = [&](const QString &layout_name) {
        auto fileName = joinPaths(LAYOUT_PATH, layout_name);
        KDDockWidgets::LayoutSaver loader;
        loader.restoreFromFile(fileName);
    };

    if (layoutName.empty()) {
        bool ok;
        QString selectedLayout = QInputDialog::getItem(this, "Load a layout", "Choose a layout", savedLayouts, 0, false, &ok);

        if (ok && !selectedLayout.isEmpty()) {
            loadLayoutFunc(selectedLayout);
        }
    } else {
        loadLayoutFunc(QString::fromStdString(layoutName));
    }
}

void Navi::deleteLayout(const std::string &layoutName) noexcept {}

void Navi::saveLayout(const std::string &_layoutName) noexcept {

    KDDockWidgets::LayoutSaver saver;

    QStringList savedLayouts = utils::savedLayouts();
    QString layoutName;

    if (_layoutName.empty()) {
        bool fine = false;
        bool ok;

        while (!fine) {

            layoutName = QInputDialog::getText(this, "Save Layout", "Enter layout name",
                                               QLineEdit::Normal, "", &ok) +
                ".json";

            if (!ok || layoutName.isEmpty())
                return;

            if (savedLayouts.contains(layoutName)) {
                QMessageBox::warning(
                    this, "Layout Name Exists",
                    QString("Layout with the name '%1' already exists. Please try saving "
                            "the layout with a unique name")
                    .arg(layoutName));
            } else {
                fine = true;
            }
        }

    } else {
        layoutName = QString::fromStdString(_layoutName) + ".json";
    }

    QDir dir;
    if (!dir.exists(LAYOUT_PATH)) {
        dir.mkdir(LAYOUT_PATH);
    }

    QString path = joinPaths(LAYOUT_PATH, layoutName);

    if (saver.saveToFile(path)) {
        m_statusbar->Message("Successfully saved layout");
    } else {
        m_statusbar->Message("Error saving the layout", MessageType::ERROR);
    }
}

void Navi::screenshot(const std::string &_path, const int &delay) noexcept {
    QScreen *screen = QGuiApplication::primaryScreen();

    if (!screen) {
        qCritical() << "Unable to find the primary screen";
        return;
    }

    QMainWindow *win = qobject_cast<QMainWindow *>(this);

    auto geom = win->geometry();

    auto takeScreenshot = [&]() {
        QPixmap pix =
            screen->grabWindow(0, geom.x(), geom.y(), geom.width(), geom.height());

        if (pix.isNull())
            return;

        if (_path.empty()) {
            auto dir = m_file_panel->getCurrentDir();
            auto filename = QUuid::createUuid().toString() + ".png";
            pix.save(joinPaths(dir, filename), "PNG");
        } else {
            pix.save(QString::fromStdString(_path) + ".png", "PNG");
        }

        m_statusbar->Message("Screenshot taken!");
    };

    if (delay > 0) {
        QTimer::singleShot(delay, takeScreenshot);
    } else {
        takeScreenshot();
    }
}


std::vector<std::string> Navi::listLayout() noexcept {
    auto savedLayouts = utils::savedLayouts();
    if (savedLayouts.empty())
        return {};

    return utils::convertToStdVector(savedLayouts);
}

void Navi::initValidCommandsList() noexcept {
    m_valid_command_list = {
        // Shell command
        "shell",

        // Mark
        "mark",
        "toggle-mark",
        "toggle-mark-dwim",
        "mark-inverse",
        "mark-all",
        "mark-dwim",
        "mark-regex",

        // Unmark
        "unmark",
        "unmark-global",
        "unmark-local",
        "unmark-dwim",
        "unmark-regex",

        // Chmod
        "chmod",
        "chmod-global",
        "chmod-local",
        "chmod-dwim",

        // Rename
        "rename",
        "rename-global",
        "rename-local",
        "rename-dwim",

        // Cut
        "cut",
        "move-to",
        "cut-local",
        "cut-global",
        "cut-dwim",

        // Copy
        "copy",
        "copy-to",
        "copy-local",
        "copy-global",
        "copy-dwim",

        // Paste
        "paste",

        // Delete
        "delete",
        "delete-local",
        "delete-global",
        "delete-dwim",

        // Trash
        "trash",
        "trash-local",
        "trash-global",
        "trash-dwim",

        // Task
        "tasks",

        // Bookmarks
        "bookmark-add",
        "bookmark-remove",
        "bookmark-edit-name",
        "bookmark-edit-path",
        "bookmark-go",
        "bookmarks-save",

        // Creation
        "new-file",
        "new-folder",

        // Panes
        "messages-pane",
        "preview-pane",
        "marks-pane",
        "bookmarks-pane",
        "shortcuts-pane",

        // Search
        "search",
        "search-regex",
        "search-next",
        "search-prev",

        // Sort
        "sort-name",
        "sort-name-desc",
        "sort-date",
        "sort-date-desc",
        "sort-size",
        "sort-size-desc",

        // Navigation
        "next-item",
        "prev-item",
        "first-item",
        "last-item",
        "middle-item",
        "up-directory",
        "select-item",

        "macro-play",
        "macro-record",
        "macro-delete",
        "macro-list",
        "macro-edit",
        "macro-save-to-file",

        "scroll-down",
        "scroll-up",

        // Echo
        "echo-info",
        "echo-warn",
        "echo-error",

        // misc
        "notify",
        "screenshot",
        "update",
        "new-window",
        "goto-symlink-target",
        "exit",
        "filter",
        "reset-filter",
        "refresh",
        "hidden-files",
        "dot-dot",
        "menu-bar",
        "focus-path",
        "item-property",
        "cycle",
        "header",
        "reload-config",
        "execute-extended-command",
        "visual-select",
        "mouse-scroll",
        "drives",
        // "syntax-highlight",
        "lua",
        "register",
        "repeat-last-command",
        "cd",
        "terminal",
        "folder-property",
        "copy-path",
        "fullscreen",
        "about",
    };
}

void Navi::initCompletion() noexcept {

  m_commands = {
      {"com1",
       {0,
        2,
        {QStringList{"arg1_value1", "arg1_value2"},
         QStringList{"arg2_value1", "arg2_value2"}}}},
      {"com2", {0, 0, {}}}, // No arguments
  };

  auto lineEdit = m_inputbar->lineEdit();
  auto compPopup = m_inputbar->completionPopup();

  m_inputbar->enableCompletion();
  compPopup->setInitialCompletions({"com1", "com2"});

  connect(lineEdit, &LineEdit::spacePressed, this, [&, lineEdit, compPopup]() {
    auto text = lineEdit->text();
    auto split = text.split(' ', Qt::SkipEmptyParts);
    QString command = split.first();
    int index = split.size() - 1;
    QStringList completions = getCompletionsForCommand(command, index);
    qDebug() << completions;
    compPopup->setCompletions(completions);
  });
}


// Function to get possible completions for a command's argument
QStringList Navi::getCompletionsForCommand(const QString &command,
                                           const int &argIndex) noexcept {
    if (!m_commands.contains(command) ||
        argIndex >= m_commands[command].argCompletions.size())
        return {};  // Return empty if command or argument index is invalid

    const CompletionType& completionSource = m_commands[command].argCompletions[argIndex];

    if (std::holds_alternative<QStringList>(completionSource)) {
        return std::get<QStringList>(completionSource);
    } else {
        return std::get<std::function<QStringList()>>(completionSource)();
    }
}
