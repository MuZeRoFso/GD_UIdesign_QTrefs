#include "SysInfoWidget.h"

SysInfoWidget::SysInfoWidget(QWidget *parent) : QWidget(parent)
{
    sysinfomodel=new SysInfo;
    updatethread=new QThread(this);
    sysinfomodel->moveToThread(updatethread);
    QFile stylesheetfile(":/qss/source/qss/SysInfoWidget.css");
    if(stylesheetfile.open(QIODevice::ReadOnly)){
        this->setStyleSheet(stylesheetfile.readAll());
        stylesheetfile.close();
    }
    initTab();
    this->resize(1280,720);
    vlayout=new QVBoxLayout(this);
    selfdesc=new QLabel("主机硬件信息及其属性",this);
    selfdesc->setObjectName("SelfDesc");
    selfdesc->setFixedHeight(50);
    refresh=new QPushButton("获取当前页面信息",this);
    refresh->setObjectName("Refresh");
    refresh->setMaximumWidth(200);
    refresh->setFixedHeight(50);
    this->setLayout(vlayout);
    vlayout->addWidget(selfdesc);
    vlayout->addWidget(tab);
    vlayout->addWidget(refresh);
    this->setObjectName("SysInfoWidget");
    connect(refresh,&QPushButton::clicked,this,&SysInfoWidget::updateCurrentItem);
    updatethread->setStackSize(15728640);
    updatethread->start(QThread::LowestPriority);
}

void SysInfoWidget::initTab()
{
    tab=new QTabWidget(this);
    tab->setObjectName("SysTab");
    initCPUW();
    tab->addTab(cpuW,"CPU与主板信息");
    initOsW();
    tab->addTab(osW,"操作系统信息");
    initMemW();
    tab->addTab(memW,"内存信息");
    initVideoW();
    tab->addTab(videoW,"显卡信息");
    initDiskW();
    tab->addTab(DiskW,"硬盘信息");
    initNetW();
    tab->addTab(netW,"网卡信息");
}

void SysInfoWidget::initCPUW()
{
    cpuW=new QWidget(this);
    cpuL=new QGridLayout(this);
    cpuW->setLayout(cpuL);
    QStringList cpuinfolist{"CPU型号","CPU架构","CPU说明","最大时钟速度","接口设计类型","CPU序列号",};
    QStringList mbinfolist{"主板型号","主板厂商","主板供应商","主板版本","上次更新日期","主板SKU","主板ID"};
    for(int row=0;row<cpuinfolist.size();row++)
        cpuL->addWidget(new QLabel(cpuinfolist.at(row),this),row,0);
    for(int row=0;row<mbinfolist.size();row++)
        cpuL->addWidget(new QLabel(mbinfolist.at(row),this),row,2);
    C_pro=new QLabel(this);
    cpuL->addWidget(C_pro,0,1);
    C_arch=new QLabel(this);
    cpuL->addWidget(C_arch,1,1);
    C_caption=new QLabel(this);
    cpuL->addWidget(C_caption,2,1);
    C_maxclock=new QLabel(this);
    cpuL->addWidget(C_maxclock,3,1);
    C_socket=new QLabel(this);
    cpuL->addWidget(C_socket,4,1);
    processorid=new QLabel(this);
    cpuL->addWidget(processorid,5,1);
    B_pro=new QLabel(this);
    cpuL->addWidget(B_pro,0,3);
    B_manu=new QLabel(this);
    cpuL->addWidget(B_manu,1,3);
    B_ven=new QLabel(this);
    cpuL->addWidget(B_ven,2,3);
    B_ver=new QLabel(this);
    cpuL->addWidget(B_ver,3,3);
    B_insdate=new QLabel(this);
    cpuL->addWidget(B_insdate,4,3);
    B_SKU=new QLabel(this);
    cpuL->addWidget(B_SKU,5,3);
    B_id=new QLabel(this);
    cpuL->addWidget(B_id,6,3);
    connect(this,&SysInfoWidget::CpuInfo,sysinfomodel,&SysInfo::getCpuInfo);
    connect(this,&SysInfoWidget::BaseBoardInfo,sysinfomodel,&SysInfo::getBaseBoardInfo);
}

void SysInfoWidget::updateCPUW()
{
    emit CpuInfo(C_pro,C_arch,C_caption,C_maxclock,C_socket,processorid);
    emit BaseBoardInfo(B_pro,B_manu,B_ven,B_ver,B_insdate,B_SKU,B_id);
}

void SysInfoWidget::initOsW()
{
    osW=new QWidget(this);
    osL=new QGridLayout(this);
    osW->setLayout(osL);
    QStringList osinfolist{"操作系统","用户名","系统安装时间","最后更新时间","系统版本","生产厂商","系统位数","当前登录用户"};
    for(int row=0;row<osinfolist.size();row++)
        osL->addWidget(new QLabel(osinfolist.at(row),this),row,0);
    os=new QLabel(this);
    osL->addWidget(os,0,1);
    csname=new QLabel(this);
    osL->addWidget(csname,1,1);
    insdate=new QLabel(this);
    osL->addWidget(insdate,2,1);
    lastupdate=new QLabel(this);
    osL->addWidget(lastupdate,3,1);
    osver=new QLabel(this);
    osL->addWidget(osver,4,1);
    osmanu=new QLabel(this);
    osL->addWidget(osmanu,5,1);
    osarch=new QLabel(this);
    osL->addWidget(osarch,6,1);
    reguser=new QLabel(this);
    osL->addWidget(reguser,7,1);
    connect(this,&SysInfoWidget::OsInfo,sysinfomodel,&SysInfo::getOsInfo);
}

