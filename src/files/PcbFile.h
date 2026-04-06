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
#pragma once

#include "files/File.h"
#include <QRgb>

class PcbFile : public File
{
public:
	PcbFile();
	bool open(const QByteArray &data);
	bool save(QByteArray &data) const;
	inline QString filterText() const {
		return QObject::tr("Field model color alteration file (*.pcb)");
	}
	inline const QMap<QString, QRgb> &modelLightColors() const {
		return _modelLightColors;
	}
	inline QRgb defaultModelLightColor() const {
		return _order.isEmpty() ? qRgb(0x80, 0x80, 0x80) : _modelLightColors.value(_order.first());
	}
	inline QRgb modelLightColor(const QString &modelName) const {
		return _modelLightColors.value(modelName, defaultModelLightColor());
	}
	void setModelLightColors(const QMap<QString, QRgb> &modelLightColors, const QStringList &order);
	void setModelLightColors(const QList< QPair<QString, QRgb> > &modelLightColors);
	void setDefaultModelLightColor(QRgb color);
private:
	QMap<QString, QRgb> _modelLightColors;
	QStringList _order;
};
