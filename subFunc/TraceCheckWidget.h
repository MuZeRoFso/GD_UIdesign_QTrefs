#pragma execution_character_set("utf-8")
#ifndef TRACECHECKWIDGET_H
#define TRACECHECKWIDGET_H
#include "Model/BrowserHistoryModel.h"

class TraceCheckWidget : public QWidget
{
/******************
 * 痕迹查询界面 包括:
 * 移动设备接入记录，网页浏览记录，近期文件操作记录，已安装软件
******************/
    Q_OBJECT
public:
    explicit TraceCheckWidget(QWidget *parent = nullptr);
    ~TraceCheckWidget() override;
private:
    QThread *updatethread;
    TraceInfo *tracemodel;
    QListWidgetItem *nethisI;
    QWidget * nethisW;
    QVBoxLayout *netlayout;
    QLabel *selfdesc;
    QTabWidget *nettab;
    QPushButton *netrefresh;
    QTableView *IE;
    QTableView *chrome;
    QTableView *edge;
    QAction *copyurl;
    QAction *openselectedurl;
    void initNethisW();
    QTableView *createNet();
signals:
    void RequestIEHistory();        //IE浏览记录获取
    void RequestChromiumHistory(TraceInfo::ChromiumType type);  //Chromium内核浏览记录获取

private slots:
    void RequestNetHis();
    void copyUrlData();
    void OpenSelectedUrl();
    void CheckError(const QString &ErrMsg);
    void paintEvent(QPaintEvent *event) override;
};

#endif // TRACECHECKWIDGET_H
