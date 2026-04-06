//    SPDX-FileCopyrightText: 2009 - 2022 Arzel Jérôme <myst6re@gmail.com>
//    SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once


#include <QtWidgets>

#define COLOR_DISPLAY_MIN_CELL_SIZE   9
#define COLOR_DISPLAY_BORDER_WIDTH    1

class ColorDisplay : public QWidget
{
    Q_OBJECT
public:
    explicit ColorDisplay(QWidget *parent = nullptr);
    void setColors(const QList<QRgb> &colors);
    const QList<QRgb> &colors() const;
    bool isReadOnly() const;
    void setReadOnly(bool ro);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    inline int cellWidth() const {
        return cellWidth(width());
    }
    inline int cellHeight() const {
        return cellHeight(height());
    }
signals:
    void colorEdited(int id, QRgb value);
    void colorHovered(int id);
private:
    int colorId(const QPoint &pos) const;
    int cellWidth(int w) const;
    int cellHeight(int h) const;
    QSize cellSize() const;
    QList<QRgb> _colors;
    bool _ro, _hover;
protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};
