#pragma once
#include "pch/pch_questiondialog.hpp"

class QuestionDialog : public QDialog {
    Q_OBJECT

    public:
    enum ButtonConfiguration {
        YesNo,
        YesNoAll
    };

    enum Result {
        Yes = QDialog::Accepted + 1,
        YesAll,
        No,
        NoAll
    };

    QuestionDialog(const QString& title, const QString& text, const QString& extraDescription, ButtonConfiguration buttonConfig, QWidget* parent = nullptr)
    : QDialog(parent) {
        setWindowTitle(title);

        // Main text
        QLabel* textLabel = new QLabel(text);
        textLabel->setWordWrap(true);

        // Expandable details
        m_detailsButton = new QPushButton(tr("Show Details"));
        m_detailsButton->setCheckable(true);
        m_detailsButton->setChecked(false);

        m_extraDescription = new QTextEdit(extraDescription);
        m_extraDescription->setReadOnly(true);
        m_extraDescription->setVisible(false);

        // Button layout
        QHBoxLayout* buttonLayout = new QHBoxLayout;
        createButtons(buttonConfig, buttonLayout);

        // Main layout
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(textLabel);
        mainLayout->addWidget(m_detailsButton);
        mainLayout->addWidget(m_extraDescription);
        mainLayout->addLayout(buttonLayout);

        // Connections
        connect(m_detailsButton, &QPushButton::toggled, this, &QuestionDialog::toggleDetails);
    }

    QuestionDialog(const QString& title, const QString& text, ButtonConfiguration buttonConfig, QWidget* parent = nullptr)
    : QDialog(parent) {
        setWindowTitle(title);

        // Main text
        QLabel* textLabel = new QLabel(text);
        textLabel->setWordWrap(true);

        // Button layout
        QHBoxLayout* buttonLayout = new QHBoxLayout;
        createButtons(buttonConfig, buttonLayout);

        // Main layout
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(textLabel);
        mainLayout->addLayout(buttonLayout);

    }

    void toggleDetails(bool checked) noexcept {
        m_extraDescription->setVisible(checked);
        m_detailsButton->setText(checked ? tr("Hide Details") : tr("Show Details"));
        adjustSize();
    }

    void createButtons(ButtonConfiguration config, QHBoxLayout* layout) noexcept {
        switch (config) {
            case YesNo:
                addButton(layout, tr("Yes"), Result::Yes);
                addButton(layout, tr("No"), Result::No);
                break;
            case YesNoAll:
                addButton(layout, tr("Yes"), Result::Yes);
                addButton(layout, tr("Yes All"), Result::YesAll);
                addButton(layout, tr("No"), Result::No);
                addButton(layout, tr("No All"), Result::NoAll);
                break;
        }
    }

    void addButton(QHBoxLayout* layout, const QString& text, Result result) noexcept {
        QPushButton* btn = new QPushButton(text);
        connect(btn, &QPushButton::clicked, this, [this, result]() {
            done(static_cast<int>(result));
        });
        layout->addWidget(btn);
    }

    void closeEvent(QCloseEvent *e) noexcept {
        done(static_cast<int>(Result::No));
    }

private:
    QPushButton* m_detailsButton;
    QTextEdit* m_extraDescription;
};
