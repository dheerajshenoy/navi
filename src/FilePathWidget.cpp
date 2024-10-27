#include "FilePathWidget.hpp"

FilePathWidget::FilePathWidget(QWidget *parent)
: QWidget(parent) {
    this->setLayout(m_layout);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    m_path_line->setReadOnly(true);
    m_layout->addWidget(m_path_line);
    m_path_line->setFrame(false);

    // Set up file system model for directory completion
    m_completer_model->setFilter(QDir::AllDirs | QDir::Hidden | QDir::NoDotAndDotDot);  // Show only directories
    m_completer_model->setRootPath(QDir::rootPath());
    m_completer = new CustomCompleter(m_completer_model, this);
    m_path_line->setCompleter(m_completer);

    // connect(m_path_line, &FilePathLineEdit::textChanged, this,  [&](const QString &text) {
    //     QString expandedText = text;
    //     if (text.startsWith("~")) {
    //         expandedText.replace(0, 1, QDir::homePath());  // Replace ~ with home directory
    //         m_completer->setCompletionPrefix(expandedText);
    //         m_completer->splitPath(expandedText);
    //     } else {
    //         m_completer->setCompletionPrefix(text);
    //     }
    //     qDebug() << m_completer->completionPrefix();
    // });

    connect(m_path_line, &QLineEdit::returnPressed, this, [&]() {
        m_path_line->setReadOnly(true);
        emit directoryChangeRequested(m_path_line->text());

    });
}

void FilePathWidget::setCurrentDir(const QString& path) noexcept {
    m_path_line->setText(path);
    m_completer_model->setRootPath(path);
}

FilePathWidget::~FilePathWidget() {}