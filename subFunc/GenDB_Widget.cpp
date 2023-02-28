#include "GenDB_Widget.h"

GenDB_Widget::GenDB_Widget(QWidget *parent) : QWidget(parent)
{
    QFile stylesheetfile(":/qss/source/qss/GenDB_Widget.css");
    if(stylesheetfile.open(QIODevice::ReadOnly)){
        this->setStyleSheet(stylesheetfile.readAll());
        stylesheetfile.close();
    }
    qRegisterMetaType<QModelIndexList>("QModelIndexList");
    init();
    initTable();
    this->setLayout(mainlayout);
    this->setObjectName("GenDB_Widget");
    gendbmodel->moveToThread(modelthread);
    modelthread->start(QThread::LowestPriority);
}

void GenDB_Widget::OpenDirDialog()
{
    fd_selectdir->setFileMode(QFileDialog::Directory);
    e_dbsavedir->setText(fd_selectdir->getExistingDirectory());
}

void GenDB_Widget::OpenSourceFile()
{
    fd_selectdir->setFileMode(QFileDialog::ExistingFile);
    QString temp=fd_selectdir->getOpenFileName();
    if(!temp.isEmpty()){
        e_sourcefile->setText(temp);
        emit SourceFileChanged(temp);
    }
}

void GenDB_Widget::init()
{
    mainlayout=new QGridLayout(this);
    selfdesc=new QLabel("文本文件转关键词数据库",this);
    selfdesc->setObjectName("SelfDesc");
    mainlayout->addWidget(selfdesc,0,0,1,7);
    l_dbname=new QLabel("生成数据库文件名: ",this);
    l_dbname->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    mainlayout->addWidget(l_dbname,1,1);
    e_dbname=new QLineEdit(this);
    e_dbname->setPlaceholderText("请输入生成的数据库文件名");
    e_dbname->setClearButtonEnabled(true);
    mainlayout->addWidget(e_dbname,1,2,1,3);
    l_dbsavedir=new QLabel("数据库保存路径: ",this);
    l_dbsavedir->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    mainlayout->addWidget(l_dbsavedir,2,1);
    e_dbsavedir=new QLineEdit(this);
    e_dbsavedir->setPlaceholderText("选择生成文件保存路径");
    e_dbsavedir->setFocusPolicy(Qt::NoFocus);
    mainlayout->addWidget(e_dbsavedir,2,2,1,3);
    b_opendir=new QPushButton("选择保存路径",this);
    mainlayout->addWidget(b_opendir,2,5);
    l_sourcefile=new QLabel("选择源文件: ",this);
    l_sourcefile->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    mainlayout->addWidget(l_sourcefile,3,1);
    e_sourcefile=new QLineEdit(this);
    e_sourcefile->setPlaceholderText("源文件路径(txt文件)");
    e_sourcefile->setFocusPolicy(Qt::NoFocus);
    mainlayout->addWidget(e_sourcefile,3,2,1,3);
    b_dbsource=new QPushButton("选择源文件",this);
    mainlayout->addWidget(b_dbsource,3,5);
    b_ok=new QPushButton("确认",this);
    mainlayout->addWidget(b_ok,4,4);
    b_cancel=new QPushButton("清空",this);
    mainlayout->addWidget(b_cancel,4,5);
    tv_dbitem=new QTableView(this);
    mainlayout->addWidget(tv_dbitem,5,0,1,7);
    fd_selectdir=new QFileDialog(this);
    modelthread=new QThread(this);
    gendbmodel=new GenDB;
    act_delete=new QAction("删除当前选项",this);
    connect(b_opendir,&QPushButton::clicked,this,&GenDB_Widget::OpenDirDialog);
    connect(b_dbsource,&QPushButton::clicked,this,&GenDB_Widget::OpenSourceFile);
    connect(this,&GenDB_Widget::SourceFileChanged,gendbmodel,&GenDB::ReadTxtFile);
    connect(b_ok,&QPushButton::clicked,this,&GenDB_Widget::StartGenDB);
    connect(b_cancel,&QPushButton::clicked,this,&GenDB_Widget::ClearSetting);
    connect(this,&GenDB_Widget::GenerateDest,gendbmodel,&GenDB::GenerateDBFile);
    connect(gendbmodel,&GenDB::GenerateDBdone,this,&GenDB_Widget::GenDBdone);
    connect(act_delete,&QAction::triggered,this,&GenDB_Widget::DeleteCurrentRow);
    connect(this,&GenDB_Widget::DeleteRows,gendbmodel,&GenDB::DeleteModelRows);
    connect(gendbmodel,&GenDB::GenDBError,this,&GenDB_Widget::GenDBError);
    connect(this,&GenDB_Widget::clearModelData,gendbmodel,&GenDB::clearModelData);
}

void GenDB_Widget::initTable()
{
    tv_dbitem->setObjectName("KeyWordTable");
    tv_dbitem->setModel(gendbmodel->getModel());
    tv_dbitem->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tv_dbitem->horizontalHeader()->setHighlightSections(false);
    tv_dbitem->verticalHeader()->setVisible(false);
    tv_dbitem->setFocusPolicy(Qt::NoFocus);
    tv_dbitem->setVerticalScrollMode(QTableView::ScrollPerPixel);
    tv_dbitem->setSelectionBehavior(QTableView::SelectRows);
    tv_dbitem->setContextMenuPolicy(Qt::ActionsContextMenu);
    tv_dbitem->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tv_dbitem->addAction(act_delete);
}

void GenDB_Widget::StartGenDB()
{
    if(e_sourcefile->text().isEmpty()){
        QMessageBox::warning(this,"信息不完整","请选择源文件");
        return;
    }else if(e_dbname->text().isEmpty()){
        QMessageBox::warning(this,"信息不完整","请给数据库文件命名");
        return;
    }else if(e_dbsavedir->text().isEmpty()){
        QMessageBox::warning(this,"信息不完整","请选择文件保存路径");
        return;
    }
    b_ok->setEnabled(false);
    b_cancel->setEnabled(false);
    b_opendir->setEnabled(false);
    b_dbsource->setEnabled(false);
    e_dbname->setEnabled(false);
    emit GenerateDest(e_dbsavedir->text()+QDir::separator()+e_dbname->text());
}

void GenDB_Widget::GenDBdone(const QString &destfile, int insertcount)
{
    if(destfile.isEmpty())
        return;
    b_ok->setEnabled(true);
    b_cancel->setEnabled(true);
    b_opendir->setEnabled(true);
    b_dbsource->setEnabled(true);
    e_dbname->setEnabled(true);
    QString tempstr=destfile + " 生成成功\n" + QString("共插入%1条数据").arg(insertcount);
    QMessageBox::information(this,"转换成功",tempstr);
}

void GenDB_Widget::DeleteCurrentRow()
{
    QModelIndexList indexlist=tv_dbitem->selectionModel()->selectedRows();
    emit DeleteRows(indexlist);
}

void GenDB_Widget::GenDBError(const QString &errormsg)
{
    QMessageBox::warning(this,"生成数据库错误",errormsg);
}

void GenDB_Widget::ClearSetting()
{
    e_dbname->clear();
    e_dbsavedir->clear();
    e_sourcefile->clear();
    emit clearModelData();
}

void GenDB_Widget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget,&opt,&p,this);
}

GenDB_Widget::~GenDB_Widget()
{
    modelthread->quit();
    modelthread->wait();
    delete gendbmodel;
}
