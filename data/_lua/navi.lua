local M = {}

local _navi = Navi.new()

_store_instance(_navi)

if not _navi then
    print("Could not create NAVI instance")
    os.exit(-1)
end

M.keymap = {}
M.io = {}
M.api = {}
M.ui = {}
M.opt = {}
M.hook = {}

M.opt.bookmark = {}

setmetatable(M.opt.bookmark, {
    __index = function (_, key)

        if key == "auto_save" then
            return _navi:get_bookmark_auto_save()
        end
    end,

    __newindex = function (_, key, value)
        if key == "auto_save" then
            _navi:set_bookmark_auto_save(value)
        end
    end
})

M.opt.bulk_rename = {}

setmetatable(M.opt.bulk_rename, {
    __index = function (_, key)
        if key == "file_threshold" then
            return _navi:get_bulk_rename_file_threshold()

        elseif key == "editor" then
            return _navi:get_bulk_rename_editor()

        elseif key == "with_terminal" then
            return _navi:get_bulk_rename_with_terminal()
        end
    end,
    __newindex = function (_, key, value)
        if key == "file_threshold" then
            return _navi:set_bulk_rename_file_threshold(value)

        elseif key == "editor" then
            return _navi:set_bulk_rename_editor(value)

        elseif key == "with_terminal" then
            return _navi:set_bulk_rename_with_terminal(value)
        end
    end
})

M.opt.symlink = {}
M.opt.highlight = {}
M.opt.mark = {}

setmetatable(M.opt, {
    __index = function(_, key)

        if key == "terminal" then
            return _navi:get_terminal()

        elseif key == "default_directory" then
            return _navi:get_default_dir()

        elseif key == "copy_path_separator" then
            return _navi:get_copy_path_separator()

        elseif key == "cycle" then
            return _navi:get_cycle()
        end

    end,

    __newindex = function(_, key, value)

        if key == "terminal" then
            _navi:set_terminal(value)

        elseif key == "default_directory" then
            _navi:set_default_dir(value)

        elseif key == "copy_path_separator" then
            _navi:set_copy_path_separator(value)

        elseif key == "cycle" then
            _navi:set_cycle(value)

        end
    end
})

---Set the keymap
---@param key string
---@param command string
---@param desc string
M.keymap.set = function (key, command, desc)
    _navi:set_keymap(key, command, desc)
end

M.ui.header = {}

---@class HeaderColumn
---@field name string
---@field type ColumnType can be one of: { "file_name", "file_permission", "file_size", "file_date" }

---@class HeaderOptions
---@field visible boolean
---@field columns HeaderColumn

---Set option for header
---@param opts HeaderOptions
M.ui.header.set = function (opts)
    if opts then
        _navi:set_header_props(opts)
    end
end

---Get options table for header
---@return HeaderOptions
M.ui.header.get = function ()
    return _navi:get_header_props()
end

setmetatable(M.ui.header, {
    __index = function (_, key)
        if key == "visible" then
            return _navi:get_header_visible()

        elseif key == "columns" then
            return _navi:get_header_columns()
        end
    end,
    __newindex = function (_, key, value)
        if key == "visible" then
            _navi:set_header_visible(value)

        elseif key == "columns" then
            if type(value) == "table" then
                return _navi:set_header_columns(value)
            end
        end
    end

})

--[[ Inputbar metatable ]]--
M.ui.inputbar = {}

---@class InputbarOptions
---@field font_size integer - font size in pixel
---@field foreground string - foreground color
---@field background string - background color
---@field font string - font family

---Sets the options for inputbar
---@param opts InputbarOptions
M.ui.inputbar.set = function (opts)
    if opts then
        _navi:set_preview_panel_props(opts)
    end
end

---Gets the options table for inputbar
---@return InputbarOptions
M.ui.inputbar.get = function ()
    return _navi:get_inputbar_props()
end

setmetatable(M.ui.inputbar, {

    __index = function (_, key)

        if key == "font" then
            return _navi:get_inputbar_font()

        elseif key == "background" then
            return _navi:get_inputbar_background()

        elseif key == "foreground" then
            return _navi:get_inputbar_foreground()

        elseif key == "font_size" then
            return _navi:get_inputbar_font_size()

        elseif key == "visible" then
            return _navi:get_inputbar_visible()

        end
    end,

    __newindex = function (_, key, value)
        if key == "font" then
            _navi:set_inputbar_font(value)

        elseif key == "background" then
            _navi:set_inputbar_background(value)

        elseif key == "foreground" then
            _navi:set_inputbar_foreground(value)

        elseif key == "font_size" then
            _navi:set_inputbar_font_size(value)

        end
    end
})

M.ui.pathbar = {}

M.ui.pathbar.toggle = function ()
    _navi:pathbar_toggle()
