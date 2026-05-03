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
#include "files/PmdFile.h"
#include "files/PmpFile.h"

PmdFile::PmdFile()
	: File()
{
}

bool PmdFile::open(const QByteArray &pmd)
{
	this->pmd = pmd;
	modified = false;

	if(pmd.size() == 4 && pmd != "\x20\x20\x20\x20") {
		qWarning() << "Unknown pmd format!" << pmd.toHex();
	}

	if(pmd.size() != 4 && pmd.size() != 10016)
		qDebug() << "pmd size" << pmd.size();

	return true;
}

bool PmdFile::save(QByteArray &pmd) const
{
	pmd = this->pmd;

	return true;
}

const QByteArray &PmdFile::getPmdData() const
{
	return pmd;
}

void PmdFile::setPmdData(const QByteArray &pmd)
{
	this->pmd = pmd;
	modified = true;
}
