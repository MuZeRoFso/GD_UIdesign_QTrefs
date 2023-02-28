#include "SecretScanWidget.h"

SecretScanWidget::SecretScanWidget(QWidget *parent) : QWidget(parent)
{
    QFile stylesheetfile(":/qss/source/qss/SecretScanWidget.css");
    if(stylesheetfile.open(QIODevice::ReadOnly)){
        this->setStyleSheet(stylesheetfile.readAll());
        stylesheetfile.close();
    }
    init();
    this->setLayout(mainlayout);
    initTable();
    initDialog();
    r_allfile->setChecked(true);
    b_opendirdialog->setEnabled(false);
    b_opensingalfile->setEnabled(false);
    r_userkeyword->setChecked(true);
    b_addkeyworddb->setEnabled(false);
    b_refreshdblist->setEnabled(false);
    cb_dblist->setEnabled(false);
    b_pause->setEnabled(false);
    b_stop->setEnabled(false);
    connect(secretmodel,&SecretScan::DBadded,this,&SecretScanWidget::setDBList);
    connect(this,&SecretScanWidget::CheckStart,secretmodel,&SecretScan::Start);
    connect(secretmodel,&SecretScan::CheckDone,this,&SecretScanWidget::ScanFinished);
    this->setObjectName("SecretScanWidget");
    secretmodel->moveToThread(modelthread);
    modelthread->setStackSize(104857600);        //100MB
    modelthread->start(/*QThread::HighestPriority*/);  //最高优先级
    secretmodel->ScanSicsDb();
}

void SecretScanWidget::init()
{
    mainlayout=new QGridLayout(this);
    l_selectdir=new QLabel("文件路径: ",this);
    l_selectdir->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    mainlayout->addWidget(l_selectdir,0,0);
    e_selectdir=new QLineEdit(this);
    e_selectdir->setFocusPolicy(Qt::NoFocus);
    e_selectdir->setPlaceholderText("全盘扫描");
    mainlayout->addWidget(e_selectdir,0,1,1,4);
    b_opendirdialog=new QPushButton("选择查找路径",this);
    connect(b_opendirdialog,&QPushButton::clicked,this,&SecretScanWidget::OpenDirDialog);
    mainlayout->addWidget(b_opendirdialog,1,3);
    b_opensingalfile=new QPushButton("选择指定文件",this);
    connect(b_opensingalfile,&QPushButton::clicked,this,&SecretScanWidget::OpenFileDialog);
    mainlayout->addWidget(b_opensingalfile,1,4);
    bg_scanmode=new QButtonGroup(this);
    r_allfile=new QRadioButton("全盘扫描",this);
    mainlayout->addWidget(r_allfile,0,5);
    bg_scanmode->addButton(r_allfile,0);
    r_selectdir=new QRadioButton("自定义扫描",this);
    mainlayout->addWidget(r_selectdir,1,5);
    bg_scanmode->addButton(r_selectdir,1);
    connect(bg_scanmode,&QButtonGroup::idClicked,this,&SecretScanWidget::PathModeChanged);
    l_selectkeyword=new QLabel("关键字: ",this);
    l_selectkeyword->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    mainlayout->addWidget(l_selectkeyword,2,0);
    e_insertkeyword=new QLineEdit(this);
    e_insertkeyword->setClearButtonEnabled(true);
    e_insertkeyword->setPlaceholderText("请输入关键字(多个关键字使用空格隔开)");
    mainlayout->addWidget(e_insertkeyword,2,1,1,4);
    bg_keywordmode=new QButtonGroup(this);
    r_userkeyword=new QRadioButton("自定义关键字",this);
    mainlayout->addWidget(r_userkeyword,2,5);
    bg_keywordmode->addButton(r_userkeyword,0);
    cb_dblist=new QComboBox(this);
    cb_dblist->setToolTip("请选择数据库");
    cb_dblist->setMaxVisibleItems(10);
    mainlayout->addWidget(cb_dblist,3,1,1,2);
    b_addkeyworddb=new QPushButton("添加新数据库",this);
    mainlayout->addWidget(b_addkeyworddb,3,3);
    b_refreshdblist=new QPushButton("刷新",this);
    mainlayout->addWidget(b_refreshdblist,3,4);
    connect(b_refreshdblist,&QPushButton::clicked,this,&SecretScanWidget::refreshDBList);
    r_localdb=new QRadioButton("数据库",this);
    bg_keywordmode->addButton(r_localdb,1);
    mainlayout->addWidget(r_localdb,3,5);
    connect(bg_keywordmode,&QButtonGroup::idClicked,this,&SecretScanWidget::KeyWordModeChanged);
    b_start=new QPushButton("开始",this);
    b_pause=new QPushButton("暂停",this);
    b_stop=new QPushButton("取消",this);
    mainlayout->addWidget(b_start,4,4);
    mainlayout->addWidget(b_pause,4,5);
    mainlayout->addWidget(b_stop,4,6);
    connect(b_start,&QPushButton::clicked,this,&SecretScanWidget::ScanStart);
    connect(b_pause,&QPushButton::clicked,this,&SecretScanWidget::ScanPause);
    connect(b_stop,&QPushButton::clicked,this,&SecretScanWidget::ScanStop);
    tv_secretfile=new QTableView(this);
    mainlayout->addWidget(tv_secretfile,5,0,1,8);
    fd_pathselector=new QFileDialog(this);
    fd_pathselector->setViewMode(QFileDialog::Detail);
    fd_pathselector->setModal(true);
    secretmodel=new SecretScan;
    modelthread=new QThread(this);
    sl_selected=new QStringList;
    userkeyword=new QStringList;
    elapsed=new QElapsedTimer;
    now=Stoped;
}