end

---Visibility of pathbar
---@param state boolean
M.ui.pathbar.visible = function (state)
    _navi:pathbar_shown(state)
end

---Focus the pathbar
M.ui.pathbar.focus = function ()
    _navi:focus_pathbar()
end

M.ui.statusbar = {}

---Toggles the statusbar
M.ui.statusbar.toggle = function ()
    _navi:toggle_statusbar()
end

---Set/update text to statusbar module
---@param name string
---@param value string
M.ui.statusbar.set_module_text = function (name, value)
    _navi:set_statusbar_module_text(name, value)
end

---Sets modules to statusbar
---@param modules string[]
M.ui.statusbar.set_modules = function (modules)
    _navi:set_statusbar_modules(modules)
end

---@class StatusbarModuleOptions
---@field text string
---@field italic boolean
---@field bold boolean
---@field background string
---@field foreground string
---@field visible boolean

---@class StatusbarModule
---@field name string
---@field options StatusbarModuleOptions

---Creates module with name and options
---@param name string
---@param options table
---@return StatusbarModule
M.ui.statusbar.create_module = function (name, options)
    return _navi:create_statusbar_module(name, options)
end

---Adds a module to the statusbar
---@param module StatusbarModule
M.ui.statusbar.add_module = function (module)
    return _navi:add_statusbar_module(module)
end

M.ui.toolbar = {}

---Toggle the toolbar
M.ui.toolbar.toggle = function ()
    _navi:toggle_toolbar()
end

---Represents a toolbar button with properties
---@class ToolbarItem
---@field label string - text to display
---@field icon string - path to valid image or standard theme icon names
---@field action function
---@field position integer
---@field tooltip string

---Represents toolbar button options
---@class ToolbarItemOptions
---@field label string
---@field icon string
---@field action function
---@field position integer
---@field tooltip string

---Creates and returns a toolbar button
---@param name string
---@param options ToolbarItemOptions
---@return ToolbarItem
M.ui.toolbar.create_button = function (name, options)
    return _navi:create_toolbar_button(name, options)
end

---Adds button to toolbar
---@param button ToolbarItem
M.ui.toolbar.add_button = function (button)
    _navi:add_toolbar_button(button)
end

---Sets item for toolbar
---@param items string[]
M.ui.toolbar.set_items = function (items)
    if type(items) == "table" then
        _navi:set_toolbar_layout(items)
    end
end

---@class ToolbarOptions
---@field icons_only boolean
---@field visible boolean

---Sets the options for toolbar
---@param opts ToolbarOptions
M.ui.toolbar.set = function (opts)
    _navi:set_toolbar_props(opts)
end

setmetatable(M.ui.toolbar, {
    __index = function (_, key)
        if key == "icons_only" then
            return _navi:get_toolbar_icons_only()

        elseif key == "visible" then
            return _navi:get_toolbar_visible()

        end
    end,
    __newindex = function (_, key, value)

        if key == "icons_only" then
            _navi:set_toolbar_icons_only(value)

        elseif key == "visible" then
            return _navi:set_toolbar_visible(value)

        end

    end

})

M.ui.file_panel = {}

---@class FilePanelOptions
---@field icons boolean whether to show icons or not
---@field font_size integer
---@field font string font to use


---Sets options for file panel
---@param props FilePanelOptions
M.ui.file_panel.set = function (props)
    _navi:set_file_panel_props(props)
end

---Gets options for file panel
---@return FilePanelOptions
M.ui.file_panel.get = function ()
    return _navi:get_file_panel_props()
end

setmetatable(M.ui.file_panel, {
    __index = function (_, key)

        if key == "icons" then
            return _navi:get_file_panel_icons()

        elseif key == "font_size" then
            return _navi:get_file_panel_font_size()

        elseif key == "font" then
            return _navi:get_file_panel_font()

        end

    end,
    __newindex = function (_, key, value)

        if key == "icons" then
            _navi:set_file_panel_icons(value)

        elseif key == "font_size" then
            _navi:set_file_panel_font_size(value)

        elseif key == "font" then
            _navi:set_file_panel_font(value)

        end
    end

})

---@class MenuItem
---@field label string - text to display
---@field submenu table - table with fields same as MenuItem (optional)
---@field action function - action to execute on clicking the item (optional)

M.ui.menubar = {}

M.ui.menubar.toggle = function ()
    _navi:toggle_menubar()
end

---Add menu to menubar
---@param item MenuItem
M.ui.menubar.add_menu = function (item)
    _navi:add_menubar_menu(item)
end

M.ui.preview_panel = {}

---Toggle preview panel
M.ui.preview_panel.toggle = function ()
    _navi:toggle_preview_panel()
end

