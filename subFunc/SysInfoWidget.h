#ifndef SYSINFOWIDGET_H
#define SYSINFOWIDGET_H
#include "Model/SysInfoModel.h"

class SysInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SysInfoWidget(QWidget *parent = nullptr);
    ~SysInfoWidget() override;
signals:
    void CpuInfo(QLabel *pro,QLabel *arch,QLabel *caption,QLabel *maxclock,QLabel *socket,QLabel *id);
    void BaseBoardInfo(QLabel *pro,QLabel *manu,QLabel *ven,QLabel *ver,QLabel *insdate,QLabel *SKU,QLabel *id);
    void OsInfo(QLabel *os,QLabel *csname,QLabel *insdate,QLabel *lastupdate,QLabel *osver,QLabel *manu,QLabel *osarch,QLabel *reguser);
    void MemInfo(QTableWidget *memW);
    void VideoInfo(QTableWidget *videoW);
    void HDDInfo(QTableWidget *logicaldiskW,QTableWidget *hddW);
    void NetInfo(QTableWidget *netW);
private:
    SysInfo *sysinfomodel;
    QVBoxLayout *vlayout;
    QLabel *selfdesc;
    QTabWidget *tab;
    QPushButton *refresh;
    QThread *updatethread;
    void initTab();
    QWidget *cpuW;
    QGridLayout *cpuL;
    QLabel *C_pro;
    QLabel *C_arch;
    QLabel *C_caption;
    QLabel *C_maxclock;
    QLabel *C_socket;
    QLabel *processorid;
    QLabel *B_pro;
    QLabel *B_manu;
    QLabel *B_ven;
    QLabel *B_ver;
    QLabel *B_insdate;
    QLabel *B_SKU;
    QLabel *B_id;
    void initCPUW();
    void updateCPUW();
    QLabel *os;
    QLabel *csname;
    QLabel *insdate;
    QLabel *lastupdate;
    QLabel *osver;
    QLabel *osmanu;
    QLabel *osarch;
    QLabel *reguser;
    QWidget *osW;
    QGridLayout *osL;
    void initOsW();
    void updateOsW();
    QTableWidget *memW;
    void initMemW();
    QTableWidget *videoW;
    void initVideoW();
    QWidget *DiskW;
    QVBoxLayout *disklayout;
    QTableWidget *logicaldiskW;
    QTableWidget *HddW;
    void initDiskW();
    QTableWidget *netW;
    void initNetW();
private slots:
    void updateCurrentItem();
    void updateMemInfo();
    void updateVideoInfo();
    void updateHDDInfo();
    void updateNetInfo();
    void paintEvent(QPaintEvent *event) override;
};

#endif // SYSINFOWIDGET_H
