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
#include "FF8Text.h"
#include "FF8Font.h"

FF8Text::FF8Text() :
	QString()
{
	setTables(::FF8Font::getCurrentConfigFont()->tables());
}

FF8Text::FF8Text(const QString &str) :
	QString(str)
{
	setTables(::FF8Font::getCurrentConfigFont()->tables());
}

FF8Text::FF8Text(const char *str) :
	QString(str)
{
	setTables(::FF8Font::getCurrentConfigFont()->tables());
}

FF8Text::FF8Text(const QByteArray &ba) :
	QString()
{
	setTables(::FF8Font::getCurrentConfigFont()->tables());
	append(fromFF8(ba));
}

FF8Text::FF8Text(const QByteArray &ba, const QList<QStringList> &tables) :
	QString()
{
	setTables(tables);
	append(fromFF8(ba));
}

QString FF8Text::fromFF8(const QByteArray &ff8str)
{
	QString ret, character;
	quint8 index, oldIndex, jp = tables.size() == 4;
	int size = ff8str.size();

	for(int i=0 ; i<size ; ++i) {
		index = (quint8)ff8str.at(i);
		if(index==0x00)						break;
		else if(index==0x01)
			ret.append("\n{NewPage}\n");
		else if(index==0x02)
			ret.append('\n');
		else if(index==0x03) {//{Name}
			if((++i) < size) {
				index = (quint8)ff8str.at(i);
				if(index>=0x30 && index<=0x3a)
					ret.append(names[index-0x30]);
				else if(index==0x40)
					ret.append(names[11]);
				else if(index==0x50)
					ret.append(names[12]);
				else if(index==0x60)
					ret.append(names[13]);
				else
					ret.append(QString("{x03%1}").arg(index, 2, 16, QChar('0')));
			}
			else
				ret.append("{x03}");
		}
		else if(index==0x04) {//{Var0}, {Var00} et {Varb0}
			if((++i) < size) {
				index = (quint8)ff8str.at(i);
				if(index>=0x20 && index<=0x27)
					ret.append(QString("{Var%1}").arg(index-0x20));
				else if(index>=0x30 && index<=0x37)
					ret.append(QString("{Var0%1}").arg(index-0x30));
				else if(index>=0x40 && index<=0x47)
					ret.append(QString("{Varb%1}").arg(index-0x40));
				else
					ret.append(QString("{x04%1}").arg(index, 2, 16, QChar('0')));
			}
			else
				ret.append("{x04}");
		}
		else if(index==0x06) {//{Color}
			if((++i) < size) {
				index = (quint8)ff8str.at(i);
				if(index>=0x20 && index<=0x2f)
					ret.append(colors[index-0x20]);
				else
					ret.append(QString("{x06%1}").arg(index, 2, 16, QChar('0')));
			}
			else
				ret.append("{x06}");
		}
		else if(index==0x09) {//{Wait000}
			if((++i) < size) {
				index = (quint8)ff8str.at(i);
				if(index>=0x20)
					ret.append(QString("{Wait%1}").arg(index-0x20, 3, 10, QChar('0')));
				else
					ret.append(QString("{x09%1}").arg(index, 2, 16, QChar('0')));
			}
			else
				ret.append("{x09}");
		}
		else if(index==0x0e) {//{Location}
			if((++i) < size) {
				index = (quint8)ff8str.at(i);
				if(index>=0x20 && index<=0x27)
					ret.append(locations[index-0x20]);
				else
					ret.append(QString("{x0e%1}").arg(index, 2, 16, QChar('0')));
			}
			else
				ret.append("{x0e}");
		}
		else if(jp && index>=0x19 && index<=0x1b) {//jp19, jp1a, jp1b
			if((++i) < size) {
				oldIndex = index;
				index = (quint8)ff8str.at(i);
				if(index>=0x20) {
					character = caract(index, oldIndex-0x18);
				} else {
					character = QString();
				}
				if(character.isEmpty()) {
					character = QString("{x%1%2}").arg(oldIndex, 2, 16, QChar('0')).arg(index, 2, 16, QChar('0'));
				}
				ret.append(character);
			} else {
				ret.append(QString("{x%1}").arg(index, 2, 16, QChar('0')));
			}
		}
		else if(index==0x1c) {//addJp
			if((++i) < size) {
				index = (quint8)ff8str.at(i);
				if(index>=0x20) {
					ret.append(QString("{Jp%1}").arg(index-0x20, 3, 10, QChar('0')));
				} else {
					ret.append(QString("{x1c%1}").arg(index, 2, 16, QChar('0')));
				}
			} else {
				ret.append("{x1c}");
			}
		}
		else if(index>=0x05 && index<=0x1f) {
			if((++i) < size)
				ret.append(QString("{x%1%2}").arg(index, 2, 16, QChar('0')).arg((quint8)ff8str.at(i), 2, 16, QChar('0')));
			else
				ret.append(QString("{x%1}").arg(index, 2, 16, QChar('0')));
		}
		else {
			character = caract(index);
			if(character.isEmpty()) {
				character = QString("{x%1}").arg(index, 2, 16, QChar('0'));
			}
			ret.append(character);
		}
	}

	return ret;
}

