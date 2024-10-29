#include "Navi.hpp"
#include <qnamespace.h>

Navi::Navi(QWidget *parent) : QMainWindow(parent) {

  initLayout();       // init layout
  initMenubar();      // init menubar
  initSignalsSlots(); // init signals and slots
  initKeybinds();
  setupCommandMap();
  initBookmarks();
  setCurrentDir("~"); // set the current directory
}

void Navi::initBookmarks() noexcept {
  // TODO: load bookmarks from config directory
  m_bookmark_manager = new BookmarkManager();
}

// Handle signals and slots
void Navi::initSignalsSlots() noexcept {

  connect(m_file_panel, &FilePanel::currentItemChanged, m_preview_panel,
          &PreviewPanel::onFileSelected);

  connect(m_file_panel, &FilePanel::currentItemChanged, m_statusbar,
          &Statusbar::SetFile);

  connect(m_file_panel, &FilePanel::afterDirChange, m_file_path_widget,
          &FilePathWidget::setCurrentDir);

  connect(m_file_path_widget, &FilePathWidget::directoryChangeRequested,
          m_file_panel, [&](const QString &dirName) {
            m_file_panel->setCurrentDir(dirName, true);
          });

  connect(m_file_panel, &FilePanel::dirItemCount, m_statusbar,
          &Statusbar::SetNumItems);

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

  commandMap["rename"] = [this](const QStringList &args) {
      m_file_panel->RenameItem();
  };
  commandMap["rename-global"] = [this](const QStringList &args) {
      m_file_panel->RenameItemsGlobal();
  };
  commandMap["rename-local"] = [this](const QStringList &args) {
      m_file_panel->RenameItemsLocal();
  };

  commandMap["help"] = [this](const QStringList &args) { ShowHelp(); };

  commandMap["copy"] = [this](const QStringList &args) {
    CopyItems(CommandScope::CURRENT);
  };
  commandMap["copy-global"] = [this](const QStringList &args) {
    CopyItems(CommandScope::GLOBAL);
  };
  commandMap["copy-local"] = [this](const QStringList &args) {
    CopyItems(CommandScope::LOCAL);
  };

  commandMap["cut"] = [this](const QStringList &args) {
    CutItems(CommandScope::CURRENT);
  };
  commandMap["cut-global"] = [this](const QStringList &args) {
    CutItems(CommandScope::GLOBAL);
  };
  commandMap["cut-local"] = [this](const QStringList &args) {
    CutItems(CommandScope::LOCAL);
  };

  commandMap["paste"] = [this](const QStringList &args) { PasteItems(); };

  commandMap["delete"] = [this](const QStringList &args) {
    m_file_panel->DeleteItem();
  };

  commandMap["delete-global"] = [this](const QStringList &args) {
    m_file_panel->DeleteItemsGlobal();
  };

  commandMap["delete-local"] = [this](const QStringList &args) {
    m_file_panel->DeleteItemsLocal();
  };

  commandMap["mark"] = [this](const QStringList &args) {
    m_file_panel->MarkItem();
  };

  commandMap["mark-inverse"] = [this](const QStringList &args) {
    m_file_panel->MarkInverse();
  };

  commandMap["mark-all"] = [this](const QStringList &args) {
    m_file_panel->MarkAllItems();
  };

  commandMap["toggle-mark"] = [this](const QStringList &args) {
    m_file_panel->ToggleMarkItem();
  };

  commandMap["unmark"] = [this](const QStringList &args) {
    m_file_panel->UnmarkItem();
  };

  commandMap["unmark-local"] = [this](const QStringList &args) {
      m_file_panel->UnmarkItemsLocal();
  };

  commandMap["unmark-global"] = [this](const QStringList &args) {
      m_file_panel->UnmarkItemsGlobal();
  };

  commandMap["new-file"] = [this](const QStringList &args) {
    m_file_panel->NewFile(args);
  };

  commandMap["new-folder"] = [this](const QStringList &args) {
    m_file_panel->NewFolder(args);
  };

  commandMap["trash"] = [this](const QStringList &args) {
    TrashItems(CommandScope::CURRENT);
  };
  commandMap["trash-local"] = [this](const QStringList &args) {
    TrashItems(CommandScope::LOCAL);
  };
  commandMap["trash-global"] = [this](const QStringList &args) {
    TrashItems(CommandScope::GLOBAL);
  };

  commandMap["exit"] = [this](const QStringList &args) {
    QApplication::quit();
  };

  commandMap["messages-pane"] = [this](const QStringList &args) {
    ToggleMessagesBuffer();
  };

  commandMap["toggle-hidden-files"] = [this](const QStringList &args) {
    m_file_panel->ToggleHiddenFiles();
  };

  commandMap["preview-pane"] = [this](const QStringList &args) {
    TogglePreviewPanel();
  };

  commandMap["menu-bar"] = [this](const QStringList &args) { ToggleMenuBar(); };

  commandMap["filter"] = [this](const QStringList &args) { Filter(); };

  commandMap["unfilter"] = [this](const QStringList &args) { ResetFilter(); };

  commandMap["refresh"] = [this](const QStringList &args) {
    m_file_panel->ForceUpdate();
  };

  commandMap["chmod"] = [this](const QStringList &args) {
    Chmod(CommandScope::CURRENT);
  };
  commandMap["chmod-local"] = [this](const QStringList &args) {
    Chmod(CommandScope::LOCAL);
  };
  commandMap["chmod-global"] = [this](const QStringList &args) {
    Chmod(CommandScope::GLOBAL);
  };

  commandMap["marks-pane"] = [this](const QStringList &args) {
    ToggleMarksBuffer();
  };

  commandMap["focus-path"] = [&](const QStringList &args) {
    m_file_path_widget->FocusLineEdit();
  };

  commandMap["item-property"] = [&](const QStringList &args) {
    m_file_panel->ItemProperty();
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

  commandMap["search"] = [&](const QStringList &args) { m_file_panel->Search(); };
  commandMap["search-next"] = [&](const QStringList &args) { m_file_panel->SearchNext(); };
  commandMap["search-prev"] = [&](const QStringList &args) {
    m_file_panel->SearchPrev();
  };

  m_input_completion_model = new QStringListModel(m_valid_command_list);
  m_inputbar->setCompleterModel(m_input_completion_model);
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
                                        m_file_panel->getCurrentDir())) {
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
    m_file_panel->setCurrentDir(bookmarkPath, true);
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
    m_file_panel->GotoItem(num);
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
  m_inputbar = new Inputbar();
  m_statusbar = new Statusbar();
  m_file_panel = new FilePanel(m_inputbar, m_statusbar);

  m_preview_panel = new PreviewPanel();
  m_file_path_widget = new FilePathWidget();
  m_log_buffer = new MessagesBuffer();
  m_marks_buffer = new MarksBuffer();

  m_marks_buffer->setMarksSet(m_file_panel->getMarksSetPTR());

  m_log_buffer->setAcceptRichText(true);
  m_log_buffer->setReadOnly(true);

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

  connect(kb_next_item, &QShortcut::activated, m_file_panel,
          &FilePanel::NextItem);
  connect(kb_prev_item, &QShortcut::activated, m_file_panel,
          &FilePanel::PrevItem);
  connect(kb_select_item, &QShortcut::activated, m_file_panel,
          &FilePanel::SelectItem);
  connect(kb_up_directory, &QShortcut::activated, m_file_panel,
          &FilePanel::UpDirectory);
  connect(kb_goto_last_item, &QShortcut::activated, m_file_panel,
          &FilePanel::GotoLastItem);
  connect(kb_goto_first_item, &QShortcut::activated, m_file_panel,
          &FilePanel::GotoFirstItem);
  connect(kb_mark_item, &QShortcut::activated, m_file_panel,
          &FilePanel::ToggleMarkItem);
  connect(kb_command, &QShortcut::activated, this,
          &Navi::ExecuteExtendedCommand);
  connect(kb_rename_items, &QShortcut::activated, this,
          [&]() { m_file_panel->RenameItem(); });
  connect(kb_delete_items, &QShortcut::activated, this,
          [&]() { m_file_panel->DeleteItem(); });
  connect(kb_search, &QShortcut::activated, m_file_panel, &FilePanel::Search);
  connect(kb_search_next, &QShortcut::activated, m_file_panel, &FilePanel::SearchNext);
  connect(kb_search_prev, &QShortcut::activated, m_file_panel, &FilePanel::SearchPrev);
  connect(kb_toggle_menubar, &QShortcut::activated, this,
          [this]() { ToggleMenuBar(); });

  connect(kb_toggle_preview_panel, &QShortcut::activated, this,
          [this]() { TogglePreviewPanel(); });

  connect(kb_focus_file_path_widget, &QShortcut::activated, this,
          [this]() { m_file_path_widget->FocusLineEdit(); });

  connect(kb_paste_items, &QShortcut::activated, this, &Navi::PasteItems);
  connect(kb_copy_items, &QShortcut::activated, this,
          [&]() { CopyItems(CommandScope::CURRENT); });
  connect(kb_unmark_items_local, &QShortcut::activated, this,
          [&]() { m_file_panel->UnmarkItemsLocal(); });

  connect(kb_toggle_hidden_files, &QShortcut::activated, m_file_panel,
          &FilePanel::ToggleHiddenFiles);
}

