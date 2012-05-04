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

#include <QtGui>
#include "FF8Text.h"
#include "JsmFile.h"
#include "TdwFile.h"
#include "FF8Image.h"
#define NOWIN	65535
#define DARKGREY	0
#define GREY		1
#define YELLOW		2
#define RED			3
#define GREEN		4
#define BLUE		5
#define PURPLE		6
#define WHITE		7
#include "Data.h"

class TextPreview : public QWidget
{
	Q_OBJECT
public:
	explicit TextPreview(QWidget *parent=0);
	void clear();
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
signals:
	void positionChanged(QPoint);
private:
	void drawTextArea(QPainter *painter);
	QPoint realPos(const FF8Window &ff8Window);
	QList<FF8Window> ff8Windows;
	QByteArray ff8Text;
	int currentPage;
	int currentWin;
	QList<int> pagesPos;
	int maxW, maxH;
	static bool curFrame;
	bool useTimer, acceptMove, readOnly;
	QPoint moveStartPosition;

	static TdwFile *tdwFile;
	static int fontColor;
	static QImage fontImage;
	static QImage iconImage;
	static void letter(int *x, int *y, int charId, QPainter *painter, quint8 tableId=0);
	static void word(int *x, int *y, const QByteArray &charIds, QPainter *painter, quint8 tableId=0);
	static void setFontColor(int id);
	static QVector<QRgb> fontPalettes[8];
	static int calcFF8TextWidth(const QByteArray &ff8Text);
	static quint8 charWidth[6][224];
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
