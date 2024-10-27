#include "Navi.hpp"

Navi::Navi(QWidget *parent) : QMainWindow(parent) {

  initLayout();       // init layout
  initMenubar();      // init menubar
  initSignalsSlots(); // init signals and slots
  initKeybinds();
  setupCommandMap();
  setCurrentDir("~"); // set the current directory
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
          m_file_panel, &FilePanel::setCurrentDir);

  connect(m_file_panel, &FilePanel::dirItemCount, m_statusbar,
          &Statusbar::SetNumItems);

  connect(m_file_panel, &FilePanel::copyPastaDone, this,
          [&](const bool &state, const QString &reason) {
            if (state)
              m_statusbar->Message("Paste Successful");
            else
              m_statusbar->Message(
                  QString("Error while Pasting! (%1)").arg(reason),
                  MessageType::ERROR, 5);
          });

  connect(m_statusbar, &Statusbar::logMessage, this, &Navi::LogMessage);
}

// Help for HELP interactive function
void Navi::ShowHelp() noexcept {}

// Setup the commandMap HashMap with the function calls
void Navi::setupCommandMap() noexcept {
  commandMap["rename"] = [this]() { RenameItems(); };

  commandMap["help"] = [this]() { ShowHelp(); };

  commandMap["copy"] = [this]() { CopyItems(); };

  commandMap["cut"] = [this]() { CutItems(); };

  commandMap["paste"] = [this]() { PasteItems(); };

  commandMap["delete"] = [this]() { DeleteItems(); };

  commandMap["mark"] = [this]() { m_file_panel->MarkItems(); };

  commandMap["toggle-mark"] = [this]() { m_file_panel->MarkOrUnmarkItems(); };

  commandMap["unmark"] = [this]() { m_file_panel->UnmarkItems(); };

  commandMap["new-file"] = [this]() { NewFile(); };

  commandMap["new-folder"] = [this]() { NewFolder(); };

  commandMap["trash"] = [this]() { TrashItems(); };

  commandMap["exit"] = [this]() { QApplication::quit(); };

  commandMap["messages"] = [this]() { ToggleMessagesBuffer(); };

  commandMap["toggle-hidden-files"] = [this]() {
    m_file_panel->ToggleHiddenFiles();
  };

  commandMap["toggle-preview-pane"] = [this]() { TogglePreviewPanel(); };

  commandMap["toggle-menu-bar"] = [this]() { ToggleMenuBar(); };

  commandMap["filter"] = [this]() { Filter(); };

  commandMap["unfilter"] = [this]() { ResetFilter(); };

  commandMap["refresh"] = [this]() { m_file_panel->ForceUpdate(); };

  commandMap["chmod"] = [this]() { Chmod(); };

  m_input_completion_model = new QStringListModel(m_valid_command_list);
  m_inputbar->setCompleterModel(m_input_completion_model);
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

// Minibuffer process commands
void Navi::ProcessCommand(const QString &commandtext) noexcept {
  QStringList commandlist = commandtext.split(" ");
  if (commandlist.isEmpty())
    return;

  QString command = commandlist[0];
  // QString arg = (commandlist.size() > 1) ? commandlist[1] : "";

  auto [isNumber, num] = utils::isNumber(command);
  if (isNumber) {
      m_file_panel->GotoItem(num);
      return;
  }

  if (commandMap.contains(command)) {
    commandMap[command](); // Call the associated function
  } else {
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
  m_file_panel = new FilePanel();

  m_preview_panel = new PreviewPanel();
  m_file_path_widget = new FilePathWidget();
  m_statusbar = new Statusbar();
  m_inputbar = new Inputbar();
  m_log_buffer = new MessagesBuffer();

  m_log_buffer->setAcceptRichText(true);
  m_log_buffer->setReadOnly(true);

  m_file_path_widget->setFocusPolicy(Qt::FocusPolicy::NoFocus);
  m_file_panel->setFocusPolicy(Qt::FocusPolicy::StrongFocus);

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
  QShortcut *kb_paste_items = new QShortcut(QKeySequence("p"), this);
  QShortcut *kb_search = new QShortcut(QKeySequence("/"), this);
  QShortcut *kb_search_next = new QShortcut(QKeySequence("n"), this);
  QShortcut *kb_search_prev = new QShortcut(QKeySequence("Shift+n"), this);
  QShortcut *kb_toggle_menubar = new QShortcut(QKeySequence("Ctrl+m"), this);
  QShortcut *kb_toggle_preview_panel =
      new QShortcut(QKeySequence("Ctrl+p"), this);
  QShortcut *kb_focus_file_path_widget =
      new QShortcut(QKeySequence("Ctrl+l"), this);

  connect(kb_next_item, &QShortcut::activated, m_file_panel, &FilePanel::NextItem);
  connect(kb_prev_item, &QShortcut::activated, m_file_panel, &FilePanel::PrevItem);
  connect(kb_select_item, &QShortcut::activated, m_file_panel,
          &FilePanel::SelectItem);
  connect(kb_up_directory, &QShortcut::activated, m_file_panel,
          &FilePanel::UpDirectory);
  connect(kb_goto_last_item, &QShortcut::activated, m_file_panel,
          &FilePanel::GotoLastItem);
  connect(kb_goto_first_item, &QShortcut::activated, m_file_panel,
          &FilePanel::GotoFirstItem);
  connect(kb_mark_item, &QShortcut::activated, m_file_panel,
          &FilePanel::MarkOrUnmarkItems);
  connect(kb_command, &QShortcut::activated, this,
          &Navi::ExecuteExtendedCommand);
  connect(kb_rename_items, &QShortcut::activated, this, &Navi::RenameItems);
  connect(kb_delete_items, &QShortcut::activated, this, &Navi::DeleteItems);
  connect(kb_search, &QShortcut::activated, this, &Navi::Search);
  connect(kb_search_next, &QShortcut::activated, this, &Navi::SearchNext);
  connect(kb_search_prev, &QShortcut::activated, this, &Navi::SearchPrev);
  connect(kb_toggle_menubar, &QShortcut::activated, this,
          [&]() { ToggleMenuBar(); });

  connect(kb_toggle_preview_panel, &QShortcut::activated, this,
          [&]() { TogglePreviewPanel(); });

  connect(kb_focus_file_path_widget, &QShortcut::activated, this,
          [&]() { m_file_path_widget->FocusLineEdit(); });

  connect(kb_paste_items, &QShortcut::activated, this, &Navi::PasteItems);
}

void Navi::Search() noexcept {
  m_file_panel->Search(m_inputbar->getInput("Search"));
}

void Navi::SearchNext() noexcept { m_file_panel->SearchNext(); }

void Navi::SearchPrev() noexcept { m_file_panel->SearchPrev(); }

void Navi::RenameItems() noexcept {
  Result res = m_file_panel->RenameItems();

  if (res.result())
    m_statusbar->Message("Rename Successful!");
  else
    m_statusbar->Message(
        QString("Error while renaming! (%1)").arg(res.reason()),
        MessageType::ERROR, 5);
}

void Navi::DeleteItems() noexcept {
  if (m_file_panel->DeleteItems())
    m_statusbar->Message("Deletion Successful!");
  else
    m_statusbar->Message("Error while deleting!", MessageType::ERROR, 5);
}

void Navi::CutItems() noexcept { m_file_panel->CutItems(); }

void Navi::CopyItems() noexcept { m_file_panel->CopyItems(); }

void Navi::TrashItems() noexcept {
  if (m_file_panel->TrashItems())
    m_statusbar->Message("Deletion Successful!");
  else
    m_statusbar->Message("Error while moving!", MessageType::ERROR, 5);
}

void Navi::ExecuteExtendedCommand() noexcept {
  // m_minibuffer->ExecuteCommand();
  QString command = m_inputbar->getInput("Command");
  ProcessCommand(command);
  m_file_panel->setFocus();
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

  connect(m_viewmenu__preview_panel, &QAction::triggered, this,
          [&](const bool &state) { TogglePreviewPanel(state); });

  connect(m_viewmenu__messages, &QAction::triggered, this,
          [&](const bool &state) { ToggleMessagesBuffer(state); });

  connect(m_filemenu__create_new_file, &QAction::triggered, this,
          [&]() { this->NewFile(); });

  connect(m_filemenu__create_new_folder, &QAction::triggered, this,
          [&]() { this->NewFolder(); });

  connect(m_tools_menu__search, &QAction::triggered, this, [&]() {
    QString searchText =
        QInputDialog::getText(this, "Search", "Enter a file/directory name");
    if (!(searchText.isEmpty() && searchText.isNull()))
      m_file_panel->Search(searchText);
  });

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
}

bool Navi::createEmptyFile(const QString &filePath) noexcept {
  QFile file(filePath);

  if (file.open(QIODevice::WriteOnly)) {
    file.close();
    return true;
  }
  return false;
}

// Create an empty file
void Navi::NewFile(const int &nfiles) noexcept {
  // Interactive file creation
  if (nfiles == -1) {
    QString filename = QInputDialog::getText(this, "Create New File",
                                             "Enter the name for the file");

    if (!(filename.isEmpty() && filename.isNull())) {
      if (createEmptyFile(m_file_panel->getCurrentDir() + QDir::separator() +
                          filename))
        m_statusbar->Message("File created successfully!");
      else
        m_statusbar->Message("Error creating file!", MessageType::ERROR, 5);
    }
  }
}

// Create an empty folder
void Navi::NewFolder(const int &nfolders) noexcept {
  // Interactive folder creation
  if (nfolders == -1) {
    QString dirname =
        QInputDialog::getText(this, "Create Folder(s)",
                              "Enter a folder name (slash separations are "
                              "considered as sub-directories)");
    if (!(dirname.isEmpty() && dirname.isNull())) {
      QDir dir;
      if (dir.mkpath(m_file_panel->getCurrentDir() + QDir::separator() +
                     dirname))
        m_statusbar->Message("Folders created successfully!");
      else
        m_statusbar->Message("Error creating folders!", MessageType::ERROR, 5);
    }
  }
}

void Navi::setCurrentDir(const QString &path) noexcept {
  m_file_panel->setCurrentDir(path);
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

void Navi::Chmod() noexcept {
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

void Navi::PasteItems() noexcept { m_file_panel->PasteItems(); }