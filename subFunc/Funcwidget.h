#pragma execution_character_set("utf-8")
#ifndef FUNCWIDGET_H
#define FUNCWIDGET_H
#include <QWidget>
#include <QListWidget>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QPainter>
#include "SecretScanWidget.h"       //涉密信息扫描界面
#include "SysInfoWidget.h"        //显示本机信息的界面
#include "TraceCheckWidget.h"       //痕迹获取界面
#include "GenDB_Widget.h"

#define NAVI_BAR_WIDTH 200

class FunctionalWidget : public QWidget
{
/*************************************
 * 用于实现左侧导航栏和以及链接对应的功能界面
 * ①涉密扫描 ②痕迹扫描 ③本机信息
*************************************/
    Q_OBJECT
public:
    explicit FunctionalWidget(QWidget *parent = nullptr);

signals:

private slots:

private:
    QHBoxLayout *mainlayout;
    QListWidget *navibar;       //功能导航栏
    QStackedWidget *stackwidget;//功能主界面
    QListWidgetItem * i_secretcheck;//涉密检查
    SecretScanWidget * secretW;
    QListWidgetItem * i_trace;//痕迹检查
    QWidget * traceW;
    QListWidgetItem * i_sysinfo;
    SysInfoWidget *sysinfoW;
    QListWidgetItem * i_gendb;
    GenDB_Widget *gendbW;
    void init();
};

#endif // FUNCWIDGET_H