---@class PreviewPanelOptions
---@field max_file_size string (max file size to preview)
---@field image_dimension table { width (integer), height (integer) }
---@field visible boolean (visibility of the preview panel)
---@field fraction number (width fraction of the main window)
---@field max_lines_to_read integer (number of lines to read from a text file)

---Sets the options for preview panel
---@param opts PreviewPanelOptions
M.ui.preview_panel.set = function (opts)
    if opts then
        _navi:set_preview_panel_props(opts)
    end
end

---Gets options table for preview panel
---@return PreviewPanelOptions
M.ui.preview_panel.get = function ()
    return _navi:get_preview_panel_props()
end

setmetatable(M.ui.preview_panel, {

    __index = function (_, key)
        if key == "visible" then
            return _navi:get_preview_panel_visible()

        elseif key == "fraction" then
            return _navi:get_preview_panel_fraction()

        elseif key == "image_dimension" then
            return _navi:get_preview_panel_image_dimension()

        elseif key == "max_file_size" then
            return _navi:get_preview_panel_max_file_size()
        end
    end,
    __newindex = function (_, key, value)
        if key == "visible" then
            return _navi:set_preview_panel_visible(value)

        elseif key == "fraction" then
            _navi:set_preview_panel_fraction(value)

        elseif key == "image_dimension" then
            _navi:set_preview_panel_image_dimension(value)

        elseif key == "max_file_size" then
            _navi:set_preview_panel_max_file_size(value)
        end
    end
})

---Returns the runtimepaths of Navi
---@return table
M.api.list_runtime_paths = function ()
    return _navi:list_runtime_paths()
end

---Registers a lua function to be used interactively in Navi
---@param name string
---@param func function
M.api.create_user_command = function (name, func)
    _navi:create_user_command(name, func)
end

---Unregister a function from Navi interactive functions
---@param name string
M.api.remove_user_command = function (name)
    _navi:remove_user_command(name)
end

---Returns the number of item(s) in the current working directory
---@return integer
M.api.count = function ()
    return _navi:count()
end

---Searches the current working directory for the item with the provided base name
---@param base_name string
M.api.search = function (base_name)
    _navi:search(base_name)
end

---Searches for the next occurence of the base name in the current working directory
M.api.search_next = function ()
    _navi:search_next()
end

---Searches for the previous occurence of the base name in the current working directory
M.api.search_prev = function ()
    _navi:search_prev()
end

---Go to the next item
M.api.next_item = function ()
    _navi:next_item()
end

---Go to the previous item
M.api.prev_item = function ()
    _navi:prev_item()
end

---Go to the first item
M.api.first_item = function()
    _navi:first_item()
end

---Go to the last item
M.api.last_item = function()
    _navi:last_item()
end

---Change the current working directory
---@param path string
M.api.cd = function (path)
    _navi:cd(path)
end

---Get the current working directory
---@return string
M.api.pwd = function ()
    return _navi:pwd()
end

---Copy
M.api.copy = function ()
    return _navi:copy()
end

---Copy (DWIM)
M.api.copy_dwim = function ()
    return _navi:copy_dwim()
end

---cut
M.api.cut = function ()
    return _navi:cut()
end

---Cut (DWIM)
M.api.cut_dwim = function ()
    return _navi:cut_dwim()
end

---Trash current item
M.api.trash = function ()
    return _navi:trash()
end

---Trash (DWIM)
M.api.trash_dwim = function ()
    return _navi:trash_dwim()
end

---Mark current item
M.api.mark = function ()
    return _navi:mark()
end

---Mark (DWIM)
M.api.mark_dwim = function ()
    return _navi:mark_dwim()
end

---Mark all files in the current working directory
M.api.mark_all = function ()
    return _navi:mark_all()
end

---Mark current directory inversly
M.api.mark_inverse = function ()
    return _navi:mark_inverse()
end

---Unmark current item
M.api.unmark = function ()
    return _navi:unmark()
end

---Unmark (DWIM)
M.api.unmark_dwim = function ()
    return _navi:unmark_dwim()
end

---Marks files using regex expression
---@param regex string
M.api.mark_regex = function (regex)
    return _navi:mark_regex(regex)
end

---Returns `t` if there are local marks (marks in the current directory)
---@return boolean
M.api.has_marks_local = function ()
    return _navi:has_marks_local()
end

---Returns `t` if there are global marks (marks anywhere)
---@return boolean
M.api.has_marks_global = function ()
    return _navi:has_marks_global()
end

---Get the list of global marks
---@return table
M.api.global_marks = function ()
    return _navi:global_marks()
end

---Get the list of local marks
---@return table
M.api.local_marks = function ()
    return _navi:local_marks()
end

---Get the count of local marks
---@return integer
M.api.local_marks_count = function ()
    return _navi:local_marks_count()
end

