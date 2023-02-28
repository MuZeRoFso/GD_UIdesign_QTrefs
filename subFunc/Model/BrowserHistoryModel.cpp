#include "BrowserHistoryModel.h"

BrowserHistoryModel::BrowserHistoryModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    currentRow=0;
    for(int col=0;col<historyheader.size();col++)
        this->setHeaderData(col,Qt::Horizontal,historyheader.at(col));
}

QVariant BrowserHistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation==Qt::Horizontal && role==Qt::DisplayRole)
        if(section<historyheader.size())
            return historyheader.at(section);
    return QVariant();
}

int BrowserHistoryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return history.size();
}

int BrowserHistoryModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return historyheader.size();
}

QVariant BrowserHistoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if(role==Qt::DisplayRole){
        if(index.row()<history.size() && index.column()<historyheader.size())
            switch(index.column()){     //title,visited_count,last_visited_time,url
            case 0:
                return history.at(index.row()).title;
            case 1:
                return history.at(index.row()).visited_count;
            case 2:
                return history.at(index.row()).last_visited_time;
            case 3:
                return history.at(index.row()).url;
            }
    }else if((role==Qt::ToolTipRole && index.column()==3) || role==Qt::WhatsThisPropertyRole)
        return history.at(index.row()).url;
    return QVariant();
}

void BrowserHistoryModel::insertRow(const BrowseHistoryInfo &newitem,const QModelIndex &parent)
{
    currentRow=history.size();
    beginInsertRows(parent,currentRow,currentRow);
    BrowseHistoryInfo temp=newitem;
    history.append(temp);
    endInsertRows();
}

void BrowserHistoryModel::clearData(const QModelIndex &parent)
{
    if(history.isEmpty())
        return;
    beginRemoveRows(parent,0,history.size()-1);
    history.clear();
    endRemoveRows();
}

#include <ShlObj.h>
#include <UrlHist.h>
#include <atlbase.h>

TraceInfo::TraceInfo()
{
    wchar_t path[MAX_PATH];
    SHGetSpecialFolderPathW(nullptr,path,CSIDL_LOCAL_APPDATA,false);
    LOCAL_APPDATA_PATH=QString::fromWCharArray(path);
    qRegisterMetaType<TraceInfo::ChromiumType>("TraceInfo::ChromiumType");
    IE=new BrowserHistoryModel;
    Google_Chrome=new BrowserHistoryModel;
    Ms_Edge=new BrowserHistoryModel;
    db=new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
    QDir db_dir=QDir();
    if(!db_dir.exists(TempDir))
        db_dir.mkdir(TempDir);
}

void TraceInfo::TranVisitedTime(const quint64 &last,QString &timestr)
{
    QDateTime date;
    date.setSecsSinceEpoch(last/1000000-11644473600);
    timestr=date.toString("yyyy-MM-d hh:mm:ss ddd");
}

QAbstractTableModel* TraceInfo::getHistoryModel(TraceInfo::ChromiumType type)
{
    switch(type){
    case 0:
        return static_cast<QAbstractTableModel*>(Google_Chrome);
    case 1:
        return static_cast<QAbstractTableModel*>(Ms_Edge);
    default:
        return static_cast<QAbstractTableModel*>(IE);
    }
}

void TraceInfo::getIEHistory()
{
    IE->clearData();
    USES_CONVERSION;
    CoInitialize(nullptr);
    void *stg;
    IUrlHistoryStg2 *tempstg;
    CoCreateInstance(CLSID_CUrlHistory,nullptr,CLSCTX_INPROC,IID_IUrlHistoryStg2,&stg);
    IEnumSTATURL *enumurl;
    tempstg=static_cast<IUrlHistoryStg2*>(stg);
    tempstg->EnumUrls(&enumurl);
    STATURL surl;
    unsigned long Fetched;
    surl.cbSize=sizeof(surl);
    enumurl->Reset();
    QString urlstr;
    while(enumurl->Next(1,&surl,&Fetched)==S_OK){
        urlstr=urlstr.fromWCharArray(surl.pwcsUrl);
        if(urlstr.startsWith("https://") | urlstr.startsWith("http://") | urlstr.startsWith("ftp://")){
            SYSTEMTIME temptime;
            FileTimeToSystemTime(&surl.ftLastUpdated,&temptime);
            QDateTime itemtime=QDateTime(QDate(temptime.wYear,temptime.wMonth,temptime.wDay),
                                         QTime(temptime.wHour,temptime.wMinute,temptime.wSecond,temptime.wMilliseconds));
            BrowseHistoryInfo temp(QString::fromWCharArray(surl.pwcsTitle),
                                   "1",
                                   itemtime.toString("yyyy-MM-d hh:mm:ss ddd"),
                                   QString::fromWCharArray(surl.pwcsUrl));
            IE->insertRow(temp);
        }
    }
}

void TraceInfo::getChromiumHistory(ChromiumType type)
{
    QString dbpath=LOCAL_APPDATA_PATH;
    QString tempdbpath;
    BrowserHistoryModel *nowModel;
    switch(type)
    {
    case Chrome:
        Google_Chrome->clearData();
        dbpath=dbpath.append(ChromeHistoryPath);
        tempdbpath=TempDir+TempChromeHistoryPath;
        nowModel=Google_Chrome;
        break;
    case MsEdge:
        Ms_Edge->clearData();
        dbpath=dbpath.append(EdgeHistoryPath);
        tempdbpath=TempDir+TempEdgeHistoryPath;
        nowModel=Ms_Edge;
        break;
    default:
        qDebug()<<"错误";
        return;
    }
    QFile dbfile(dbpath);
    if(!dbfile.exists()){
        emit CheckError("数据库文件不存在");
        return;
    }
    QFile sqlfile(dbpath);
    QFile tmpsqlfile(tempdbpath);
    if(tmpsqlfile.exists()){
        tmpsqlfile.remove();
    }
    sqlfile.copy(tempdbpath);
    db->setDatabaseName(tempdbpath);
    db->open();
    QSqlQuery *query=new QSqlQuery(*db);
    QString sql("SELECT title,visit_count,last_visit_time,url FROM urls WHERE visit_count != 0 ORDER BY last_visit_time DESC;");
    query->exec(sql);
    if(query->isValid()){
        emit CheckError("数据库读取失败");
        return;
    }
    QString timestr;
    while(query->next()){
        TranVisitedTime(query->value(2).toULongLong(),timestr);
        BrowseHistoryInfo temp(query->value(0).toString(),query->value(1).toString(),
                               timestr,query->value(3).toString());
        nowModel->insertRow(temp);
    }
    query->clear();
    delete query;
    db->close();
}