void SysInfoWidget::updateOsW()
{
    emit OsInfo(os,csname,insdate,lastupdate,osver,osmanu,osarch,reguser);
}

void SysInfoWidget::initMemW()
{
    memW=new QTableWidget(this);
    QStringList memtabhead{"内存型号","生产厂商","内存容量(GB)","内存频率(MHz)","内存位宽","序列号"};
    memW->setColumnCount(memtabhead.size());
    memW->setHorizontalHeaderLabels(memtabhead);
    memW->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    memW->setSelectionBehavior(QAbstractItemView::SelectRows);
    memW->verticalHeader()->setVisible(false);
    memW->horizontalHeader()->setHighlightSections(false);
    memW->setFocusPolicy(Qt::NoFocus);
    connect(this,&SysInfoWidget::MemInfo,sysinfomodel,&SysInfo::getMemInfo);
    connect(sysinfomodel,&SysInfo::MemInfoisReady,this,&SysInfoWidget::updateMemInfo);
}

void SysInfoWidget::updateMemInfo()
{
    const QList<_MemInfo> *memlist=sysinfomodel->getMemList();
    int rowcount=memlist->size();
    memW->setRowCount(rowcount);
    for(int row=0;row<rowcount;row++){//Pro,Ven,Size,MHz,Width,ID
        _MemInfo const *temp=&memlist->at(row);
        memW->setItem(row,0,new QTableWidgetItem(temp->Pro));
        memW->setItem(row,1,new QTableWidgetItem(temp->Ven));
        memW->setItem(row,2,new QTableWidgetItem(temp->Size));
        memW->setItem(row,3,new QTableWidgetItem(temp->MHz));
        memW->setItem(row,4,new QTableWidgetItem(temp->Width));
        memW->setItem(row,5,new QTableWidgetItem(temp->ID));
    }
}

void SysInfoWidget::initVideoW()
{
    videoW=new QTableWidget(this);
    QStringList videotabhead{"显卡型号","显卡内存","驱动更新时间","驱动版本","生产厂商","刷新率","分辨率"};
    videoW->setColumnCount(videotabhead.size());
    videoW->setHorizontalHeaderLabels(videotabhead);
    videoW->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    videoW->setSelectionBehavior(QAbstractItemView::SelectRows);
    videoW->verticalHeader()->setVisible(false);
    videoW->horizontalHeader()->setHighlightSections(false);
    videoW->setFocusPolicy(Qt::NoFocus);
    connect(this,&SysInfoWidget::VideoInfo,sysinfomodel,&SysInfo::getVideoInfo);
    connect(sysinfomodel,&SysInfo::VideoInfoisReady,this,&SysInfoWidget::updateVideoInfo);
}

void SysInfoWidget::updateVideoInfo()
{
    const QList<_Video> *videolist=sysinfomodel->getVideoList();
    int rowcount=videolist->size();
    videoW->setRowCount(rowcount);
    for(int row=0;row<rowcount;row++){//Pro,Size,LastUpdate,DriverVer,Ven,Refresh,Resolu
        _Video const *temp=&videolist->at(row);
        videoW->setItem(row,0,new QTableWidgetItem(temp->Pro));
        videoW->setItem(row,1,new QTableWidgetItem(temp->Size));
        videoW->setItem(row,2,new QTableWidgetItem(temp->LastUpdate));
        videoW->setItem(row,3,new QTableWidgetItem(temp->DriverVer));
        videoW->setItem(row,4,new QTableWidgetItem(temp->Ven));
        videoW->setItem(row,5,new QTableWidgetItem(temp->Refresh));
        videoW->setItem(row,6,new QTableWidgetItem(temp->Resolu));
    }
}

void SysInfoWidget::initDiskW()
{
    DiskW=new QWidget(this);
    disklayout=new QVBoxLayout;
    DiskW->setLayout(disklayout);
    logicaldiskW=new QTableWidget;
    disklayout->addWidget(logicaldiskW);
    QStringList disktabhead{"分区名称","盘符","文件系统","总容量","可用空间","磁盘ID"};
    logicaldiskW->setColumnCount(disktabhead.size());
    logicaldiskW->setHorizontalHeaderLabels(disktabhead);
    logicaldiskW->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    logicaldiskW->setSelectionBehavior(QAbstractItemView::SelectRows);
    logicaldiskW->verticalHeader()->setVisible(false);
    logicaldiskW->horizontalHeader()->setHighlightSections(false);
    logicaldiskW->setFocusPolicy(Qt::NoFocus);
    HddW=new QTableWidget;
    disklayout->addWidget(HddW);
    QStringList drivetabhead{"硬盘型号","总容量","固件版本","序列号","硬盘索引(Index)"};
    HddW->setColumnCount(drivetabhead.size());
    HddW->setHorizontalHeaderLabels(drivetabhead);
    HddW->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    HddW->setSelectionBehavior(QAbstractItemView::SelectRows);
    HddW->verticalHeader()->setVisible(false);
    HddW->horizontalHeader()->setHighlightSections(false);
    HddW->setFocusPolicy(Qt::NoFocus);
    connect(this,&SysInfoWidget::HDDInfo,sysinfomodel,&SysInfo::getHDDInfo);
    connect(sysinfomodel,&SysInfo::HDDInfoisReady,this,&SysInfoWidget::updateHDDInfo);
}

