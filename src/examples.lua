---@class navi

--- Adding module to statusbar

local module = navi.ui.statusbar.create_module("test", {
    text = "DD",
    italic = true,
    bold = true,
    background = "#FFFFFF"
})

navi.ui.statusbar.add_module(module)


--- Registering user function

function hello ()
    navi.ui.statusbar.set_module_text("test", "FF")
end

navi.api.register_function("hello", hello)


--- Setting toolbar items

navi.ui.toolbar.set_items({
    "home",
    "parent_directory",
    "previous_directory",
})

--- Adding button to toolbar

local btn = navi.ui.toolbar.create_button("btn", {
    name = "BTN",
    icon = "home",
    action = function () navi.io.msg("HELLO") end,
    tooltip = "this is a toolbar button tooltip"
})

-- you can add it using `add_button` function
navi.ui.toolbar.add_button(btn)

-- OR

-- you can add it while setting the buttons for the toolbar using `set_items` function
navi.ui.toolbar.set_items({
    "home",
    btn
})
