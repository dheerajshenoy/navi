#pragma once

#include <QSortFilterProxyModel>
#include <QRegularExpression>

// Custom Proxy Model for Orderless Matching
class OrderlessFilterModel : public QSortFilterProxyModel {
public:
    explicit OrderlessFilterModel(QObject *parent = nullptr)
    : QSortFilterProxyModel(parent) {}

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override {
        const QString itemText =
            sourceModel()
              ->data(sourceModel()->index(sourceRow, 0, sourceParent))
              .toString();

        const QStringList words = itemText.split("-", Qt::SkipEmptyParts);

        // Check if all words in the filter are present in itemText
        for (const QString& word : words) {
            if (!itemText.contains(word, Qt::CaseInsensitive)) {
                return false;
            }
        }
        return true;
    }
};