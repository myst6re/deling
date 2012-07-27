#include "HexLineEdit.h"

HexLineEdit::HexLineEdit(QWidget *parent) :
	QLineEdit(parent), _noEmit(false)
{
	connect(this, SIGNAL(textEdited(QString)), SLOT(emitDataEdited()));
}

HexLineEdit::HexLineEdit(const QByteArray &contents, QWidget *parent) :
	QLineEdit(parent), _noEmit(false)
{
	setData(contents);
}

QByteArray HexLineEdit::data() const
{
	return QByteArray::fromHex(text().toLatin1()).leftJustified(maxLength()/2, '\0', true);
}

void HexLineEdit::setData(const QByteArray &contents)
{
	_noEmit = true;
	setMaxLength(contents.size() * 2);
	setInputMask(QString(contents.size() * 2, 'H'));
	_noEmit = false;
	setText(contents.toHex());
	emit dataChanged(contents);
}

void HexLineEdit::emitDataEdited()
{
	if(_noEmit)		return;

	emit dataEdited(data());
}

QString HexLineEdit::text() const
{
	return QLineEdit::text();
}

void HexLineEdit::setText(const QString &text)
{
	QLineEdit::setText(text);
}

void HexLineEdit::setMaxLength(int maxLength)
{
	QLineEdit::setMaxLength(maxLength);
}

void HexLineEdit::setInputMask(const QString &inputMask)
{
	QLineEdit::setInputMask(inputMask);
}
