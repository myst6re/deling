#ifndef FIELDPS_H
#define FIELDPS_H

#include "Field.h"

class FieldPS : public Field
{
public:
	enum MimSections {
		Pvp, Mim, Tdw, Pmp
	};
	enum DatSections {
		Inf, Ca, Id, Map, Msk, Rat, Mrt, AKAO, Msd, Pmd, Jsm, Last
	};

	FieldPS(const QByteArray &data, quint32 isoFieldID);
	~FieldPS();

	bool hasMapFiles() const;
//	void setIsoFieldID(quint32 isoFieldID);
	quint32 isoFieldID() const;
	bool open(const QByteArray &dat_data);
	bool save(QByteArray &dat_data);

private:
	quint32 _isoFieldID;
};

#endif // FIELDPS_H
