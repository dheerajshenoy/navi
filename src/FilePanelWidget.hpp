#pragma once

#include <QSplitter>
#include "FilePanel.hpp"
#include "PreviewPanel.hpp"
#include "Statusbar.hpp"
#include "Inputbar.hpp"
#include "sol/sol.hpp"
#include <QVBoxLayout>

class FilePanelWidget : public QWidget {
Q_OBJECT
public:
    FilePanelWidget(Statusbar *sb, Inputbar *ib, const QString &initDirectory = "~",
                    QWidget *parent = nullptr);

    FilePanel *filePanel() { return m_file_panel; }
    PreviewPanel *previewPanel() { return m_preview_panel; }
    QSplitter *splitter() { return m_splitter; }

    FilePanelWidget *clone(Statusbar *sb, Inputbar *ib,
                           const QString &initDirectory = "~", QWidget *parent = nullptr) {
        FilePanelWidget *newWidget =
            new FilePanelWidget(sb, ib, initDirectory, parent);
        applyConfiguration(newWidget);
        return newWidget;
    }
    void SaveConfiguration(const sol::table &confTable) noexcept { m_file_pane_table = confTable; }

signals:
    void marksListChanged();

private:
    void applyConfiguration(FilePanelWidget *widget) noexcept;
    FilePanel *m_file_panel = nullptr;
    QSplitter *m_splitter = nullptr;
    PreviewPanel *m_preview_panel = nullptr;
    QVBoxLayout *m_layout = nullptr;
    sol::table m_file_pane_table;
};