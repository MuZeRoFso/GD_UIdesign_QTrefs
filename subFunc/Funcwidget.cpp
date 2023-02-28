#include "Funcwidget.h"

FunctionalWidget::FunctionalWidget(QWidget *parent) : QWidget(parent)
{
    QFile StyleSheet(":/qss/source/qss/FuncWidget.css");
    if(StyleSheet.open(QIODevice::ReadOnly)){
        QString funcstylesheet=StyleSheet.readAll();
        this->setStyleSheet(funcstylesheet);
        StyleSheet.close();
    }
    init();
    mainlayout=new QHBoxLayout(this);
    this->setLayout(mainlayout);
    mainlayout->setSpacing(0);
    mainlayout->addWidget(navibar);
    mainlayout->addWidget(stackwidget);
}

void FunctionalWidget::init()
{
    QFont itemfont("\"黑体\",\"宋体\",Arial, Helvetica, sans-serif");
    itemfont.setPixelSize(20);
    navibar=new QListWidget(this);
    navibar->setFocusPolicy(Qt::NoFocus);
    navibar->setObjectName("NaviBar");
    navibar->setFixedSize(NAVI_BAR_WIDTH,770);
    stackwidget=new QStackedWidget(this);
    stackwidget->setFixedSize(1200,770);
    i_secretcheck=new QListWidgetItem("涉密检查",navibar);
    i_secretcheck->setTextAlignment(Qt::AlignCenter);
    i_secretcheck->setFont(itemfont);
    secretW=new SecretScanWidget(this);
    stackwidget->addWidget(secretW);
    i_trace=new QListWidgetItem("网页浏览记录",navibar);
    i_trace->setTextAlignment(Qt::AlignCenter);
    i_trace->setFont(itemfont);
    traceW=new TraceCheckWidget(stackwidget);
    stackwidget->addWidget(traceW);
    i_sysinfo=new QListWidgetItem("主机信息",navibar);
    i_sysinfo->setTextAlignment(Qt::AlignCenter);
    i_sysinfo->setFont(itemfont);
    sysinfoW=new SysInfoWidget(stackwidget);
    stackwidget->addWidget(sysinfoW);
    i_gendb=new QListWidgetItem("生成数据库",navibar);
    i_gendb->setTextAlignment(Qt::AlignCenter);
    i_gendb->setFont(itemfont);
    gendbW=new GenDB_Widget(stackwidget);
    stackwidget->addWidget(gendbW);
    navibar->setCurrentRow(0);
    connect(navibar,&QListWidget::currentRowChanged,stackwidget,&QStackedWidget::setCurrentIndex);
}
