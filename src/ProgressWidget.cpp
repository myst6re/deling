/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2024 Arzel Jérôme <myst6re@gmail.com>
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
#include "ProgressWidget.h"

ProgressWidget::ProgressWidget(const QString &labelText, ButtonLabel buttonLabel, QWidget *parent) :
	ArchiveObserverProgressDialog(&_progress),
	_progress(parent, Qt::Dialog | Qt::WindowCloseButtonHint),
	_taskBarButton(parent)
{
	_progress.setLabelText(labelText);
	_progress.setCancelButtonText(buttonLabel == Cancel ? tr("Cancel") : tr("Stop"));
	_progress.setRange(0, 0);
	_progress.setWindowModality(Qt::WindowModal);
	_progress.show();
	_taskBarButton.setState(QTaskBarButton::Invisible);
}

ProgressWidget::~ProgressWidget()
{
	_taskBarButton.setState(QTaskBarButton::Invisible);
}

void ProgressWidget::setObserverMaximum(unsigned int max)
{
	ArchiveObserverProgressDialog::setObserverMaximum(max);
	_taskBarButton.setRange(0, max);
	_taskBarButton.reset();
	_taskBarButton.setState(QTaskBarButton::Normal);
}

void ProgressWidget::setObserverValue(int value)
{
	ArchiveObserverProgressDialog::setObserverValue(value);
	_taskBarButton.setValue(value);
}