---Get the count of global marks
---@return integer
M.api.global_marks_count = function ()
    return _navi:global_marks_count()
end

---Highlights item with the given basename
---@param item_name string
M.api.highlight = function (item_name)
    return _navi:highlight(item_name)
end

---Filters item in the current directory with the provided filters
---@param filter_string string
M.api.filter = function (filter_string)
    return _navi:filter(filter_string)
end

---Resets filter_string
M.api.reset_filter = function ()
    return _navi:reset_filter()
end

---Toggles mark of current item
M.api.toggle_mark = function ()
    return _navi:toggle_mark()
end

--- Toggles mark (DWIM)
M.api.toggle_mark_dwim = function ()
    return _navi:toggle_mark_dwim()
end

---Renames current item
M.api.rename = function ()
    return _navi:rename()
end

---Renames current item
M.api.rename_dwim = function ()
    return _navi:rename_dwim()
end

---Deletes current item
M.api.delete = function ()
    return _navi:rename_dwim()
end

---Deletes current item (DWIM)
M.api.delete_dwim = function ()
    return _navi:delete_dwim()
end

---Execute a shell command asynchronously
---@param command string
---@param args string[]
M.api.execute = function (command, args)
    return _navi:execute(command, args)
end

---MessageType enum
---@enum MessageType
M.io.msgtype = {
    info = 0,
    warning = 1,
    error = 2
}

---Show a message to user
---@param msg string - message to display
---@param type MsgType
M.io.msg = function (msg, type)
    type = type or M.io.msgtype.info
    _navi:message(msg, type)
end

---Get input from user and return the input value
---@param prompt string - prompt text to display
---@param title string - title of the input window
---@param default_text string - placeholder text
M.io.input = function (prompt, title, default_text)
    return _navi:input(prompt, title, default_text)
end

---Get choice out of a list of items from user and return the choice
---@param prompt string - prompt text to display
---@param title string - title of the input window
---@param items string[] - list of items to give as options
---@param default_choice integer - default choice (1 bases index)
M.io.input_choice = function (prompt, title, items, default_choice)
    if default_choice < 0 then
        if #items > 0 then
            default_choice = 1
        end
    end
    return _navi:input_items(prompt, title, items, default_choice)
end

---Adds a function to the list for hook
---@param name string
---@param func function
M.hook.add = function (name, func)
    _navi:add_hook(name, func)
end

---Removes all the functions registered for hook
M.hook.clear_functions = function (name)
    _navi:clear_functions_for_hook(name)
end

---Triggers hook
---@param name string
M.hook.trigger = function (name)
    _navi:trigger_hook(name)
end

setmetatable(M.ui.menubar, {

    __index = function (_, key)
        if key == "visible" then
            return _navi:get_menubar_visible()

        elseif key == "icons" then
            return _navi:get_menubar_icons()
        end

    end,
    __newindex = function (_, key, value)
        if key == "visible" then
            return _navi:set_menubar_visible(value)

        elseif key == "icons" then
            return _navi:set_menubar_icons(value)
        end
    end
})

setmetatable(M.ui.pathbar, {

    __index = function (_, key)
        if key == "visible" then
            return _navi:get_pathbar_visible()

        elseif key == "font" then
            return _navi:get_pathbar_font()

        elseif key == "font_size" then
            return _navi:get_pathbar_font_size()

        elseif key == "italic" then
            return _navi:get_pathbar_italic()

        elseif key == "bold" then
            return _navi:get_pathbar_bold()

        end
    end,
    __newindex = function (_, key, value)
        if key == "visible" then
            return _navi:set_pathbar_visible(value)

        elseif key == "font" then
            return _navi:get_pathbar_font()

        elseif key == "font_size" then
            return _navi:get_pathbar_font_size()

        elseif key == "italic" then
            return _navi:get_pathbar_italic()

        elseif key == "bold" then
            return _navi:get_pathbar_bold()
        end
    end
})


setmetatable(M.ui.statusbar, {

    __index = function (_, key)

        if key == "font" then
            return _navi:get_statusbar_font()

        elseif key == "font_size" then
            return _navi:get_statusbar_font_size()

        elseif key == "background" then
            return _navi:get_statusbar_background()

        elseif key == "visible" then
            return _navi:get_statusbar_visible()

        elseif key == "modules" then
            return _navi:get_statusbar_modules()

        end
    end,
    __newindex = function (_, key, value)

        if key == "font" then
            _navi:set_statusbar_font(value)

        elseif key == "font_size" then
            _navi:set_statusbar_font_size(value)

        elseif key == "background" then
            _navi:set_statusbar_background(value)

        elseif key == "visible" then
            _navi:set_statusbar_visible(value)

        elseif key == "modules" then
            if type(value) == "table" then
                _navi:set_statusbar_modules(value)
            end

        end

    end
})

return M