QByteArray FF8Text::toFF8(const QString &string, bool jp)
{
	FF8Text ff8Txt(string);
	QStringList fontL = ::FF8Font::fontList();
	ff8Txt.setTables(::FF8Font::font(fontL.at((int)jp))->tables());

	return ff8Txt.toFF8();
}

QByteArray FF8Text::toFF8() const
{
	QString string = *this;
	QByteArray ff8str;
	QChar comp/*, comp2*/;
	qsizetype stringSize = string.size();
	bool ok, ok2, jp = tables.size() == 4;
	ushort value, value2;

	for(qsizetype c = 0; c < stringSize; ++c)
	{
		comp = string.at(c);
		if(comp=='\n') {//\n{NewPage}\n,\n
			if(string.mid(c+1, 10).compare("{NewPage}\n", Qt::CaseInsensitive) == 0) {
				ff8str.append('\x01');
				c += 10;
			}
			else
				ff8str.append('\x02');
			continue;
		}
		else if(comp=='{') {
			QString rest = string.mid(c);
			for(quint8 i=0 ; i<11 ; ++i)
				if(rest.startsWith(names[i])) {//{Name}
					ff8str.append('\x03').append(char(0x30 + i));
					c += qstrlen(names[i])-1;
					goto end;
				}
			if(rest.startsWith(names[11])) {//{Angelo}
				ff8str.append("\x03\x40");
				c += qstrlen(names[11])-1;
				continue;
			}
			if(rest.startsWith(names[12])) {//{Griever}
				ff8str.append("\x03\x50");
				c += qstrlen(names[12])-1;
				continue;
			}
			if(rest.startsWith(names[13])) {//{Boko}
				ff8str.append("\x03\x60");
				c += qstrlen(names[13])-1;
				continue;
			}
			for(quint8 i=0 ; i<16 ; ++i) {
				if(rest.startsWith(colors[i], Qt::CaseInsensitive)) {//{Color}
					ff8str.append('\x06').append(char(0x20 + i));
					c += qstrlen(colors[i])-1;
					goto end;
				}
			}
			for(quint8 i=0 ; i<8 ; ++i) {
				if(rest.startsWith(locations[i], Qt::CaseInsensitive)) {//{Location}
					ff8str.append('\x0e').append(char(0x20 + i));
					c += qstrlen(locations[i])-1;
					goto end;
				}
			}
			if(rest.startsWith("{Var0", Qt::CaseInsensitive) && rest.at(6)=='}') {//{Var00}
				value = rest.mid(5,1).toUShort(&ok);
				if(ok && value<8) {
					ff8str.append('\x04');
					ff8str.append((char)(value+0x30));
					c += 6;
					continue;
				}
			}
			if(rest.startsWith("{Varb", Qt::CaseInsensitive) && rest.at(6)=='}') {//{Varb0}
				value = rest.mid(5,1).toUShort(&ok);
				if(ok && value<8) {
					ff8str.append('\x04');
					ff8str.append((char)(value+0x40));
					c += 6;
					continue;
				}
			}
			if(rest.startsWith("{Var", Qt::CaseInsensitive) && rest.at(5)=='}') {//{Var0}
				value = rest.mid(4,1).toUShort(&ok);
				if(ok && value<8) {
					ff8str.append('\x04');
					ff8str.append((char)(value+0x20));
					c += 5;
					continue;
				}
			}
			if(rest.startsWith("{Wait", Qt::CaseInsensitive) && rest.at(8)=='}') {//{Wait000}
				value = rest.mid(5,3).toUShort(&ok);
				if(ok) {
					ff8str.append('\x09');
					ff8str.append((char)(value+0x20));
					c += 8;
					continue;
				}
			}
			if(rest.startsWith("{Jp", Qt::CaseInsensitive) && rest.at(6)=='}') {//{Jp000}
				value = rest.mid(3,3).toUShort(&ok);
				if(ok) {
					ff8str.append('\x1c');
					ff8str.append((char)(value+0x20));
					c += 6;
					continue;
				}
			}

			rest.truncate(4);
			for(quint16 i=0xe8 ; i<=0xff ; ++i)
			{
				if(QString::compare(rest, caract(i))==0)
				{
					ff8str.append((char)i);
					c += 3;
					goto end;
				}
			}

			if(string.at(c+1)=='x') {
				if(string.at(c+4)=='}') {//{xff}
					value = string.mid(c+2,2).toUShort(&ok,16);
					if(ok) {
						ff8str.append((char)value);
						c += 4;
						continue;
					}
				}
				else if(string.at(c+6)=='}') {//{xffff}
					value = string.mid(c+2,2).toUShort(&ok,16);
					value2 = string.mid(c+4,2).toUShort(&ok2,16);
					if(ok && ok2) {
						ff8str.append((char)value);
						ff8str.append((char)value2);
						c += 6;
						continue;
					}
				}
			}

			continue;// character '{' is not in ff8 table
		}
//		else if(c+1<stringSize) {//"in","e ","ne","to","re","HP","l ","ll","GF","nt","il","o ","ef","on"," w"," r","wi","fi","EC","s ","ar","FE"," S","ag"
//			comp2 = string.at(c+1);
//			if(comp=='i' && comp2=='n') {		ff8str.append('\xe8');++c;continue;		}
//			else if(comp=='e' && comp2==' ') {	ff8str.append('\xe9');++c;continue;		}
//			else if(comp=='n' && comp2=='e') {	ff8str.append('\xea');++c;continue;		}
//			else if(comp=='t' && comp2=='o') {	ff8str.append('\xeb');++c;continue;		}
//			else if(comp=='r' && comp2=='e') {	ff8str.append('\xec');++c;continue;		}
//			else if(comp=='H' && comp2=='P') {	ff8str.append('\xed');++c;continue;		}
//			else if(comp=='l' && comp2==' ') {	ff8str.append('\xee');++c;continue;		}
//			else if(comp=='l' && comp2=='l') {	ff8str.append('\xef');++c;continue;		}
//			else if(comp=='G' && comp2=='F') {	ff8str.append('\xf0');++c;continue;		}
//			else if(comp=='n' && comp2=='t') {	ff8str.append('\xf1');++c;continue;		}
//			else if(comp=='i' && comp2=='l') {	ff8str.append('\xf2');++c;continue;		}
//			else if(comp=='o' && comp2==' ') {	ff8str.append('\xf3');++c;continue;		}
//			else if(comp=='e' && comp2=='f') {	ff8str.append('\xf4');++c;continue;		}
//			else if(comp=='o' && comp2=='n') {	ff8str.append('\xf5');++c;continue;		}
//			else if(comp==' ' && comp2=='w') {	ff8str.append('\xf6');++c;continue;		}
//			else if(comp==' ' && comp2=='r') {	ff8str.append('\xf7');++c;continue;		}
//			else if(comp=='w' && comp2=='i') {	ff8str.append('\xf8');++c;continue;		}
//			else if(comp=='f' && comp2=='i') {	ff8str.append('\xf9');++c;continue;		}
//			else if(comp=='E' && comp2=='C') {	ff8str.append('\xfa');++c;continue;		}
//			else if(comp=='s' && comp2==' ') {	ff8str.append('\xfb');++c;continue;		}
//			else if(comp=='a' && comp2=='r') {	ff8str.append('\xfc');++c;continue;		}
//			else if(comp=='F' && comp2=='E') {	ff8str.append('\xfd');++c;continue;		}
//			else if(comp==' ' && comp2=='S') {	ff8str.append('\xfe');++c;continue;		}
//			else if(comp=='a' && comp2=='g') {	ff8str.append('\xff');++c;continue;		}
//		}

		for(quint16 i=0x20 ; i<=0xff ; ++i)
		{
			if(QString::compare(comp, caract(i))==0)
			{
				ff8str.append((char)i);
				goto end;
			}
		}

		if(jp) {
			for(quint8 table=1 ; table<4 ; ++table)
			{
				for(quint16 i=0x20 ; i<=0xff ; ++i)
				{
					if(QString::compare(comp, caract(i, table))==0)
					{
						switch(table) {
						case 1:
							ff8str.append('\x19');
							break;
						case 2:
							ff8str.append('\x1a');
							break;
						case 3:
							ff8str.append('\x1b');
							break;
						}
						ff8str.append(char(i));
						goto end;
					}
				}
			}
		}

		end:;
	}

	return ff8str;
}

