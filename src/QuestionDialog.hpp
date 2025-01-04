#pragma once

#include <QDialog>
#include <QWidget>

class QuestionDialog : public QDialog {

public:
    QuestionDialog(QWidget *parent = nullptr);

    enum class Button {
        Yes = 0,
        No,
        YesToAll,
        NoToAll
    };

    void set_extra_desc(const QString &str) noexcept;


    inline void extra_desc_visible(const bool &state) noexcept {
        m_extra_desc_visible = state;
    }

    int exec() noexcept override;

private:

    bool m_extra_desc_visible = true;
};