void SecretScanWidget::initTable()
{
    tv_secretfile->setObjectName("SecretTable");
    tv_secretfile->setModel(secretmodel->getSecretFileModel());
    tv_secretfile->setShowGrid(false);
    tv_secretfile->setSelectionBehavior(QTableView::SelectRows);
    tv_secretfile->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tv_secretfile->setVerticalScrollMode(QTableView::ScrollPerPixel);
    tv_secretfile->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tv_secretfile->setSelectionMode(QTableView::SingleSelection);
    tv_secretfile->setContextMenuPolicy(Qt::ActionsContextMenu);
    tv_secretfile->verticalHeader()->setVisible(false);
    tv_secretfile->horizontalHeader()->setHighlightSections(false);
    tv_secretfile->setFocusPolicy(Qt::NoFocus);
    copytvrow=new QAction("复制文件路径",this);
    openfile=new QAction("打开当前文件",this);
    openfilepath=new QAction("打开当前文件目录",this);
    tv_secretfile->addAction(copytvrow);
    tv_secretfile->addAction(openfile);
    tv_secretfile->addAction(openfilepath);
    connect(copytvrow,&QAction::triggered,this,&SecretScanWidget::copyTableData);
    connect(openfile,&QAction::triggered,this,&SecretScanWidget::OpenSelectedFile);
    connect(openfilepath,&QAction::triggered,this,&SecretScanWidget::OpenSelectedPath);
}

