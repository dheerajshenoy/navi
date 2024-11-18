#include "Navi.hpp"

void Navi::initNaviLuaAPI() noexcept {

    // ItemProperty type
    lua.new_usertype<FilePanel::ItemProperty>(
        "ItemProperty", "name", &FilePanel::ItemProperty::name, "size",
        &FilePanel::ItemProperty::size, "mimeName",
                                              &FilePanel::ItemProperty::mimeName);

    lua["navi"] = lua.create_table();

    lua["navi"]["add_hook"] = [this](const std::string &hook_name,
                                     const sol::function &func) {
        m_hook_manager->addHook(hook_name, func);
    };

    // DIR API
    lua["navi"]["api"] = lua.create_table();

    lua["navi"]["api"]["cd"] = [this](const std::string &path) {
      this->Lua__ChangeDirectory(path);
    };

    lua["navi"]["api"]["count"] = [this](const std::string &path) {
      this->Lua__ItemCount(path);
    };

    lua["navi"]["api"]["create_dir"] =
        [this](const std::vector<std::string> &paths) {
          this->Lua__CreateFolders(paths);
        };

    lua["navi"]["api"]["is_file"] = [this](const std::string &path) {
        return this->Lua__IsFile(path);
    };

    lua["navi"]["api"]["is_dir"] = [this](const std::string &path) {
      return this->Lua__IsDirectory(path);
    };

    lua["navi"]["api"]["next_item"] = [this]() { m_file_panel->NextItem(); };

    lua["navi"]["api"]["prev_item"] = [this]() { m_file_panel->PrevItem(); };

    lua["navi"]["api"]["first_item"] = [this]() {
      m_file_panel->GotoFirstItem();
    };

    lua["navi"]["api"]["last_item"] = [this]() {
      m_file_panel->GotoLastItem();
    };

    lua["navi"]["api"]["middle_item"] = [this]() {
        m_file_panel->GotoMiddleItem();
    };

    lua["navi"]["api"]["up_directory"] = [this]() {
      m_file_panel->UpDirectory();
    };

    lua["navi"]["api"]["select_item"] = [this]() {
        m_file_panel->SelectItem();
    };


    lua["navi"]["api"]["item_name"] = [this]() {
        return m_file_panel->getCurrentItem().toStdString();
    };

    lua["navi"]["api"]["item_property"] = [this]() {
        return m_file_panel->getItemProperty();
    };

    lua["navi"]["api"]["dir_name"] = [this]() {
      return m_file_panel->getCurrentDir().toStdString();
    };
    lua["navi"]["api"]["filter"] = [this](const QString &filter) {
      m_file_panel->Filters(filter);
    };
    lua["navi"]["api"]["reset_filter"] = [this]() {
      m_file_panel->ResetFilter();
    };
    lua["navi"]["api"]["mark"] = [this]() { return m_file_panel->MarkItem(); };
    lua["navi"]["api"]["mark_inverse"] = [this]() {
      m_file_panel->MarkInverse();
    };
    lua["navi"]["api"]["mark_all"] = [this]() { return m_file_panel->MarkAllItems(); };
    lua["navi"]["api"]["mark_dwim"] = [this]() { return m_file_panel->MarkDWIM(); };
    lua["navi"]["api"]["unmark"] = [this]() { m_file_panel->UnmarkItem(); };
    lua["navi"]["api"]["unmark_dwim"] = [this]() {
      m_file_panel->UnmarkDWIM();
    };
    lua["navi"]["api"]["toggle_mark"] = [this]() {
      m_file_panel->ToggleMarkItem();
    };
    lua["navi"]["api"]["toggle_mark_dwim"] = [this]() {
      m_file_panel->ToggleMarkDWIM();
    };
    lua["navi"]["api"]["rename"] = [this]() { m_file_panel->RenameItem(); };
    lua["navi"]["api"]["rename_dwim"] = [this]() {
      m_file_panel->RenameDWIM();
    };
    lua["navi"]["api"]["delete"] = [this]() { m_file_panel->DeleteItem(); };
    lua["navi"]["api"]["delete_dwim"] = [this]() {
      m_file_panel->DeleteDWIM();
    };

    lua["navi"]["api"]["chmod"] = [this]() { m_file_panel->ChmodItem(); };

    lua["navi"]["api"]["local_marks"] = [this]() {
      return utils::convertToStdVector(
          m_file_panel->model()->getMarkedFilesLocal());
    };

    lua["navi"]["api"]["local_marks_count"] = [this]() {
        return m_file_panel->model()->getMarkedFilesCountLocal();
    };

    lua["navi"]["api"]["global_marks"] = [this]() {
        return utils::convertToStdVector(m_file_panel->model()->getMarkedFiles());
    };

    lua["navi"]["api"]["global_marks_count"] = [this]() {
      return m_file_panel->model()->getMarkedFilesCount();
    };

    lua["navi"]["api"]["has_marks_global"] = [this]() {
        return m_file_panel->model()->hasMarks();
    };

    lua["navi"]["api"]["has_marks_local"] = [this]() {
      return m_file_panel->model()->hasMarksLocal();
    };
    lua["navi"]["api"]["cut"] = [this]() { m_file_panel->CutItem(); };
    lua["navi"]["api"]["cut_dwim"] = [this]() { m_file_panel->CutDWIM(); };
    lua["navi"]["api"]["copy_dwim"] = [this]() { m_file_panel->CopyDWIM(); };
    lua["navi"]["api"]["copy"] = [this]() { return m_file_panel->CopyItem(); };
    lua["navi"]["api"]["trash"] = [this]() { m_file_panel->TrashItem(); };
    lua["navi"]["api"]["new_folder"] =
        [this](const std::vector<std::string> &folderNames) {
          QStringList folders = utils::stringListFromVector(folderNames);
          m_file_panel->NewFolder(folders);
        };

    lua["navi"]["api"]["new_files"] =
        [this](const std::vector<std::string> &fileNames) {
          QStringList files = utils::stringListFromVector(fileNames);
          m_file_panel->NewFile(files);
        };


    lua["navi"]["api"]["search"] = [this](const std::string &searchTerm) {
        m_file_panel->Search(QString::fromStdString(searchTerm));
    };

    lua["navi"]["api"]["search_next"] = [this]() {
      m_file_panel->SearchNext();
    };

    lua["navi"]["api"]["search_prev"] = [this]() {
      m_file_panel->SearchPrev();
    };

    lua["navi"]["api"]["sort_name"] = [this]() {};

    // UI API

    lua["navi"]["ui"] = lua.create_table();

    lua["navi"]["ui"]["menubar"] = [this](const bool &state) {
      ToggleMenuBar(state);
    };

    lua["navi"]["ui"]["menubar"] = [this]() {
      ToggleMenuBar();
    };

    lua["navi"]["ui"]["statusbar"] = [this](const bool &state) {
      ToggleStatusBar(state);
    };

    lua["navi"]["ui"]["statusbar"] = [this]() {
      ToggleStatusBar();
    };

    lua["navi"]["ui"]["pathbar"] = [this](const bool &state) {
      TogglePathWidget(state);
    };

    lua["navi"]["ui"]["pathbar"] = [this]() {
      TogglePathWidget();
    };

    lua["navi"]["ui"]["preview_panel"] = [this](const bool &state) {
      TogglePreviewPanel(state);
    };

    lua["navi"]["ui"]["preview_panel"] = [this]() { TogglePreviewPanel(); };

    lua["navi"]["ui"]["messages"] = [this]() { ToggleMessagesBuffer(); };

    lua["navi"]["ui"]["messages"] = [this](const bool &state) {
      ToggleMessagesBuffer(state);
    };

    lua["navi"]["ui"]["shortcuts"] = [this](const bool &state) {
      ToggleShortcutsBuffer(state);
    };

    lua["navi"]["ui"]["shortcuts"] = [this]() { ToggleShortcutsBuffer(); };

    lua["navi"]["ui"]["marks"] = [this](const bool &state) {
      ToggleMarksBuffer(state);
    };

    lua["navi"]["ui"]["marks"] = [this]() {
        ToggleMarksBuffer();
    };

    // IO API
    lua["navi"]["io"] = lua.create_table();

    lua["navi"]["io"]["msg"] = [this](const std::string &message,
                                          const MessageType &type) {
      this->Lua__Message(message, type);
    };

    lua["navi"]["io"]["msgtype"] = lua.create_table_with(
        "info", MessageType::INFO, "warn", MessageType::WARNING, "error",
                                                      MessageType::ERROR);

    lua["navi"]["io"]["input"] = [this](const std::string &prompt,
                                        const std::string &def_text,
                                        const std::string &selection_text) {
      this->Lua__Input(prompt, def_text, selection_text);
    };

    // SHELL API

    lua["navi"]["shell"] = lua.create_table();
    lua["navi"]["shell"]["execute"] = [this](const std::string &command) {
        this->Lua__Shell(command);
    };
}
