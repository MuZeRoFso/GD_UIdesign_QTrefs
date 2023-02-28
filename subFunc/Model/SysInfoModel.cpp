#include "SysInfoModel.h"

SysInfo::SysInfo(QObject *parent) : QObject(parent)
{
    powershell=QString("C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe");
    re=new QRegularExpression("\\w+ : ");
    codec=QTextCodec::codecForName("System");
    memlist=new QList<_MemInfo>;
    videolist=new QList<_Video>;
    disklist=new QList<_DiskPart>;
    hddlist=new QList<_HDD>;
    ethlist=new QList<_Ethernet>;
}

const QList<_MemInfo>* SysInfo::getMemList() { return memlist;}
const QList<_Video>* SysInfo::getVideoList(){ return videolist;}
const QList<_DiskPart>* SysInfo::getDiskList(){ return disklist;}
const QList<_HDD>* SysInfo::getHddList(){ return hddlist;}
const QList<_Ethernet>* SysInfo::getEthList(){ return ethlist;}

void SysInfo::getCpuInfo(QLabel *pro, QLabel *arch, QLabel *caption, QLabel *maxclock, QLabel *socket, QLabel *id)
{//"CPU型号","CPU架构","CPU说明","最大时钟速度","接口设计类型"
    QStringList command,outputlist;
    command<<"Get-CimInstance -Class Win32_Processor"<<
             " | Format-list Name,Caption,MaxClockSpeed,SocketDesignation,ProcessorId";
    getOutputList(command,outputlist);
    pro->setText(outputlist.at(0));
    caption->setText(outputlist.at(1));
    maxclock->setText(outputlist.at(2));
    socket->setText(outputlist.at(3));
    id->setText(outputlist.at(4));
    arch->setText(QSysInfo::currentCpuArchitecture());
}

void SysInfo::getBaseBoardInfo(QLabel *pro, QLabel *manu, QLabel *ven, QLabel *ver, QLabel *insdate, QLabel *SKU, QLabel *id)
{//"主板型号","主板厂商","主板供应商","主板版本","安装时间","主板ID"
    QStringList command,outputlist;
    command<<"Get-CimInstance -Class Win32_BaseBoard"<<
             " | Format-list Product,Manufacturer,Version,SerialNumber";
    getOutputList(command,outputlist);
    pro->setText(outputlist.at(0));
    manu->setText(outputlist.at(1));
    ver->setText(outputlist.at(2));
    id->setText(outputlist.at(3));
    QString baseboardRegPath("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\BIOS");
    QSettings settings(baseboardRegPath,QSettings::NativeFormat);
    ven->setText(settings.value("BIOSVendor","0").toString());
    insdate->setText(settings.value("BIOSReleaseDate","0").toString());
    SKU->setText(settings.value("SystemSKU","0").toString());
}

void SysInfo::getOsInfo(QLabel *os, QLabel *csname, QLabel *insdate, QLabel *lastupdate, QLabel *osver, QLabel *manu, QLabel *osarch, QLabel *reguser)
{//"操作系统","用户名","系统安装时间","最后更新时间","系统版本","生产厂商","系统位数","当前登录用户"
    QStringList command,outputlist;
    command<<"Get-CimInstance -Class Win32_OperatingSystem"<<
             " | Format-list Caption,CSName,InstallDate,LastBootUpTime,Version,Manufacturer,OSArchitecture,RegisteredUser";
    getOutputList(command,outputlist);
    os->setText(outputlist.at(0));
    csname->setText(outputlist.at(1));
    insdate->setText(outputlist.at(2));
    lastupdate->setText(outputlist.at(3));
    osver->setText(outputlist.at(4));
    manu->setText(outputlist.at(5));
    osarch->setText(outputlist.at(6));
    reguser->setText(outputlist.at(7));
}

void SysInfo::getMemInfo()
{
    QStringList command,outputlist;
    command<<"Get-CimInstance -Class Win32_PhysicalMemory"<<
             " | Format-list PartNumber,Manufacturer,Capacity,Speed,DataWidth,SerialNumber";
    getOutputList(command,outputlist);
    int memcount=outputlist.size()/MemItemCount;
    QString ramstr;
    memlist->clear();
    for(int row=0;row<memcount;row++){
        _MemInfo temp;
        temp.Pro=outputlist.at(row*MemItemCount+0);
        temp.Ven=outputlist.at(row*MemItemCount+1);
        BtoStr(outputlist.at(row*MemItemCount+2).toULongLong(),ramstr);
        temp.Size=ramstr;
        temp.MHz=outputlist.at(row*MemItemCount+3);
        temp.Width=outputlist.at(row*MemItemCount+4);
        temp.ID=outputlist.at(row*MemItemCount+5);
        memlist->append(temp);
    }
    emit MemInfoisReady();
}

