local navi = require("navi")

navi.keymap.set( "h", "up-directory", "Go to the parent directory" )
navi.keymap.set( "j", "next-item", "Go to the next item" )
navi.keymap.set( "k", "prev-item", "Go to the previous item" )
navi.keymap.set( "l", "select-item", "Select item" )
navi.keymap.set( "f", "filter", "Filter item visibility" )
navi.keymap.set( "g,g", "first-item", "Go to the first item" )
navi.keymap.set( "z,z", "middle-item", "Go to middle item" )
navi.keymap.set( "Shift+g", "last-item", "Go to the last item" )
navi.keymap.set( "Shift+r", "rename-dwim", "Rename item(s)" )
navi.keymap.set( "Shift+d", "delete-dwim", "Delete item(s)" )
navi.keymap.set( "Space", "toggle-mark-dwim", "Mark item(s)" )
navi.keymap.set( "F5", "refresh", "Refresh current directory" )
navi.keymap.set( "Shift+Space", "mark-inverse", "Mark inverse item(s)" )
navi.keymap.set( "Shift+v", "visual-select", "Visual selection mode" )
navi.keymap.set( ":", "execute-extended-command", "Execute extended command" )
navi.keymap.set( "y,y", "copy-dwim", "Copy item(s)" )
navi.keymap.set( "p", "paste", "Paste item(s)" )
navi.keymap.set( "Shift+u", "unmark-local", "Unmark all item(s)" )
navi.keymap.set( "/", "search", "Search (regex)" )
navi.keymap.set( "n", "search-next", "Search next" )
navi.keymap.set( "Shift+n", "search-prev", "Search previous" )
navi.keymap.set( "Ctrl+m", "menu-bar", "Toggle menu bar" )
navi.keymap.set( "Ctrl+p", "preview-pane", "Toggle preview pane" )
navi.keymap.set( "Ctrl+l", "focus-path", "Focus path bar" )
navi.keymap.set( "Shift+t", "trash-dwim", "Trash item(s)" )
navi.keymap.set( ".", "hidden-files", "Toggle hidden items" )
navi.keymap.set( "q", "macro-record", "Record or Finish recording macro" )
navi.keymap.set( "Shift+c", "copy-path", "Copy path(s)" )
navi.keymap.set( "Ctrl+u", "scroll-up", "Scroll up a page" )
navi.keymap.set( "Ctrl+d", "scroll-down", "Scroll down a page" )


navi.opt.font = "Rajdhani Semibold"

navi.ui.header.columns = {
    { name = "File", type = "file_name" },
    { name = "Size", type = "file_size" },
}

navi.ui.vheader.set_props({
    visible = true,
    border = true,
})

navi.ui.pathbar.set_props({
    bold = true,
})

navi.ui.statusbar.modules =  {
    "name",
    "macro",
    "visual_mode",
    "stretch",
    "filter",
    "count",
    "size",
    "modified_date",
    "permission"
}

navi.ui.menubar.set_props({
    icons = true,
    visible = false
})

navi.ui.header.visible = false
navi.ui.toolbar.visible = false

navi.ui.toolbar.set_items({
    "home",
    "parent_directory",
    "previous_directory",
    "next_directory",
})

navi.ui.vheader.set_props({
    visible = true,
})

-- File Panel
navi.ui.file_panel.set_props({
    font_size = 18,
    icons = true,
    grid = false,
    gridstyle = "dashline",
})

-- Preview Panel
navi.ui.preview_panel.set_props({
    fraction = 0.4,
    image_dimension = { width = 400, height = 400 },
})

navi.opt.cycle = false
navi.opt.hidden_files = false

navi.ui.file_panel.symlink.set_props({
    separator = "🡒 ",
    foreground = "orange",
    bold = true,
    underline = true,
})

navi.ui.cursor.set_props({
    background = "#444444",
})
