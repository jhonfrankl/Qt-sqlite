#ifndef MMSDEF_H
#define MMSDEF_H
#include <QMap>
#if defined(MMSOAT_LIBRARY)
#  define MMSOATSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MMSOATSHARED_EXPORT Q_DECL_IMPORT
#endif
typedef struct settingDA {
    QString dU;
    int SIUnit = 0;
    QString minVal;
    QString maxVal;
    QString stepSize;
    bool isOk = false;
} stuSDA;

typedef struct comtradFileInfo {
    QString fileName;
    quint32 fileSize;
    quint64 lastModified;
    bool isDownload = false;
}stuCFileInfo;

class MMSOATSHARED_EXPORT SIUnit
{
public:
    SIUnit()
    {
        m_mapUnit.insert(1,"");
        m_mapUnit.insert(2,"m");
        m_mapUnit.insert(3,"kg");
        m_mapUnit.insert(4,"s");
        m_mapUnit.insert(5,"A");
        m_mapUnit.insert(6,"K");
        m_mapUnit.insert(7,"mol");
        m_mapUnit.insert(8,"cd");
        m_mapUnit.insert(9,"deg");
        m_mapUnit.insert(10,"rad");
        m_mapUnit.insert(11,"sr");
        m_mapUnit.insert(21,"Gy");
        m_mapUnit.insert(22,"q");
        m_mapUnit.insert(23,"℃");
        m_mapUnit.insert(24,"Sv");
        m_mapUnit.insert(25,"F");
        m_mapUnit.insert(26,"C");
        m_mapUnit.insert(27,"S");
        m_mapUnit.insert(28,"H");
        m_mapUnit.insert(29,"V");
        m_mapUnit.insert(30,"ohm");
        m_mapUnit.insert(31,"J");
        m_mapUnit.insert(32,"N");
        m_mapUnit.insert(33,"Hz");
        m_mapUnit.insert(34,"lx");
        m_mapUnit.insert(35,"Lm");
        m_mapUnit.insert(36,"Wb");
        m_mapUnit.insert(37,"T");
        m_mapUnit.insert(38,"W");
        m_mapUnit.insert(39,"Pa");
        m_mapUnit.insert(41,"m²");
        m_mapUnit.insert(42,"m³");
        m_mapUnit.insert(43,"m/s");
        m_mapUnit.insert(44,"m/s²");
        m_mapUnit.insert(45,"m³/s");
        m_mapUnit.insert(46,"m/m³");
        m_mapUnit.insert(47,"M");
        m_mapUnit.insert(48,"kg/m³");
        m_mapUnit.insert(49,"m²/s");
        m_mapUnit.insert(50,"W/m K");
        m_mapUnit.insert(51,"J/K");
        m_mapUnit.insert(52,"ppm");
        m_mapUnit.insert(53,"1/s");
        m_mapUnit.insert(54,"rad/s");
        m_mapUnit.insert(61,"VA");
        m_mapUnit.insert(62,"W");
        m_mapUnit.insert(63,"VAr");
        m_mapUnit.insert(64,"theta");
        m_mapUnit.insert(65,"cos(theta)");
        m_mapUnit.insert(66,"Vs");
        m_mapUnit.insert(67,"V²");
        m_mapUnit.insert(68,"As");
        m_mapUnit.insert(69,"A²");
        m_mapUnit.insert(70,"A²t");
        m_mapUnit.insert(71,"VAh");
        m_mapUnit.insert(72,"Wh");
        m_mapUnit.insert(73,"VArh");
        m_mapUnit.insert(74,"V/Hz");
    }
    QString getSIUnit(int ord) const
    {
        if(m_mapUnit.contains(ord))
            return m_mapUnit[ord];
        return QString();
    }
private:
    QMap <int,QString> m_mapUnit;
};
#endif // MMSDEF_H
