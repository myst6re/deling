#ifndef SOUNDWIDGET_H
#define SOUNDWIDGET_H

#include "widgets/PageWidget.h"

class SoundWidget : public PageWidget
{
	Q_OBJECT
public:
	SoundWidget();
	void clear();
	void setReadOnly(bool ro);
	void fill();
	inline QString tabName() const { return tr("Sons"); }
private slots:
	void setCurrentSound(int id);
	void editSfxValue(double v);
	void exportAkao();
	void importAkao();
private:
	void build();
	void fillList(int count);

	QListWidget *list1;
	// Sfx
	QWidget *sfxGroup;
	QDoubleSpinBox *sfxValue;
	// AkaoList
	QWidget *akaoGroup;
	QPushButton *exportButton, *importButton;

};

#endif // SOUNDWIDGET_H
