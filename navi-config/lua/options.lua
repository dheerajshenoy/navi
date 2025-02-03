local custom_funcs = require("functions")

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

navi.api.create_user_command("zoxide", custom_funcs.zoxide)

navi.ui.cursor.set_props({
    background = "#444444",
})
