#ifndef SPECIALCHARACTERSDIALOG_H
#define SPECIALCHARACTERSDIALOG_H

#include <QtGui>
#include "widgets/TdwGrid.h"
#include "FF8Font.h"
#include "FF8Text.h"

class SpecialCharactersDialog : public QDialog
{
	Q_OBJECT
public:
	explicit SpecialCharactersDialog(QWidget *parent = 0);
	QString selectedCharacter() const;
private:
	QComboBox *tableSelect;
	TdwGrid *grid;
};

#endif // SPECIALCHARACTERSDIALOG_H
