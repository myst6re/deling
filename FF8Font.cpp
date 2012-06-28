#include "FF8Font.h"

FF8Font::FF8Font(TdwFile *tdw, const QByteArray &txtFileData) :
	tdw(tdw)
{
	openTxt(QString::fromUtf8(txtFileData.data()));
}

bool FF8Font::isValid() const
{
	return !tables.isEmpty() && !tdw->isNull();
}

const QString &FF8Font::name() const
{
	return _name;
}

void FF8Font::openTxt(const QString &data)
{
	qDebug() << "openTxt";
	int tableCount = tdw->tableCount();
	QStringList lines = data.split(QRegExp("\\s*(\\r\\n|\\r|\\n)\\s*"), QString::SkipEmptyParts);
	QRegExp nameRegExp("#NAME\\s+(\\S.*)"), letterRegExp("\\s*\"([^\"]*|\\\"*)\"\\s*,?\\s*");
	// nameRegExp:		#NAME blah blah blah
	// letterRegExp:	"Foo", "Foo","Foo"
	QStringList table;

	if(tableCount < 1) {
		qWarning() << "invalid tdwFile!";
		return;
	}

	foreach(const QString &line, lines) {
		if(line.startsWith("#")) {
			if(nameRegExp.indexIn(line) != -1) {
				QStringList capturedTexts = nameRegExp.capturedTexts();
				_name = capturedTexts.at(1).trimmed();
				qDebug() << _name;
			}
		}
		else {
			int offset=0;
			while((offset = letterRegExp.indexIn(line, offset)) != -1) {
				QStringList capturedTexts = letterRegExp.capturedTexts();
				table.append(capturedTexts.at(1));
				offset += capturedTexts.first().size();

				if(table.size() == 224) {
					tables.append(table);
					if(tables.size() > tableCount) {
						//print();
						return;
					}
					table = QStringList();
				}
			}
		}
	}

	if(!table.isEmpty()) {
		if(table.size() < 224) {
			for(int i=table.size() ; i<224 ; ++i) {
				table.append(QString());
			}
		}

		tables.append(table);
	}

	//print();
}

void FF8Font::print()
{
	int tid=1;
	foreach(const QStringList &t, tables) {
		qDebug() << QString("table %1").arg(tid++).toLatin1().data();
		for(int j=0 ; j<14 ; ++j) {
			QString buf;
			for(int i=0 ; i<16 ; ++i) {
				buf += QString("\"%1\",").arg(t[j*16 + i]);
			}
			qDebug() << buf.toLatin1().data();
		}
	}
}
