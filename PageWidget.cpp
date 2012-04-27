#include "PageWidget.h"

PageWidget::PageWidget(QWidget *parent) :
	QWidget(parent), builded(false), filled(false), readOnly(false)
{
}

bool PageWidget::isBuilded() const
{
	return builded;
}

void PageWidget::build()
{
	builded = true;
	setReadOnly(readOnly);
}

bool PageWidget::isFilled() const
{
	return filled;
}

void PageWidget::fill()
{
	if(!isBuilded())	build();

	setEnabled(true);
	filled = true;
}

void PageWidget::clear()
{
	setEnabled(false);
	filled = false;
}

void PageWidget::setReadOnly(bool ro)
{
	readOnly = ro;
}

bool PageWidget::isReadOnly() const
{
	return readOnly;
}
