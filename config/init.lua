
SETTINGS = {
    ui = {

        statusbar = {
            visual_line_mode = {
                text = "V",
                background = "#FF5000",
                foreground = "#000000",
                italic = true,
                bold = true,
                padding = "4px",
            },
            macro_mode = {
                text = "M",
                background = "#FF3124",
                foreground = "#000000",
                italic = true,
                bold = true,
                padding = "4px",
            }
        },

        tasks_pane = {
            output_scrollback_lines = 100, -- TODO: Not yet added
        },
    }
}

require("options")
require("keybindings")
