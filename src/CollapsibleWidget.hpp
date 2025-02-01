#pragma once

#include <QFrame>
#include <QGridLayout>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QToolButton>
#include <QWidget>
#include <QPropertyAnimation>

class CollapsibleWidget : public QWidget {
    Q_OBJECT

    private:
    QGridLayout mainLayout;
    QToolButton toggleButton;
    QFrame headerLine;
    QParallelAnimationGroup toggleAnimation;
    QScrollArea contentArea;
    int animationDuration{300};

public:
    explicit CollapsibleWidget(const QString & title = QString(),
                               const int animationDuration = 300,
                               QWidget *parent = nullptr);
    void setContentLayout(QLayout *layout) noexcept;
};
