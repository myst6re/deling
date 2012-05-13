#include "TdwGrid.h"

TdwGrid::TdwGrid(QWidget *parent) :
	QWidget(parent), tdwFile(0), _currentTable(0)
{
	setFixedSize(16 * (12 + 1 + 1*2) + 1, 14 * (12 + 1 + 1*2) + 1);
}

TdwGrid::~TdwGrid()
{
}

void TdwGrid::setTdwFile(TdwFile *tdwFile)
{
	this->tdwFile = tdwFile;
	update();
}

void TdwGrid::clear()
{
	this->tdwFile = 0;
	update();
}

int TdwGrid::currentTable() const
{
	return _currentTable;
}

void TdwGrid::setCurrentTable(int currentTable)
{
	_currentTable = currentTable;
}

void TdwGrid::paintEvent(QPaintEvent *)
{
	const int lineCountV=17, lineCountH=15, charaSize=12, padding=1, cellSize=charaSize+1+padding*2;
	QLine linesV[lineCountV], linesH[lineCountH];

	for(int i=0 ; i<lineCountV ; ++i) {
		linesV[i].setPoints(QPoint(i*cellSize, 0), QPoint(i*cellSize, height()));
	}

	for(int i=0 ; i<lineCountH ; ++i) {
		linesH[i].setPoints(QPoint(0, i*cellSize), QPoint(width(), i*cellSize));
	}

	QPainter p(this);

	if(isEnabled()) {
		p.setBrush(Qt::black);
		p.drawRect(0, 0, width(), height());
	}

	p.setPen(Qt::gray);

	p.drawLines(linesV, lineCountV);
	p.drawLines(linesH, lineCountH);

	if(tdwFile) {
		int x2=0, y2=0, charCount=tdwFile->charCount();

		for(int i=0 ; i<charCount ; ++i) {
			p.drawImage(QPoint(1+padding+x2*cellSize, 1+padding+y2*cellSize), tdwFile->letter(i, 0, true));
			x2++;
			if(x2 == 16) {
				++y2;
				x2 = 0;
			}
		}
	}
}


