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

//	if(pmd.size() == 10016) {
//		QFile debug("pmp/_debug_"+PmpFile::currentFieldName+".txt");
//		debug.open(QIODevice::WriteOnly | QIODevice::Truncate);
//		const char *constPmd = pmd.constData();

//		for(int i=0 ; i<16 ; ++i) {
//			for(int j=0 ; j<16 ; ++j) {
//				qint16 zz1, zz2, zz3;
//				memcpy(&zz1, &constPmd[i*372 + j*20], 2);
//				memcpy(&zz2, &constPmd[i*372 + j*20 + 2], 2);
//				memcpy(&zz3, &constPmd[i*372 + j*20 + 4], 2);
//				debug.write(QString("%1 %2 %3 ").arg(zz1, 4, 10, QChar('0')).arg(zz2, 4, 10, QChar('0')).arg(zz3, 4, 10, QChar('0')).toLatin1());
//				for(int k=3 ; k<10 ; ++k) {
//					debug.write(QString("%1|%2 ").arg((quint8)pmd.at(i*372 + j*20 + k*2), 3, 10, QChar('0')).arg((quint8)pmd.at(i*372 + j*20 + k*2 + 1), 3, 10, QChar('0')).toLatin1());
//				}
//				debug.write("\n");
//			}
//			debug.write(pmd.mid(16*20 + i*372, 52).toHex());
//			debug.write("\n\n");
//		}
//		debug.close();
//	}

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