void SysInfo::getVideoInfo()
{
    QStringList command,outputlist;
    command<<"Get-CimInstance -Class Win32_VideoController"<<
             " | Format-list Name,AdapterRAM,DriverDate,DriverVersion,AdapterCompatibility,CurrentRefreshRate,CurrentHorizontalResolution,CurrentVerticalResolution";
    getOutputList(command,outputlist);
    int videocount=outputlist.size()/VideoItemCount;
    QString ramstr;
    videolist->clear();
    for(int row=0;row<videocount;row++){
        _Video temp;
        temp.Pro=outputlist.at(row*VideoItemCount+0);
        BtoStr(outputlist.at(row*VideoItemCount+1).toULongLong(),ramstr);
        temp.Size=ramstr;
        temp.LastUpdate=outputlist.at(row*VideoItemCount+2);
        temp.DriverVer=outputlist.at(row*VideoItemCount+3);
        temp.Ven=outputlist.at(row*VideoItemCount+4);
        temp.Refresh=outputlist.at(row*VideoItemCount+5);
        temp.Resolu=outputlist.at(row*VideoItemCount+6)+"x "+outputlist.at(row*VideoItemCount+7);
        videolist->append(temp);
    }
    emit VideoInfoisReady();
}

void SysInfo::getHDDInfo()
{
    QStringList command,outputlist;
    command<<"Get-CimInstance -Class Win32_LogicalDisk"<<
             " | Format-list VolumeName,Caption,FileSystem,Size,FreeSpace,VolumeSerialNumber";
    getOutputList(command,outputlist);
    int diskcount=outputlist.size()/LogicalDiskCount;
    QString sizestr;
    disklist->clear();
    for(int row=0;row<diskcount;row++){
        _DiskPart temp;
        temp.Name=outputlist.at(row*LogicalDiskCount+0);
        temp.Flag=outputlist.at(row*LogicalDiskCount+1);
        temp.FS=outputlist.at(row*LogicalDiskCount+2);
        BtoStr(outputlist.at(row*LogicalDiskCount+3).toULongLong(),sizestr);
        temp.TotalSize=sizestr;
        BtoStr(outputlist.at(row*LogicalDiskCount+4).toULongLong(),sizestr);
        temp.Free=sizestr;
        temp.ID=outputlist.at(row*LogicalDiskCount+5);
        disklist->append(temp);
    }
    command.clear();
    outputlist.clear();
    command<<"Get-CimInstance -ClassName Win32_DiskDrive"<<
             " | Format-list Caption,Size,FirmwareRevision,SerialNumber,Index";
    getOutputList(command,outputlist);
    int drivecount=outputlist.size()/DiskDriverCount;
    hddlist->clear();
    for(int row=0;row<drivecount;row++){
        _HDD temp;
        temp.Pro=outputlist.at(row*DiskDriverCount+0);
        BtoStr(outputlist.at(row*DiskDriverCount+1).toULongLong(),sizestr);
        temp.TotalSize=sizestr;
        temp.Ver=outputlist.at(row*DiskDriverCount+2);
        temp.ID=outputlist.at(row*DiskDriverCount+3);
        temp.Index=outputlist.at(row*DiskDriverCount+4);
        hddlist->append(temp);
    }
    emit HDDInfoisReady();
}

void SysInfo::getNetInfo()
{
    QStringList command,outputlist;
    command<<"Get-CimInstance -Class Win32_NetworkAdapterConfiguration"<<
             " | Where-Object{$_.IPAddress -notlike \"\"}"<<
             " | Format-list Description,MACAddress,IPAddress,IPSubnet,SettingID";
    getOutputList(command,outputlist);
    int netcount=outputlist.size()/NetItemCount;
    QStringList IP;
    QRegularExpression IPre(", ");
    ethlist->clear();
    for(int row=0;row<netcount;row++){//Name,MAC,IP4,IP6,SubNet,ID
        _Ethernet temp;
        temp.Name=outputlist.at(row*NetItemCount+0);
        temp.MAC=outputlist.at(row*NetItemCount+1);
        IP=outputlist.at(row*NetItemCount+2).split(IPre);
        temp.IP4=IP.value(0).remove(0,1);
        temp.IP6=IP.value(1).remove(-2,1);
        temp.SubNet=outputlist.at(row*NetItemCount+3).split(IPre).value(0).remove(0,1);
        temp.ID=outputlist.value(row*NetItemCount+4);
        ethlist->append(temp);
    }
    emit NetInfoisReady();
}

void SysInfo::getOutputList(QStringList &command,QStringList &outputlist)
{
    QProcess process;
    QByteArray output;
    process.start(powershell,command);
    if(process.waitForFinished())
        output=process.readAllStandardOutput().simplified();
    process.close();
    outputlist=codec->toUnicode(output).split(*re);
    outputlist.removeAll("");
}

void SysInfo::BtoStr(quint64 b, QString &str)
{
    if(b/GB>=1)
        str.setNum(b/GB).append(" GB");
    else if(b/MB>=1)
        str.setNum(b/MB).append(" MB");
    else if(b/KB>=1)
        str.setNum(b/KB).append(" KB");
    else
        str.setNum(b).append(" B");
}

SysInfo::~SysInfo()
{
    memlist->clear();
    delete memlist;
    videolist->clear();
    delete videolist;
    disklist->clear();
    delete disklist;
    hddlist->clear();
    delete hddlist;
    ethlist->clear();
    delete ethlist;
}
