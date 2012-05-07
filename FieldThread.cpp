#include "FieldThread.h"

void FieldThread::run()
{
	if(fieldArchive->openBG(field)
            && field->hasBackgroundFile()) {
        QPixmap bg = field->getBackgroundFile()->background();

        emit background(bg);
    } else {
        emit background(FF8Image::errorPixmap());
    }
}