QString FF8Text::caract(quint8 ord, quint8 table) const
{
	if(table < tables.size()) {
		return tables.at(table).at(ord-0x20);
	}
	else {
		return QString();
	}
}

QString FF8Text::getCaract(quint8 ord, quint8 table, bool jp)
{
	FF8Text ff8Txt;
	QStringList fontL = ::FF8Font::fontList();
	ff8Txt.setTables(::FF8Font::font(fontL.at((int)jp))->tables());

	return ff8Txt.caract(ord, table);
}

void FF8Text::setTables(const QList<QStringList> &tables)
{
	this->tables = tables;
}

const char *FF8Text::names[14] =
{
	"{Squall}","{Zell}","{Irvine}","{Quistis}","{Rinoa}","{Selphie}","{Seifer}","{Edea}","{Laguna}","{Kiros}","{Ward}","{Angelo}","{Griever}","{Boko}"
};

const char *FF8Text::colors[16] =
{
	"{Darkgrey}","{Grey}","{Yellow}","{Red}","{Green}","{Blue}","{Purple}","{White}",
	"{DarkgreyBlink}","{GreyBlink}","{YellowBlink}","{RedBlink}","{GreenBlink}","{BlueBlink}","{PurpleBlink}","{WhiteBlink}"
};

const char *FF8Text::locations[8] =
{
	"{Galbadia}","{Esthar}","{Balamb}","{Dollet}","{Timber}","{Trabia}","{Centra}","{Horizon}"
};
