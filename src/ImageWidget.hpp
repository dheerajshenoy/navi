#pragma once

#include <QWidget>
#include <QPainter>
#include <QImage>

class ImageWidget : public QWidget {
    Q_OBJECT

    public:
  explicit ImageWidget(QWidget *parent = nullptr) : QWidget(parent) {}

    // Set the raw image data for rendering
    void setImage(const QImage &image) noexcept {
        m_image = image;

        // Resize the widget to fit the new image dimensions
        setFixedSize(this->width(), this->height());

        // Request a repaint with the new data
        this->update();
    }

    void clear() noexcept {
        m_image = QImage();
        this->update();
    }

    inline void rotate(const qreal &angle = 0) noexcept {
        m_rotationAngle = angle;
        QTransform transform;
        transform.rotate(angle);
        m_image = m_image.transformed(transform, Qt::SmoothTransformation);
        this->update();
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);

        if (m_image.isNull())
            return;

        QPainter painter(this);

        // Draw the image at the top-left corner (0, 0)
        painter.drawImage(0, 0, m_image);
    }

private:
    QImage m_image;
    qreal m_rotationAngle;
};
