#ifndef MMSCOMAGENT_H
#define MMSCOMAGENT_H
/*******************************************************
* @file        mmscomagent.h
* @brief       mms通讯代理通讯类头文件
* @author      MJF
* @date        2022-04-20 13:59:08
********************************************************/

#include <QObject>
#include <QSharedPointer>

#if defined(MMSCOMAGENT_LIBRARY)
#  define MMSCOMAGENTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MMSCOMAGENTSHARED_EXPORT Q_DECL_IMPORT
#endif

/**
 * @brief The MMSComAgent class
 *
 */
class MMSComAgentPrivate;
/**< 定值属性结构体*/
class MMSCOMAGENTSHARED_EXPORT MMSComAgent : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MMSComAgent)
public:
    MMSComAgent(QObject *parent = Q_NULLPTR);
    ~MMSComAgent();

    /**
    * @brief    连接保护装置
    * @param    ip:ip地址，port:端口号，默认为102，msec:超时时间，默认为3000ms
    * @return   true:连接成功   false:连接失败
    */
    bool connectDevice(QString ip,quint16 port = 102,int msec = 3000);

    /**
    * @brief    读值
    * @param    path: 路径    fc:功能限定符  msec:超时时间
    * @return   QVariant QVariant无效表示读取失败
    */
    QVariant readObject(QString path, QString fc, int msec = 3000);

    /**
     * @brief   获取保护装置所有定值路径
     * @param   msec:超时时间默认为10000ms
     * @return  每个定值的路径 QStirngList
     */
    QStringList getSetValPath(int msec = 3000);

    /**
     * @brief   获取定值dipath
     * @param   msec:超时时间默认为3000ms
     * @return  定值的dipath
     */
    QString getSetValDiPath(QString path,int msec = 3000);

    /**
     * @brief   获取错误信息
     * @param   无
     * @return  QString 错误信息
     */
    QString getLastError(long *pSCode = nullptr);
protected:
    QSharedPointer<MMSComAgentPrivate> d_ptr;
};

#endif // MMSCOMAGENT_H
