//    SPDX-FileCopyrightText: 2009 - 2022 Arzel Jérôme <myst6re@gmail.com>
//    SPDX-FileCopyrightText: 2022 Chris Rizzitello <sithlord48@gmail.com>
//    SPDX-License-Identifier: LGPL-3.0-or-later

#include "ColorDisplay.h"

#include <QColorDialog>
#include <QMouseEvent>
#include <QPainter>

ColorDisplay::ColorDisplay(QWidget *parent)
    : QWidget(parent)
    , _ro(false)
    , _hover(false)
{
}

QSize ColorDisplay::sizeHint() const
{
    return minimumSizeHint();
}

QSize ColorDisplay::minimumSizeHint() const
{
    const qreal scale = devicePixelRatio();
    int colorCount = std::max<int>(1, _colors.size());
    int w = (COLOR_DISPLAY_MIN_CELL_SIZE * scale + COLOR_DISPLAY_BORDER_WIDTH) * colorCount + COLOR_DISPLAY_BORDER_WIDTH;
    int h = COLOR_DISPLAY_MIN_CELL_SIZE * scale + COLOR_DISPLAY_BORDER_WIDTH * 2;
    return QSize(w, h);
}

void ColorDisplay::setColors(const QList<QRgb> &colors)
{
    _colors = colors;
    setMouseTracking(_colors.size() > 1);
    update();
    if (!_colors.isEmpty())
        setToolTip(QColor(_colors.first()).name());
}

const QList<QRgb> &ColorDisplay::colors() const
{
    return _colors;
}

bool ColorDisplay::isReadOnly() const
{
    return _ro;
}

void ColorDisplay::setReadOnly(bool ro)
{
    _ro = ro;
}

int ColorDisplay::cellWidth(int width) const
{
    QSize minSize = minimumSizeHint();
    if (_colors.isEmpty()) {
        return cellHeight(width);
    }
    // Ensure current widget width respect minimum size hint
    int w = std::max<int>(width, minSize.width());
    return (w - (_colors.size() + 1) * COLOR_DISPLAY_BORDER_WIDTH) / _colors.size();
}

int ColorDisplay::cellHeight(int height) const
{
    QSize minSize = minimumSizeHint();
    // Ensure current widget width respect minimum size hint
    int h = std::max<int>(height, minSize.height());
    return h - 2 * COLOR_DISPLAY_BORDER_WIDTH;
}

QSize ColorDisplay::cellSize() const
{
    return QSize(cellWidth(width()), cellHeight(height()));
}

void ColorDisplay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setPen(palette().color(QPalette::Shadow));

    // Colors
    const QSize cellS = cellSize();
    const int size = std::max<int>(_colors.size(), 1),
              cellFullWidth = cellS.width() + COLOR_DISPLAY_BORDER_WIDTH,
              cellFullHeight = cellS.height() + COLOR_DISPLAY_BORDER_WIDTH;
    for (int i = 0; i < size; ++i) {
        const int x = i * cellFullWidth;
        painter.drawRect(x, 0, cellFullWidth, cellFullHeight);
        QColor color;
        if (_colors.isEmpty()) {
            color = Qt::transparent;
        } else if (isEnabled()) {
            color = QColor(_colors.at(i));
        } else {
            const int gray = qGray(_colors.at(i));
            color = QColor(gray, gray, gray);
        }
        painter.fillRect(x + COLOR_DISPLAY_BORDER_WIDTH,
                         COLOR_DISPLAY_BORDER_WIDTH,
                         cellS.width(), cellS.height(), color);
    }
    // Red frame
    if (isEnabled() && !isReadOnly() && _hover) {
        painter.setPen(palette().color(QPalette::Mid));
        const QPoint cursorPos = mapFromGlobal(cursor().pos());
        const int x = colorId(cursorPos) * cellFullWidth;
        painter.drawRect(x, 0, cellFullWidth, cellFullHeight);
    }
}

int ColorDisplay::colorId(const QPoint &pos) const
{
    return std::min<int>(pos.x() / (cellWidth() + COLOR_DISPLAY_BORDER_WIDTH), _colors.size() - 1);
}

void ColorDisplay::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event)
    _hover = true;
    update();
}

void ColorDisplay::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    _hover = false;
    update();
}

void ColorDisplay::mouseMoveEvent(QMouseEvent *event)
{
    update();
    Q_EMIT colorHovered(colorId(event->pos()));
}

void ColorDisplay::mouseReleaseEvent(QMouseEvent *event)
{
    if (isReadOnly())
        return;

    const int colorIndex = colorId(event->pos());
    if (colorIndex >= _colors.size())
        return;
    QColor color = QColorDialog::getColor(_colors.at(colorIndex), this, tr("Choose a new color"));
    if (color.isValid()) {
        _colors.replace(colorIndex, color.rgb());
        Q_EMIT colorEdited(colorIndex, color.rgb());
    }
    update();
}
