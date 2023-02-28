#include "TraceCheckWidget.h"

TraceCheckWidget::TraceCheckWidget(QWidget *parent) : QWidget(parent)
{
    QFile stylesheetfile(":/qss/source/qss/TraceCheckWidget.css");
    if(stylesheetfile.open(QIODevice::ReadOnly)){
        this->setStyleSheet(stylesheetfile.readAll());
        stylesheetfile.close();
    }
    tracemodel=new TraceInfo;
    updatethread=new QThread(this);
    tracemodel->moveToThread(updatethread);
    netlayout=new QVBoxLayout(this);
    this->setLayout(netlayout);
    initNethisW();
    selfdesc=new QLabel("查询网页浏览器历史记录",this);
    selfdesc->setObjectName("SelfDesc");
    netrefresh=new QPushButton("获取浏览器记录",this);
    netrefresh->setObjectName("Refresh");
    netrefresh->setMaximumWidth(200);
    netlayout->addWidget(selfdesc);
    netlayout->addWidget(nettab);
    netlayout->addWidget(netrefresh);
    this->setObjectName("TraceInfoWidget");
    connect(netrefresh,&QPushButton::clicked,this,&TraceCheckWidget::RequestNetHis);
    updatethread->setStackSize(15728640);
    updatethread->start(QThread::LowestPriority);
}

void TraceCheckWidget::initNethisW()
{
    nettab=new QTabWidget(this);
    nettab->setObjectName("NetTab");
    copyurl=new QAction("复制当前网址",this);
    openselectedurl=new QAction("打开当前网址",this);
    IE=createNet();
    IE->setModel(tracemodel->getHistoryModel());
    nettab->addTab(IE,"IE浏览器");
    chrome=createNet();
    chrome->setModel(tracemodel->getHistoryModel(TraceInfo::Chrome));
    nettab->addTab(chrome,"Chrome浏览器");
    edge=createNet();
    edge->setModel(tracemodel->getHistoryModel(TraceInfo::MsEdge));
    nettab->addTab(edge,"Edge浏览器");
    connect(this,&TraceCheckWidget::RequestIEHistory,tracemodel,&TraceInfo::getIEHistory);
    connect(this,&TraceCheckWidget::RequestChromiumHistory,tracemodel,&TraceInfo::getChromiumHistory);
    connect(tracemodel,&TraceInfo::CheckError,this,&TraceCheckWidget::CheckError);
    connect(copyurl,&QAction::triggered,this,&TraceCheckWidget::copyUrlData);
    connect(openselectedurl,&QAction::triggered,this,&TraceCheckWidget::OpenSelectedUrl);
}

QTableView *TraceCheckWidget::createNet()
{
    QTableView *temp=new QTableView(this);
    temp->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    temp->setSelectionBehavior(QAbstractItemView::SelectRows);
    temp->horizontalHeader()->setHighlightSections(false);
    temp->verticalHeader()->setVisible(false);
    temp->setFocusPolicy(Qt::NoFocus);
    temp->setVerticalScrollMode(QTableView::ScrollPerPixel);
    temp->setSelectionMode(QTableView::SingleSelection);
    temp->setContextMenuPolicy(Qt::ActionsContextMenu);
    temp->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    temp->addAction(copyurl);
    temp->addAction(openselectedurl);
    return temp;
}

void TraceCheckWidget::RequestNetHis()
{
    switch(nettab->currentIndex()){
    case 0:
        emit RequestIEHistory();
        break;
    case 1:
        emit RequestChromiumHistory(TraceInfo::Chrome);
        break;
    case 2:
        emit RequestChromiumHistory(TraceInfo::MsEdge);
        break;
    }
}

void TraceCheckWidget::copyUrlData()
{
    QString urlpath;
    QModelIndex index;
    switch(nettab->currentIndex()){
    case 0:
        index=IE->selectionModel()->selectedRows().at(0);
        urlpath=IE->selectionModel()->model()->data(index,Qt::WhatsThisPropertyRole).toString();
        break;
    case 1:
        index=chrome->selectionModel()->selectedRows().at(0);
        urlpath=chrome->selectionModel()->model()->data(index,Qt::WhatsThisPropertyRole).toString();
        break;
    case 2:
        index=edge->selectionModel()->selectedRows().at(0);
        urlpath=edge->selectionModel()->model()->data(index,Qt::WhatsThisPropertyRole).toString();
        break;
    }
    QClipboard *board=QApplication::clipboard();
    board->setText(urlpath);
}

void TraceCheckWidget::OpenSelectedUrl()
{
    QUrl selectedurl;
    QModelIndex index;
    switch(nettab->currentIndex()){
    case 0:
        index=IE->selectionModel()->selectedRows().at(0);
        selectedurl.setUrl(IE->selectionModel()->model()->data(index,Qt::WhatsThisPropertyRole).toString());
        break;
    case 1:
        index=chrome->selectionModel()->selectedRows().at(0);
        selectedurl.setUrl(chrome->selectionModel()->model()->data(index,Qt::WhatsThisPropertyRole).toString());
        break;
    case 2:
        index=edge->selectionModel()->selectedRows().at(0);
        selectedurl.setUrl(edge->selectionModel()->model()->data(index,Qt::WhatsThisPropertyRole).toString());
        break;
    }
    QDesktopServices::openUrl(selectedurl);
}

void TraceCheckWidget::CheckError(const QString &ErrMsg)
{
    QMessageBox::warning(this,"浏览记录读取失败",ErrMsg);
}

void TraceCheckWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget,&opt,&p,this);
}

TraceCheckWidget::~TraceCheckWidget()
{
    updatethread->quit();
    updatethread->wait();
    delete tracemodel;
}
