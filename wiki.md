
# Table of Contents

1.  [Naming Conventions](#org7e89bc1)
2.  [Bookmarks](#orgb1b32bb)
3.  [Interactive Commands](#org5101381)
    1.  [Types of Command](#orgf8e319b)
        1.  [Current Scope](#org422b385)
        2.  [Local Scope](#org5a1f8a7)
        3.  [Global Scope](#org69282e8)
4.  [Commands List](#orgb1814e2)
    1.  [Marking Item](#org9b259aa)
        1.  [mark](#orgfb14829)
        2.  [toggle-mark](#orgb4de1d5)
        3.  [mark-all](#org81e7369)
        4.  [mark-inverse](#org7d18f1c)
    2.  [Unmarking Item](#org99ae645)
        1.  [unmark](#org3d7febe)
        2.  [unmark-local](#orgaaad429)
        3.  [unmark-global](#org9ff8847)
    3.  [Change Permission](#org6d61548)
        1.  [chmod](#org776b266)
        2.  [chmod-local](#orga79f368)
        3.  [chmod-global](#org9762791)
    4.  [Renaming Files](#org576865a)
        1.  [rename](#orgc633442)
        2.  [rename-local](#orgf435b94)
        3.  [rename-global](#orgf0f724c)
    5.  [Cutting Files](#org5bdd14d)
        1.  [cut](#org6299f2c)
        2.  [cut-local](#org77112f4)
        3.  [cut-global](#orgd79b04f)
    6.  [Copying Files](#org939c976)
        1.  [copy](#org1c9b5d6)
        2.  [copy-local](#orgcb12ebd)
        3.  [copy-global](#org04c30e0)
    7.  [Pasting (Moving) Files](#org283e7a3)
        1.  [paste](#orgb9eb88b)
    8.  [Deleting Files](#orga9e76e0)
        1.  [delete](#org6f80ee4)
        2.  [delete-local](#orgfb8f0c2)
        3.  [delete-global](#org4f1f6aa)
    9.  [Trashing Files](#org32718dd)
        1.  [trash](#org9428993)
        2.  [trash-local](#orgf60fa37)
        3.  [trash-global](#org850a37e)
    10. [filter](#org535b922)
    11. [unfilter](#org11ab0df)
    12. [refresh](#org05e352f)
    13. [hidden-files](#org6965b44)
    14. [Panes](#orgf7b3d48)
        1.  [bookmark-pane](#org7bb728b)
        2.  [marks-pane](#org0e4e747)
        3.  [messages-pane](#orge9c5cc1)
        4.  [preview-pane](#org902c400)
    15. [Misc](#orgf24acb3)
        1.  [menu-bar](#org3af06ca)
        2.  [focus-path](#orgf2d7d38)
        3.  [item-property](#org777a230)
        4.  [toggle-header](#org46539d7)
        5.  [toggle-cycle](#org39eb10e)
    16. [Shell Commands](#orgfc342c4)
        1.  [shell-command](#org83a3276)
        2.  [shell-command-async](#orgc74b2a5)
    17. [Bookmarks](#org9b87b9d)
        1.  [bookmark-add](#org6b21ff3)
        2.  [bookmark-remove](#orgcd41816)
        3.  [bookmark-edit](#org17cdc75)
        4.  [bookmark-go](#orgb7938fe)
        5.  [bookmark-save](#orgab3d565)
    18. [Searching Items](#org6a5c95a)
        1.  [search](#orgde316cd)
        2.  [search-next](#org78746fc)
        3.  [search-prev](#org03ddbd8)
5.  [Linux Only](#org210aca5)
6.  [Configuration with Lua](#org10fab7c)
7.  [Scripting with Lua](#org07bf2b0)

**NOTE: This page is still a work in progress.**


<a id="org7e89bc1"></a>

# Naming Conventions

1.  `Item` refers to a file or a directory.

2.  `Highlighting` refers to the action of clicking a directory item (one
    or more than one) once. This means that you can use your mouse to
    select and highlight multiple items in the directory.

3.  `Marking` refers to the action of the `mark` or `toggle-mark` command
    to "select" one or more currently highlighted directory items. You
    can see the list of all the files that you have marked in the
    `Marks List` using `View -> Marks List` or by using the `show-marks`
    command. When you call any of the commands, the commands will be
    aware of these markings and do the action accordingly. For example,
    if we use the `rename` command and if there are marked files, your
    choice of text editor is opened with the list of all the file names.
    You can change these and write to the file to rename them
    accordingly.


<a id="orgb1b32bb"></a>

# Bookmarks

Bookmarks allows you to instantly go to your favourite or frequently
visited directories. You can add, edit, remove and even load bookmarks
directly from a text file. Bookmarks are read and stored in plain text
files that has the following structure:

​# This is a comment &#x2013; Any lines that begin with # are ignored and empty
lines are ignored as well.

bookmarktitle1 bookmarkdirectorypath1 bookmarktitle2
bookmarkdirectorypath2


<a id="org5101381"></a>

# Interactive Commands

These are commands that can be executed using the `inputbar` UI
component. By default, pressing `: (colon)` key pops up the inputbar.
You can type a name of the command in this bar or a line number to go to
in the current directory of the focused pane. Example: `mark` or `42`,
where `mark` would mark the current item under focus or highlighted
items (more than one items).


<a id="orgf8e319b"></a>

## Types of Command

There are three variants for most of the commands: `Current`, `Local`
and `Global`. If any interactive command has scope available, they are
named accordingly. For example, the `rename` command is actually the
`Current` scoped command. It's other variants are `rename-local` and
`rename-global` for `Local` and `Global` scope respectively.


<a id="org422b385"></a>

### Current Scope

This applies to the currently selected item. Any command that is
`Current` scoped will run only on the currently highlighted item.

**This includes mouse selection (also called as Highlighting)**.


<a id="org5a1f8a7"></a>

### Local Scope

This applies to the marks that are in the current directory which is
conveniently called as a **Local Mark**. Any command that is `Local`
scoped will run on the local marks.

**This does not include mouse highlighting.**


<a id="org69282e8"></a>

### Global Scope

This applies to the marks that are present in any directory. Unlike the
`Local` scope the command that is `Global` scoped will run on the marks
that are present in any directory.

**This does not include mouse highlighting**.


<a id="orgb1814e2"></a>

# Commands List

Below is an exhaustive list of commands that are currently available in
Navi. The default keybindings associated with them are mentioned. Below
is a mapping of what some keys mean.

1.  `C` - Control
2.  `M` - Meta (Alt Key)
3.  `S` - Shift
4.  `SPC` - Space


<a id="org9b259aa"></a>

## Marking Item


<a id="orgfb14829"></a>

### mark

Marks the current item.


<a id="orgb4de1d5"></a>

### toggle-mark

Toggle the mark of the current item.

Default keybinding: `SPC`


<a id="org81e7369"></a>

### mark-all

Marks all the items in the current directory.

Default keybinding: `C-a`


<a id="org7d18f1c"></a>

### mark-inverse

Inverses the marks in the current directory.

Default keybinding: `S-SPC`


<a id="org99ae645"></a>

## Unmarking Item


<a id="org3d7febe"></a>

### unmark

Unmarks the current item.


<a id="orgaaad429"></a>

### unmark-local

`unmark` for **Local Marks**

Default keybinding: `S-u`


<a id="org9ff8847"></a>

### unmark-global

`unmark` for **Global Marks**

Default keybinding: `C-S-u`


<a id="org6d61548"></a>

## Change Permission


<a id="org776b266"></a>

### chmod

Change the permission of the current item using three digit numbers

Example: 777, 666, 000 etc.


<a id="orga79f368"></a>

### chmod-local

Similar to `chmod` for **Local Marks**


<a id="org9762791"></a>

### chmod-global

Similar to `chmod` for **Global Marks**


<a id="org576865a"></a>

## Renaming Files

**NOTE: Renaming more than one item leads to *bulk renaming*. This is
where a text editor of your choice (set it in the configuration) opens
up a temporary "rename file" and enables you to rename all the marked
files once you save and close the said "rename file"**


<a id="orgc633442"></a>

### rename

Rename the highlighted item.

Default keybinding: `S-r`


<a id="orgf435b94"></a>

### rename-local

`rename` for **Local Marks**


<a id="orgf0f724c"></a>

### rename-global

`rename` for **Global Marks**


<a id="org5bdd14d"></a>

## Cutting Files


<a id="org6299f2c"></a>

### cut

Cut (prepare for moving) the current item.

Default keybinding: `C-x`


<a id="org77112f4"></a>

### cut-local

`cut` for **Local Marks**


<a id="orgd79b04f"></a>

### cut-global

`cut` for **Global Marks**


<a id="org939c976"></a>

## Copying Files


<a id="org1c9b5d6"></a>

### copy

Copy the current item.

Default keybinding: `C-c`


<a id="orgcb12ebd"></a>

### copy-local

`copy` for **Local Marks**


<a id="org04c30e0"></a>

### copy-global

`copy` for **Global Marks**


<a id="org283e7a3"></a>

## Pasting (Moving) Files


<a id="orgb9eb88b"></a>

### paste

Paste (Move) the current item.

Default keybinding: `p`


<a id="orga9e76e0"></a>

## Deleting Files


<a id="org6f80ee4"></a>

### delete

Delete the highlighted items(s).

**WARNING: Please be careful when using this command, this does not trash
the items, it directly deletes them. If you want to trash use the
*trash* command**

Default keybinding: `S-d`


<a id="orgfb8f0c2"></a>

### delete-local

`delete` for **Local Marks**


<a id="org4f1f6aa"></a>

### delete-global

`delete` for **Global Marks**


<a id="org32718dd"></a>

## Trashing Files


<a id="org9428993"></a>

### trash

Trash the current item.

Default keybinding: `S-t`


<a id="orgf60fa37"></a>

### trash-local

`trash` for **Local Marks**

Default keybinding: `S-t`


<a id="org850a37e"></a>

### trash-global

`trash` for **Global Marks**

Default keybinding: `S-t`


<a id="org535b922"></a>

## filter

Set a filter to directory.

Example: `*` displays everything, `*.csv` displays only the csv files,
`*.png` displays only the png files


<a id="org11ab0df"></a>

## unfilter

Reset the appplied filter.


<a id="org05e352f"></a>

## refresh

Force refresh the current directory.

**NOTE: By default, Navi watches the directory for changes and loads
them, so there is no requirement to refresh the directory. This command
is there just in case something does not look right.**


<a id="org6965b44"></a>

## hidden-files

Toggles the hidden files.

Default keybinding: `C-h`

NOTE: Hidden files are those items whose name start with a period like
`.config`, `.gitignore` etc.


<a id="orgf7b3d48"></a>

## Panes


<a id="org7bb728b"></a>

### bookmark-pane

Opens the bookmarks list.


<a id="org0e4e747"></a>

### marks-pane

Opens the marks list.


<a id="orge9c5cc1"></a>

### messages-pane

Opens the messages list.


<a id="org902c400"></a>

### preview-pane

Toggles the preview pane.

Default keybinding: `C-p`

The preview pane handles previewing images (good number of formats) and
PDF documents (first page) `asynchronously`. This means that the
previewing experience will be seamless and without any lag. Navi uses
`ImageMagick` library under the hood for previewing images and therefore
any image formats supported my ImageMagick is supported by Navi.


<a id="orgf24acb3"></a>

## Misc


<a id="org3af06ca"></a>

### menu-bar

Toggles the menu bar.

Default keybinding: `C-m`


<a id="orgf2d7d38"></a>

### focus-path

Focuses the path widget and sets it in edit mode.

Default keybinding: `C-l`


<a id="org777a230"></a>

### item-property

Display the property of the currently focused item.


<a id="org46539d7"></a>

### toggle-header

Toggle the display of header information.


<a id="org39eb10e"></a>

### toggle-cycle

Toggle the cycle (last item to first item and vice-versa) during navigation.


<a id="orgfc342c4"></a>

## Shell Commands


<a id="org83a3276"></a>

### TODO shell-command

Run a shell command *synchronously* (blocking).

**NOTE: This blocks the main GUI thread until the command finished
executing**


<a id="orgc74b2a5"></a>

### TODO shell-command-async

Run a shell command *asynchronously* (non-blocking).

The running commands can be seen in the `Task Widget`


<a id="org9b87b9d"></a>

## Bookmarks


<a id="org6b21ff3"></a>

### bookmark-add

Add a new non-existing bookmark


<a id="orgcd41816"></a>

### bookmark-remove

Remove an existing bookmark


<a id="org17cdc75"></a>

### bookmark-edit

Edit an existing bookmark

-   Args:
    
    `title` - this tells Navi to edit the bookmark title `path` - this
    tells Navi to edit the bookmark directory that the bookmark points to


<a id="orgb7938fe"></a>

### bookmark-go

Go to the directory pointed by the bookmark

-   Args:
    
    `bookmark-name` - a unique bookmark name


<a id="orgab3d565"></a>

### bookmark-save

Save the bookmarks that have been added.

**NOTE: Saving of the bookmarks added is done when the application exits.
If you feel insecure about losing your bookmarks, then you can run this
command manually.**


<a id="org6a5c95a"></a>

## Searching Items

Searching is **Regular Expression** enabled. So you can pinpoint exactly
the file that you want to look for.


<a id="orgde316cd"></a>

### search

Searches the current directory for the search term

Default Keybinding: `/`


<a id="org78746fc"></a>

### search-next

Goes to the next best match for the search term

Default Keybinding: `n`


<a id="org03ddbd8"></a>

### search-prev

Goes to the previous best match for the search term

Default Keybinding: `S-n`


<a id="org210aca5"></a>

# Linux Only

Sorry, this software is built keeping in mind Linux and it's derivatives
only.


<a id="org10fab7c"></a>

# Configuration with Lua

Navi can be configured using lua. The default configuration file is
provided in this git repository. Navi looks for the configuration file
in the XDG standard config directory (`~/.config`). You can place your
configuration file in the `~/.config/navi/` directory. The configuration
file should be named `config.lua`.

Configuration includes all the settings that can be changed for Navi and
keybindings can also be changed.

**TL;DR: Navi configuration file (config.lua) should be placed at this
location ( `~/.config/navi/config.lua`)**

Copy and paste the following template config file into the configuration directory.

    settings = {
        ui = {
            preview_pane = {
                shown = false -- preview pane visibility
            },
    
            menu_bar = {
                shown = false -- menubar visibility
            },
    
            status_bar = {
                shown = false -- statusbar visibility
            },
    
            file_pane = {
                -- the key names (the names on the left hand side) should be one of name,
                -- permission, modified_date and size. The right hand side string represents
                -- the header titles
                columns = {
                    name = "NAME",
                    -- permission = "PERM",
                    -- modified_date = "Date",
                    -- size = "SIZE"
                },
                headers = false, -- headers visibility
                cycle = true, -- cycle from the last item to the first and vice-versa or no
            }
        }
    }


<a id="org07bf2b0"></a>

# TODO Scripting with Lua

Navi can be scripted to include custom actions by subscribing to hooks
and perform custom actions in response to those hooks.

