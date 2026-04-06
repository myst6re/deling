/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2026 Arzel Jérôme <myst6re@gmail.com>
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
#include "files/PcbFile.h"

PcbFile::PcbFile()
	: File()
{
}

bool PcbFile::open(const QByteArray &data)
{
	const char *constData = data.constData();
	int count = qMin(data.size() / 8, 64);

	_modelLightColors.clear();
	_order.clear();

	for (int i = 0; i < count; ++i) {
		QByteArray nameBA(constData + i * 8, 4);
		QRgb lightColor = qRgb(constData[i * 8 + 4 + 1], constData[i * 8 + 4 + 2], constData[i * 8 + 4 + 3]);
		QString modelName = QString::fromLatin1(nameBA, qstrnlen(nameBA, 4));
		_modelLightColors.insert(modelName, lightColor);
		_order.append(modelName);
	}
	modified = false;

	return true;
}

bool PcbFile::save(QByteArray &data) const
{
	quint32 def = defaultModelLightColor();

	for (const QString &modelName: _order) {
		data.append(modelName.toLatin1().leftJustified(4, '\0', true));
		QRgb lightColor = _modelLightColors.value(modelName, def);
		data.append('\0');
		data.append(char(qRed(lightColor)));
		data.append(char(qGreen(lightColor)));
		data.append(char(qBlue(lightColor)));
	}

	return true;
}

void PcbFile::setModelLightColors(const QMap<QString, quint32> &modelLightColors, const QStringList &order)
{
	_modelLightColors = modelLightColors;
	_order = order;
	modified = true;
}

void PcbFile::setModelLightColors(const QList< QPair<QString, QRgb> > &modelLightColors)
{
	_order.clear();
	_modelLightColors.clear();
	for (const QPair<QString, QRgb> &pair: modelLightColors) {
		_order.append(pair.first);
		_modelLightColors.insert(pair.first, pair.second);
	}
	modified = true;
}

void PcbFile::setDefaultModelLightColor(quint32 color)
{
	_modelLightColors["def"] = color;

	if (!_order.contains("def")) {
		_order.prepend("def");
	}
	modified = true;
}
