#include "AboutDialog.h"
#include "../parameters.h"
#include "../Data.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent, Qt::Dialog | Qt::CustomizeWindowHint)
{
	QLabel *image = new QLabel(this);
	image->setPixmap(QPixmap(":/images/deling_city.png"));

	QLabel *desc1 = new QLabel(QString("<h1 style=\"text-align:center\">%1</h1>").arg(PROG_FULLNAME) % tr("Par Jérôme &lt;myst6re&gt; Arzel <br/><a href=\"https://github.com/myst6re/deling/\">"
	                "github.com/myst6re/deling</a>"), this);
	desc1->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
	desc1->setTextFormat(Qt::RichText);
	desc1->setOpenExternalLinks(true);

	QLabel *desc2 = new QLabel(tr("Merci à :<ul style=\"margin:0\"><li>Aali</li>"
	                            "<li>Aladore384</li>"
	                            "<li>Asa</li>"
	                            "<li>Maki</li>"
	                            "<li>kruci</li></ul>"), this);
	desc2->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
	desc2->setTextFormat(Qt::RichText);
	desc2->setOpenExternalLinks(true);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
	buttonBox->addButton(QDialogButtonBox::Close);
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));

	QLabel *desc3 = new QLabel(QString("Qt %1").arg(QT_VERSION_STR), this);
	QPalette pal = desc3->palette();
	pal.setColor(QPalette::WindowText, QColor(0xAA, 0xAA, 0xAA));
	desc3->setPalette(pal);

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(desc1, 0, 0, 1, 2);
	layout->addWidget(desc2, 1, 0, 1, 2);
	layout->addWidget(image, 2, 0, 1, 2);
	layout->addWidget(desc3, 3, 0);
	layout->addWidget(buttonBox, 3, 1);
}
