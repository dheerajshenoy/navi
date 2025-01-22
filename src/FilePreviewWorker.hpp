#pragma once

#include "pch/pch_filepreviewworker.hpp"
#include "SyntaxHighlighterTS.hpp"

class FilePreviewWorker : public QObject {
    Q_OBJECT

public:
  explicit FilePreviewWorker(QObject *parent = nullptr) : QObject(parent) {}

void setMaxPreviewThreshold(const qint64 &maxThreshold) noexcept {
    m_max_preview_threshold = maxThreshold;
}

void setPreviewDimension(const int &width, const int &height) noexcept {
    m_height = height; m_width = width;
}

signals:
    void errorOccurred(const QString &errorMessage);
    void textPreviewReady(const QString &textContent,
                        const SyntaxHighlighterTS::Language &language);
    void clearPreview();

private:
    qint64 m_max_preview_threshold = 1e+7;
    int m_width, m_height;
};
