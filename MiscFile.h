#ifndef MISCFILE_H
#define MISCFILE_H

#include <QtCore>

class MiscFile
{
public:
	MiscFile();
	bool open(const QByteArray &inf, const QByteArray &rat, const QByteArray &mrt, const QByteArray &pmp, const QByteArray &pmd, const QByteArray &pvp);
	bool save(QByteArray &inf, QByteArray &rat, QByteArray &mrt, QByteArray &pmp, QByteArray &pmd, QByteArray &pvp);
	bool isModified();
	const QString &getMapName();
	void setMapName(const QString &mapName);
	const QList<quint16> &getGateways();
	void setGateways(const QList<quint16> &gateways);
	const QByteArray &getRatData();
	void setRatData(const QByteArray &rat);
	const QByteArray &getMrtData();
	void setMrtData(const QByteArray &mrt);
	const QByteArray &getPvpData();
	void setPvpData(const QByteArray &pvp);
	const QByteArray &getPmpData();
	void setPmpData(const QByteArray &pmp);
	const QByteArray &getPmdData();
	void setPmdData(const QByteArray &pmd);
	QList<int> searchAllBattles();
private:
	QString mapName;
	QByteArray inf;
	QByteArray rat;
	QByteArray mrt;
	QByteArray pvp, pmp, pmd;
	QList<quint16> gateways;
	bool modified;
};

#endif // MISCFILE_H
