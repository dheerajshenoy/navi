local custom_funcs = require("functions")

navi.ui.header.columns = {
    { name = "File", type = "file_name" },
    { name = "Size", type = "file_size" },
}

-- UI Things

-- navi.ui.preview_panel.visible = false
-- navi.ui.preview_panel.fraction = 0.3

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

navi.ui.menubar.visible = false
navi.ui.header.visible = false
navi.ui.toolbar.visible = false

-- File Panel
navi.ui.file_panel.set_props({
    font_size = 18,
    icons = false
})

-- Preview Panel
navi.ui.preview_panel.set_props({
    visible = false,
    fraction = 0.4,
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