void SecretScanWidget::initDialog()
{
    dl_addDB=new QDialog(this,Qt::WindowCloseButtonHint);
    dl_addDB->setModal(true);
    dl_addDB->setWindowTitle("添加涉密关键词数据库");
    dl_addDB->resize(500,300);
    dl_layout=new QGridLayout;
    dl_addDB->setLayout(dl_layout);
    dl_l_dbname=new QLabel("数据库名称: ");
    dl_l_dbname->setAlignment(Qt::AlignCenter);
    dl_layout->addWidget(dl_l_dbname,0,0);
    dl_l_selectfile=new QLabel("数据库文件: ");
    dl_l_selectfile->setAlignment(Qt::AlignCenter);
    dl_layout->addWidget(dl_l_selectfile,1,0);
    dl_e_dbname=new QLineEdit;
    dl_e_dbname->setClearButtonEnabled(true);
    dl_e_dbname->setPlaceholderText("请填写数据库名称");
    dl_layout->addWidget(dl_e_dbname,0,1,1,2);
    dl_e_selectfile=new QLineEdit;
    dl_e_selectfile->setPlaceholderText("选择数据库文件路径");
    dl_e_selectfile->setFocusPolicy(Qt::NoFocus);
    dl_e_selectfile->setClearButtonEnabled(true);
    dl_layout->addWidget(dl_e_selectfile,1,1,1,2);
    dl_b_openselector=new QPushButton("选择文件");
    dl_layout->addWidget(dl_b_openselector,1,3);
    dl_b_OK=new QPushButton("确认");
    dl_b_cancel=new QPushButton("取消");
    dl_layout->addWidget(dl_b_OK,2,1);
    dl_layout->addWidget(dl_b_cancel,2,2);
    dl_fd_fileselector=new QFileDialog;
    dl_fd_fileselector->setViewMode(QFileDialog::Detail);
    dl_fd_fileselector->setFileMode(QFileDialog::ExistingFile);
    connect(b_addkeyworddb,&QPushButton::clicked,dl_addDB,&QDialog::show);
    connect(dl_b_openselector,&QPushButton::clicked,dl_fd_fileselector,&QFileDialog::exec);
    connect(dl_fd_fileselector,&QFileDialog::fileSelected,dl_e_selectfile,&QLineEdit::setText);
    connect(dl_b_OK,&QPushButton::clicked,this,&SecretScanWidget::AddNewDB);
    connect(this,&SecretScanWidget::InsertNewDB,secretmodel,&SecretScan::addKeyWordDataBase);
    connect(dl_b_cancel,&QPushButton::clicked,dl_addDB,&QDialog::close);
}

void SecretScanWidget::PathModeChanged(int id)
{
    if(id==0){
        b_opendirdialog->setEnabled(false);
        b_opensingalfile->setEnabled(false);
        e_selectdir->setPlaceholderText("全盘扫描");
    }
    else if(id==1){
        b_opendirdialog->setEnabled(true);
        b_opensingalfile->setEnabled(true);
        e_selectdir->setPlaceholderText("选择 扫描路径/单个文件 进行检查");
    }
}

void SecretScanWidget::KeyWordModeChanged(int id)
{
    if(id==0){
        cb_dblist->setEnabled(false);
        e_insertkeyword->setEnabled(true);
        b_addkeyworddb->setEnabled(false);
        b_refreshdblist->setEnabled(false);
    }else if(id==1){
        cb_dblist->setEnabled(true);
        e_insertkeyword->setEnabled(false);
        b_addkeyworddb->setEnabled(true);
        b_refreshdblist->setEnabled(true);
    }
}

void SecretScanWidget::OpenDirDialog()
{
    fd_pathselector->setFileMode(QFileDialog::Directory);
    QString temp=fd_pathselector->getExistingDirectory();
    e_selectdir->setText(temp);
    DirsOrFiles=false;
}

void SecretScanWidget::OpenFileDialog()
{
    fd_pathselector->setFileMode(QFileDialog::ExistingFiles);
    sl_selected->clear();
    sl_selected->append(fd_pathselector->getOpenFileNames());
    e_selectdir->setText(sl_selected->join(';'));
    DirsOrFiles=true;
}

void SecretScanWidget::AddNewDB()
{
    if(dl_e_dbname->text().isEmpty() || dl_e_selectfile->text().isEmpty()){
        QMessageBox::warning(this,"错误格式","请填写完整的数据库名称/路径");
        return;
    }
    emit InsertNewDB(dl_e_dbname->text(),dl_e_selectfile->text());
    dl_addDB->close();
}

