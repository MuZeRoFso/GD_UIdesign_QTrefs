#ifndef SYSINFOMODEL_H
#define SYSINFOMODEL_H
#include <QWidget>
#include <QThread>
#include <QLabel>
#include <QTabWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QGridLayout>
#include <QPainter>
#include <QProcess>
#include <QSysInfo>
#include <QSettings>
#include <QPushButton>
#include <QTextCodec>
#include <QFile>
#include <QDebug>

struct _MemInfo;
struct _Video;
struct _DiskPart;
struct _HDD;
struct _Ethernet;

class SysInfo : public QObject
{
    Q_OBJECT
public:
    explicit SysInfo(QObject *parent=nullptr);
    ~SysInfo();
    void BtoStr(quint64 b,QString &str);

signals:
    void MemInfoisReady();
    void VideoInfoisReady();
    void HDDInfoisReady();
    void NetInfoisReady();

public slots:
    void getCpuInfo(QLabel *pro,QLabel *arch,QLabel *caption,QLabel *maxclock,QLabel *socket,QLabel *id);
    void getBaseBoardInfo(QLabel *pro,QLabel *manu,QLabel *ven,QLabel *ver,QLabel *insdate,QLabel *SKU,QLabel *id);
    void getOsInfo(QLabel *os,QLabel *csname,QLabel *insdate,QLabel *lastupdate,QLabel *osver,QLabel *manu,QLabel *osarch,QLabel *reguser);
    void getMemInfo();
    void getVideoInfo();
    void getHDDInfo();
    void getNetInfo();
    const QList<_MemInfo> *getMemList();
    const QList<_Video> *getVideoList();
    const QList<_DiskPart> *getDiskList();
    const QList<_HDD> *getHddList();
    const QList<_Ethernet> *getEthList();

private:
    QString powershell;
    QRegularExpression *re;
    QTextCodec *codec;
    const quint32 GB=1073741824;
    const quint32 MB=1048576;
    const quint32 KB=1024;
    const quint8 MemItemCount=6;
    const quint8 VideoItemCount=7;
    const quint8 LogicalDiskCount=6;
    const quint8 DiskDriverCount=5;
    const quint8 NetItemCount=5;
    QList<_MemInfo> *memlist;
    QList<_Video> *videolist;
    QList<_DiskPart> *disklist;
    QList<_HDD> *hddlist;
    QList<_Ethernet> *ethlist;
    void getOutputList(QStringList &command,QStringList &outputlist);
};

struct _MemInfo{    //"内存型号","生产厂商","内存容量(GB)","内存频率(MHz)","内存位宽","序列号"
    QString Pro,Ven,Size,MHz,Width,ID;
};
struct _Video{  //"显卡型号","显卡内存","驱动更新时间","驱动版本","生产厂商","刷新率","分辨率"
    QString Pro,Size,LastUpdate,DriverVer,Ven,Refresh,Resolu;
};
struct _DiskPart{   //"分区名称","盘符","文件系统","总容量","可用空间","磁盘ID"
    QString Name,Flag,FS,TotalSize,Free,ID;
};

struct _HDD{    //"硬盘型号","总容量","固件版本","序列号","硬盘索引(Index)"
    QString Pro,TotalSize,Ver,ID,Index;
};

struct _Ethernet{   //"网卡名称","物理地址(MAC)","IPV4地址","IPV6地址","子网掩码","网卡ID"
    QString Name,MAC,IP4,IP6,SubNet,ID;
};

#endif // SYSINFOMODEL_H