void SysInfoWidget::updateHDDInfo()
{
    QList<_DiskPart> const *disklist=sysinfomodel->getDiskList();
    int rowcount=disklist->size();
    logicaldiskW->setRowCount(rowcount);
    for(int row=0;row<rowcount;row++){//Name,Flag,FS,TotalSize,Free,ID
        _DiskPart const *temp=&disklist->at(row);
        logicaldiskW->setItem(row,0,new QTableWidgetItem(temp->Name));
        logicaldiskW->setItem(row,1,new QTableWidgetItem(temp->Flag));
        logicaldiskW->setItem(row,2,new QTableWidgetItem(temp->FS));
        logicaldiskW->setItem(row,3,new QTableWidgetItem(temp->TotalSize));
        logicaldiskW->setItem(row,4,new QTableWidgetItem(temp->Free));
        logicaldiskW->setItem(row,5,new QTableWidgetItem(temp->ID));
    }
    QList<_HDD> const *hddlist=sysinfomodel->getHddList();
    rowcount=hddlist->size();
    HddW->setRowCount(rowcount);
    for(int row=0;row<rowcount;row++){//Pro,TotalSize,Ver,ID,Index
        _HDD const *temp=&hddlist->at(row);
        HddW->setItem(row,0,new QTableWidgetItem(temp->Pro));
        HddW->setItem(row,1,new QTableWidgetItem(temp->TotalSize));
        HddW->setItem(row,2,new QTableWidgetItem(temp->Ver));
        HddW->setItem(row,3,new QTableWidgetItem(temp->ID));
        HddW->setItem(row,4,new QTableWidgetItem(temp->Index));
        }
}

void SysInfoWidget::initNetW()
{
    netW=new QTableWidget(this);
    QStringList nettabhead{"网卡名称","物理地址(MAC)","IPV4地址","IPV6地址","子网掩码","网卡ID"};
    netW->setColumnCount(nettabhead.size());
    netW->setHorizontalHeaderLabels(nettabhead);
    netW->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    netW->setSelectionBehavior(QAbstractItemView::SelectRows);
    netW->setHorizontalScrollMode(QTableView::ScrollPerPixel);
    netW->verticalHeader()->setVisible(false);
    netW->horizontalHeader()->setHighlightSections(false);
    netW->setFocusPolicy(Qt::NoFocus);
    connect(this,&SysInfoWidget::NetInfo,sysinfomodel,&SysInfo::getNetInfo);
    connect(sysinfomodel,&SysInfo::NetInfoisReady,this,&SysInfoWidget::updateNetInfo);
}

void SysInfoWidget::updateNetInfo()
{
    netW->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    const QList<_Ethernet> *netlist=sysinfomodel->getEthList();
    int rowcount=netlist->size();
    netW->setRowCount(rowcount);
    for(int row=0;row<rowcount;row++){//Name,MAC,IP4,IP6,SubNet,ID
        _Ethernet const *temp=&netlist->at(row);
        netW->setItem(row,0,new QTableWidgetItem(temp->Name));
        netW->setItem(row,1,new QTableWidgetItem(temp->MAC));
        netW->setItem(row,2,new QTableWidgetItem(temp->IP4));
        netW->setItem(row,3,new QTableWidgetItem(temp->IP6));
        netW->setItem(row,4,new QTableWidgetItem(temp->SubNet));
        netW->setItem(row,5,new QTableWidgetItem(temp->ID));
    }
}

void SysInfoWidget::updateCurrentItem()
{
    switch(tab->currentIndex()){
    case 0: //CPU和主板模块
        updateCPUW();
        break;
    case 1: //操作系统
        updateOsW();
        break;
    case 2: //内存信息
        memW->setRowCount(0);
        emit MemInfo(memW);
        break;
    case 3: //显卡信息
        videoW->setRowCount(0);
        emit VideoInfo(videoW);
        break;
    case 4: //硬盘信息
        logicaldiskW->setRowCount(0);
        HddW->setRowCount(0);
        emit HDDInfo(logicaldiskW,HddW);
        break;
    case 5: //网卡信息
        netW->setRowCount(0);
        emit NetInfo(netW);
        break;
    }
}

void SysInfoWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget,&opt,&p,this);
}

SysInfoWidget::~SysInfoWidget()
{
    updatethread->quit();
    updatethread->wait();
    delete sysinfomodel;
}
