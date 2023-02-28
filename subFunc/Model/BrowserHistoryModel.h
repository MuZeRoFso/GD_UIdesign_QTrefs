#pragma execution_character_set("utf-8")
#ifndef BROWSERHISTORYMODEL_H
#define BROWSERHISTORYMODEL_H
#include <QApplication>
#include <QWidget>
#include <QThread>
#include <QListWidget>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTableView>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSettings>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <QLabel>
#include <QPainter>
#include <QAbstractItemModel>
#include <QAction>
#include <QMessageBox>
#include <QClipboard>
#include <QDesktopServices>
#include <QDebug>

struct BrowseHistoryInfo;

class BrowserHistoryModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit BrowserHistoryModel(QObject *parent = nullptr);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void insertRow(const BrowseHistoryInfo &newitem,const QModelIndex &parent = QModelIndex());
    void clearData(const QModelIndex &parent = QModelIndex());
signals:
    void Historyisready();

private:
    int currentRow;
    QStringList const historyheader{"网页标题","浏览次数","浏览时间","网址"};
    QList<BrowseHistoryInfo> history;
};

struct BrowseHistoryInfo{
    BrowseHistoryInfo(QString title,QString visited_count,QString last_visited_time,QString url) :
    title(title),visited_count(visited_count),last_visited_time(last_visited_time),url(url){}
    QString title,visited_count,last_visited_time,url;
};

class TraceInfo : public QObject
{
    Q_OBJECT
public:
    enum ChromiumType{Chrome=0, MsEdge=1, Defalut_Type};
    Q_ENUM(ChromiumType)

    TraceInfo();
    void TranVisitedTime(const quint64 &last,QString &timestr);
    QAbstractTableModel * getHistoryModel(ChromiumType type=Defalut_Type);
public slots:
    void getIEHistory();        //IE浏览记录获取
    void getChromiumHistory(TraceInfo::ChromiumType type=Chrome);  //Chromium内核浏览记录获取

signals:
    void CheckError(const QString &errmsg);
private:
    QString LOCAL_APPDATA_PATH;
    const QString TempDir="TempDir\\";
    const QString MoblieAccessPath="HKEY_LOCAL_MACHINE\\SYSTEM\\ControlSet001\\Enum\\USBSTOR";
    const QString ChromeHistoryPath="\\Google\\Chrome\\User Data\\Default\\History";
    const QString TempChromeHistoryPath="tmp_ChromeHistory";
    const QString EdgeHistoryPath="\\Microsoft\\Edge\\User Data\\Default\\History";
    const QString TempEdgeHistoryPath="tmp_EdgeHistory";
    const QString RecentDocsPath;
    const QString InstallApp32Path;
    const QString InstallApp64Path;
    BrowserHistoryModel *IE,*Google_Chrome,*Ms_Edge;
    QSqlDatabase *db;
};

#endif // BROWSERHISTORYMODEL_H
