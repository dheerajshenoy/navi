#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QTableView>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QRegularExpression>
#include <QVariant>
#include <QLineEdit>
#include <QHeaderView>

struct Keybind {
    QString key;
    QString command;
    QString desc;
};

class KeyBindFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT

    public:
    KeyBindFilterProxyModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {}

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override {
        // Get the source model (KeyBindModel)
        QModelIndex index0 = sourceModel()->index(source_row, 0, source_parent);
        QModelIndex index1 = sourceModel()->index(source_row, 1, source_parent);
        QModelIndex index2 = sourceModel()->index(source_row, 2, source_parent);

        // Get the value for each column
        QString key = sourceModel()->data(index0).toString();
        QString command = sourceModel()->data(index1).toString();
        QString description = sourceModel()->data(index2).toString();

        // Create a regular expression based on the filter text
        QString filterText = filterRegularExpression().pattern();
        QRegularExpression regex(filterText, QRegularExpression::CaseInsensitiveOption);

        // Check if any of the columns match the filter
        if (regex.match(key).hasMatch() ||
            regex.match(command).hasMatch() ||
            regex.match(description).hasMatch()) {
            return true;
        }

        return false;
    }
};

class KeyBindModel : public QAbstractTableModel {
    Q_OBJECT

    public:
    KeyBindModel(QObject *parent = nullptr) : QAbstractTableModel(parent) {}

    // Set the list of keybinds
    void setKeyBinds(const QList<Keybind>& keyBinds) {
        beginResetModel();
        m_keyBinds = keyBinds;
        endResetModel();
    }

    // Row and column count
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return m_keyBinds.size();
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        return 3;  // key, command, description
    }

    // Data for the table view
    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid()) return QVariant();

        const Keybind &keyBind = m_keyBinds.at(index.row());

        switch (role) {
        case Qt::DisplayRole:
            switch (index.column()) {
            case 0:
                return keyBind.key;
            case 1:
                return keyBind.command;
            case 2:
                return keyBind.desc;
            default:
                return QVariant();
            }
        default:
            return QVariant();
        }
    }

    // Header data
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            switch (section) {
            case 0:
                return "Key";
            case 1:
                return "Command";
            case 2:
                return "Description";
            default:
                return QVariant();
            }
        }
        return QVariant();
    }

private:
    QList<Keybind> m_keyBinds;  // Stores the keybinds
};

class ShortcutsWidget : public QWidget {
Q_OBJECT
public:
  ShortcutsWidget(const QList<Keybind> &, QWidget *parent = nullptr);

    inline void setKeybinds(const QList<Keybind> &list) noexcept {
      m_model->setKeyBinds(list);
    }

    void show() noexcept {
        emit visibilityChanged(true);
        QWidget::show();
    }

    void hide() noexcept {
        emit visibilityChanged(false);
        QWidget::hide();
    }

signals:
    void visibilityChanged(const bool &state);

private:
    QVBoxLayout *m_layout = new QVBoxLayout();
    QTableView *m_table_view = new QTableView();
    KeyBindModel *m_model = new KeyBindModel();
    QLineEdit *m_line_edit = new QLineEdit();
    KeyBindFilterProxyModel *proxyModel = new KeyBindFilterProxyModel();
};