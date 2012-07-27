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
#include "files/PmdFile.h"

PmdFile::PmdFile()
	: File()
{
}

bool PmdFile::open(const QByteArray &pmd)
{
	this->pmd = pmd;

	return true;
}

bool PmdFile::save(QByteArray &pmd)
{
	pmd = this->pmd;
	modified = false;

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