void Navi::CutItems(const CommandScope &scope) noexcept {
  m_file_panel->CutItems();
}

void Navi::CopyItems(const CommandScope &scope) noexcept {
  m_file_panel->CopyItems();
}

void Navi::TrashItems(const CommandScope &scope) noexcept {
  switch (scope) {

  case CommandScope::CURRENT: {
    Result<bool> result = m_file_panel->TrashItem();
    if (result.result())
      m_statusbar->Message(result.reason());
    else
      m_statusbar->Message(result.reason(), MessageType::ERROR, 5);
  } break;

  case CommandScope::LOCAL:
  case CommandScope::GLOBAL:
    break;
  }
}

void Navi::ExecuteExtendedCommand() noexcept {
    // m_minibuffer->ExecuteCommand();
    m_inputbar->enableCommandCompletions();
    QString command = m_inputbar->getInput("Command");
    ProcessCommand(command);
    m_inputbar->disableCommandCompletions();
    // m_file_panel->setFocus();
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

  m_viewmenu__preview_panel = new QAction("Preview Panel");
  m_viewmenu__preview_panel->setCheckable(true);
  m_viewmenu__preview_panel->setChecked(true);

  m_viewmenu__messages = new QAction("Messages");
  m_viewmenu__messages->setCheckable(true);

  m_viewmenu__marks_buffer = new QAction("Marks List");
  m_viewmenu__marks_buffer->setCheckable(true);

  m_viewmenu__bookmarks_buffer = new QAction("Bookmarks");
  m_viewmenu__bookmarks_buffer->setCheckable(true);

  m_viewmenu__files_menu = new QMenu("Files");

  m_viewmenu__files_menu__hidden = new QAction("Hidden");
  m_viewmenu__files_menu__hidden->setCheckable(true);

  m_viewmenu__files_menu__dotdot = new QAction("DotDot");
  m_viewmenu__files_menu__dotdot->setCheckable(true);

  m_viewmenu->addMenu(m_viewmenu__files_menu);
  m_viewmenu__files_menu->addAction(m_viewmenu__files_menu__hidden);
  m_viewmenu__files_menu->addAction(m_viewmenu__files_menu__dotdot);

  m_viewmenu->addAction(m_viewmenu__preview_panel);
  m_viewmenu->addAction(m_viewmenu__menubar);
  m_viewmenu->addAction(m_viewmenu__messages);
  m_viewmenu->addAction(m_viewmenu__marks_buffer);

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

  connect(m_viewmenu__marks_buffer, &QAction::triggered, this,
          [&](const bool &state) { ToggleMarksBuffer(state); });

  connect(m_viewmenu__bookmarks_buffer, &QAction::triggered, this,
          [&](const bool &state) { ToggleBookmarksBuffer(state); });

  connect(m_viewmenu__preview_panel, &QAction::triggered, this,
          [&](const bool &state) { TogglePreviewPanel(state); });

  connect(m_viewmenu__messages, &QAction::triggered, this,
          [&](const bool &state) { ToggleMessagesBuffer(state); });

  connect(m_filemenu__create_new_file, &QAction::triggered, this,
          [&]() { m_file_panel->NewFile(); });

  connect(m_filemenu__create_new_folder, &QAction::triggered, this,
          [&]() { m_file_panel->NewFolder(); });

  connect(m_tools_menu__search, &QAction::triggered, this, [&]() { m_file_panel->Search(); });

  connect(m_viewmenu__files_menu__hidden, &QAction::triggered, m_file_panel,
          &FilePanel::ToggleHiddenFiles);

  connect(m_viewmenu__menubar, &QAction::triggered, this,
          [&](const bool &state) { Navi::ToggleMenuBar(state); });

  // Handle visibility state change to reflect in the checkbox of the menu item

  connect(m_menubar, &Menubar::visibilityChanged, this,
          [&](const bool &state) { m_viewmenu__menubar->setChecked(state); });

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

void Navi::setCurrentDir(const QString &path) noexcept {
  m_file_panel->setCurrentDir(path, true);
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

void Navi::Filter() noexcept {
  m_statusbar->SetFilterMode(true);
  QString filterString = m_inputbar->getInput("Filter String");
  m_file_panel->Filters(filterString);
}

void Navi::ResetFilter() noexcept {
  m_statusbar->SetFilterMode(false);
  m_file_panel->ResetFilter();
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
  m_log_buffer->append(coloredMessage);
}

Navi::~Navi() {}

void Navi::chmodHelper() noexcept {
  QString permString = m_inputbar->getInput("Permission Number");
  if (permString.length() != 3 ||
      !permString.contains(QRegularExpression("^[0-7]{3}$"))) {
    m_statusbar->Message("Invalid permission string."
                         "Expected a three-digit octal string like '755'.",
                         MessageType::ERROR, 5);
    return;
  }
  if (m_file_panel->Chmod(permString)) {
    m_statusbar->Message("Chmodded file successfully");
    m_statusbar->UpdateFile();
  } else
    m_statusbar->Message("Error Chmoding file!", MessageType::ERROR, 5);
}

void Navi::Chmod(const CommandScope &scope) noexcept {

  switch (scope) {
  case CommandScope::CURRENT:
  case CommandScope::LOCAL:
  case CommandScope::GLOBAL:
    break;
  }
}

void Navi::PasteItems() noexcept { m_file_panel->PasteItems(); }

void Navi::SaveBookmarkFile() noexcept {
  if (m_bookmark_manager->saveBookmarksFile()) {
    m_statusbar->Message("Bookmark save successful!");
  } else {
    m_statusbar->Message("Error saving bookmarks!", MessageType::ERROR);
  }
}