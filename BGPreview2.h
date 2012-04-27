#ifndef BGPREVIEW2_H
#define BGPREVIEW2_H

#include <QtGui>

class BGPreview2 : public QLabel
{
    Q_OBJECT
public:
	explicit BGPreview2(QWidget *parent = 0);
	void setName(const QString &);
private:
	bool showSave;
	QString name;
	void savePixmap();
protected:
	void paintEvent(QPaintEvent *);
	void enterEvent(QEvent *);
	void leaveEvent(QEvent *);
	void mousePressEvent(QMouseEvent *);
};

#endif // BGPREVIEW2_H