void SecretScanWidget::ScanStart()
{
    if(bg_scanmode->checkedId()==1 && e_selectdir->text().isEmpty()){
        QMessageBox::warning(this,"信息不完整","请选择查找路径/文件");
        return;
    }
    if(bg_keywordmode->checkedId()==0 && e_insertkeyword->text().isEmpty()){ //自定义模式
        QMessageBox::warning(this,"信息不完整","请输入查找关键字");
        return;
    }else if(bg_keywordmode->checkedId()==1 && cb_dblist->currentText().isEmpty()){
        QMessageBox::warning(this,"信息不完整","请选择关键词数据库");
        return;
    }
    if(now==Stoped){      //停止状态启动，需要传递各种参数
        elapsed->start();
        now=Running;
        b_start->setEnabled(false);
        b_pause->setEnabled(true);
        b_stop->setEnabled(true);
        b_start->setText("继续");
        b_opendirdialog->setEnabled(false);
        b_opensingalfile->setEnabled(false);
        if(bg_scanmode->checkedId()==0){        //全盘扫描
            secretmodel->setAllFile(true);
        }else if(bg_scanmode->checkedId()==1){
            secretmodel->setAllFile(false);
            if(DirsOrFiles==false)
                secretmodel->setDirPath(e_selectdir->text());
            else if(DirsOrFiles==true)
                secretmodel->setFilePath(*sl_selected);
        }
        if(bg_keywordmode->checkedId()==0){ //自定义模式
            userkeyword->clear();
            userkeyword->append(e_insertkeyword->text().simplified().split(' '));
            secretmodel->setUserKeyWord(userkeyword);
        }else if(bg_keywordmode->checkedId()==1)
            secretmodel->setKeyWordDB(cb_dblist->currentText());
        emit CheckStart();
    }else if(now==Paused){
        b_start->setEnabled(false);
        b_pause->setEnabled(true);
        b_stop->setEnabled(true);
        secretmodel->MatchThreadResume();
    }
}

void SecretScanWidget::ScanPause()
{
    secretmodel->MatchThreadPause();
    b_start->setEnabled(true);
    b_pause->setEnabled(false);
    now=Paused;
}

void SecretScanWidget::ScanStop()
{
    secretmodel->MatchThreadStop();
}

void SecretScanWidget::ScanFinished()
{
    double emsec=static_cast<double>(elapsed->elapsed())/1000;
    b_start->setEnabled(true);
    b_pause->setEnabled(false);
    b_stop->setEnabled(false);
    b_start->setText("开始");
    now=Stoped;
    if(bg_scanmode->checkedId()==1){
        b_opendirdialog->setEnabled(true);
        b_opensingalfile->setEnabled(true);
    }
    QString temptip=QString("检测完成 耗时: %1S").arg(emsec);
    QMessageBox::information(this,"提示",temptip);
}

void SecretScanWidget::setDBList(const QStringList dblist)
{
    cb_dblist->clear();
    cb_dblist->addItems(dblist);
}

void SecretScanWidget::refreshDBList()
{
    secretmodel->ScanSicsDb();
}

void SecretScanWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget,&opt,&p,this);
}

void SecretScanWidget::copyTableData()
{
    QModelIndex index=tv_secretfile->selectionModel()->selectedRows().at(0);
    QString filepath=tv_secretfile->selectionModel()->model()->data(index,Qt::WhatsThisPropertyRole).toString();
    QClipboard *board=QApplication::clipboard();
    board->setText(filepath);
}

void SecretScanWidget::OpenSelectedFile()
{
    QModelIndex index=tv_secretfile->selectionModel()->selectedRows().at(0);
    QString filename=tv_secretfile->selectionModel()->model()->data(index,Qt::WhatsThisPropertyRole).toString();
    QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
}

void SecretScanWidget::OpenSelectedPath()
{
QModelIndex index=tv_secretfile->selectionModel()->selectedRows().at(0);
    QString abslutepath=tv_secretfile->selectionModel()->model()->data(index,Qt::DisplayPropertyRole).toString();
    QDesktopServices::openUrl(QUrl::fromLocalFile(abslutepath));
}

SecretScanWidget::~SecretScanWidget()
{
    delete secretmodel;
    modelthread->quit();
    modelthread->wait();
    delete modelthread;
    delete sl_selected;
    delete elapsed;
}
