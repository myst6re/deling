#include "TdwWidget.h"

TdwWidget::TdwWidget(QWidget *parent) :
	PageWidget(parent)
{
}

void TdwWidget::build()
{
	if(isBuilded())		return;

	tdwGrid = new TdwGrid(this);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(tdwGrid);
	layout->setContentsMargins(QMargins());

	PageWidget::build();
}

void TdwWidget::clear()
{
	if(!isFilled())		return;

	tdwGrid->clear();

	PageWidget::clear();
}

void TdwWidget::fill()
{
	if(!isBuilded())	build();
	if(isFilled())		clear();

	if(!hasData() || !data()->hasTdwFile()) return;

	tdwGrid->setTdwFile(data()->getTdwFile());

	PageWidget::fill();
}
