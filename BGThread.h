#ifndef BGTHREAD_H
#define BGTHREAD_H

#include <QtGui>



class BGThread : public QThread
{
	Q_OBJECT
public:
	void run();
	QByteArray mapData;
	QByteArray mimData;
signals:
	void BG(QPixmap);
private:
	QPixmap type1();
	QPixmap type2();
};

#endif // BGTHREAD_H
