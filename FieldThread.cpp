#include "FieldThread.h"

void FieldThread::run()
{
    QByteArray chara_data;
    if(fieldArchive->openBG(field, chara_data)
            && field->hasBackgroundFile()) {
        QPixmap bg = field->getBackgroundFile()->background();

        emit background(bg);
    } else {
        emit background(FF8Image::errorPixmap());
    }
}
