#ifndef HEXLINEEDIT_H
#define HEXLINEEDIT_H

#include <QtGui>

class HexLineEdit : public QLineEdit
{
	Q_OBJECT
public:
	explicit HexLineEdit(QWidget *parent=0);
	explicit HexLineEdit(const QByteArray &contents, QWidget *parent=0);
	QByteArray data() const;
public slots:
	void setData(const QByteArray &contents);
signals:
	void dataChanged(const QByteArray &data);
	void dataEdited(const QByteArray &data);
private:
	QString text() const;
	void setMaxLength(int maxLength);
	void setInputMask(const QString &inputMask);

	bool _noEmit;
private slots:
	void emitDataEdited();
	void setText(const QString &);
};

#endif // HEXLINEEDIT_H
