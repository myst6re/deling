#ifndef FIELDTHREAD_H
#define FIELDTHREAD_H

#include <QtCore>
#include "FieldArchive.h"
#include "Field.h"

class FieldThread : public QThread
{
	Q_OBJECT
public:
    explicit FieldThread(FieldArchive *fieldArchive, Field *field, QObject *parent = 0)
        : QThread(parent), fieldArchive(fieldArchive), field(field) { }
    void run();
signals:
    void background(QPixmap);
private:
    explicit FieldThread(QObject *parent = 0) : QThread(parent) { }
    FieldArchive *fieldArchive;
    Field *field;
};

#endif // FIELDTHREAD_H
