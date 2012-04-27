#ifndef PLAINTEXTEDIT_H
#define PLAINTEXTEDIT_H

#include <QtGui>

class PlainTextEdit : public QWidget
{
	Q_OBJECT
public:
	explicit PlainTextEdit(QWidget *parent = 0);
	QPlainTextEdit *textEdit();
signals:
	
public slots:
private:
	QPlainTextEdit *_textEdit;
protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual bool eventFilter(QObject *obj, QEvent *event);
};

#endif // PLAINTEXTEDIT_H
