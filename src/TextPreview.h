/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2012 Arzel Jérôme <myst6re@gmail.com>
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#ifndef TEXTPREVIEW_H
#define TEXTPREVIEW_H

#include <QtWidgets>
#include "FF8Text.h"
#include "files/JsmFile.h"
#include "files/TdwFile.h"
#include "FF8Image.h"
#define NOWIN	65535
#include "Data.h"

class TextPreview : public QWidget
{
	Q_OBJECT
public:
	explicit TextPreview(QWidget *parent=0);
	void clear();
	static void reloadFont();
	void setWins(const QList<FF8Window> &windows, bool update=true);
	void resetCurrentWin();
	int getCurrentWin();
	FF8Window getWindow();
	void setReadOnly(bool);
	int getNbWin();
	void nextWin();
	void prevWin();
	void clearWin();
	void setText(const QByteArray &textData, bool reset=true);
	int getCurrentPage();
	int getNbPages();
	void nextPage();
	void prevPage();
	static void setFontImageAdd(TdwFile *tdwFile);
	static QImage getIconImage(int charId);
	void calcSize();
	inline QSize windowSize() const {
		return QSize(maxW, maxH);
	}
signals:
	void positionChanged(const QPoint &);
private:
	void drawTextArea(QPainter *painter);
	QPoint realPos(const FF8Window &ff8Window);
	void letter(int *x, int *y, int charId, QPainter *painter, quint8 tableId=0);
	void word(int *x, int *y, const QByteArray &charIds, QPainter *painter, quint8 tableId=0);

	QList<FF8Window> ff8Windows;
	QByteArray ff8Text;
	int currentPage;
	int currentWin;
	QList<int> pagesPos;
	int maxW, maxH;
	static bool curFrame;
	bool useTimer, acceptMove, readOnly;
	QPoint moveStartPosition;
	TdwFile::Color fontColor;

	static TdwFile *font;
	static TdwFile *tdwFile;
	static QImage iconImage;

	static int calcFF8TextWidth(const QByteArray &ff8Text);
	static const quint8 iconWidth[96];
	static const quint8 iconPadding[96];
	static const char *optimisedDuo[24];
	static QList<QByteArray> names;
	static int namesWidth[14];
	static QList<QByteArray> locations;
	static int locationsWidth[8];
protected:
	void paintEvent(QPaintEvent *event);
	void timerEvent(QTimerEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
};

#endif // TEXTPREVIEW_H
