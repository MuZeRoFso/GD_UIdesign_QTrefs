#pragma execution_character_set("utf-8")
#ifndef SECRETSCANWIDGET_H
#define SECRETSCANWIDGET_H
#include "Model/ACTreeModel.h"

class SecretScanWidget : public QWidget
{
    Q_OBJECT
public:
    enum CheckedStatus{
        Running,
        Paused,
        Stoped
    };
    Q_ENUM(CheckedStatus)
    explicit SecretScanWidget(QWidget *parent = nullptr);
    ~SecretScanWidget() override;
signals:
    void CheckStart();
    void CheckResume();
    void InsertNewDB(const QString &dbname,const QString &dbpath);
private:
    QGridLayout *mainlayout;        //主要布局 网格布局
    QLabel *l_selectdir;            //选择路径
    QLineEdit *e_selectdir;         //路径保存
    QPushButton *b_opendirdialog;   //打开文件夹选择窗口
    QPushButton *b_opensingalfile;  //打开文件选择窗口
    QRadioButton *r_allfile;        //全盘扫描模式
    QRadioButton *r_selectdir;      //选择路径模式
    QButtonGroup *bg_scanmode;      //全盘 or 自选的单选框分组
    QLabel *l_selectkeyword;        //选择关键词
    QLineEdit *e_insertkeyword;     //用户自输入关键词集
    QComboBox *cb_dblist;           //关键词数据库复选框
    QPushButton *b_addkeyworddb;    //添加关键词数据库
    QPushButton *b_refreshdblist;   //刷新数据库列表
    QRadioButton *r_localdb;        //选择已有数据库
    QRadioButton *r_userkeyword;    //用户输入关键词
    QButtonGroup *bg_keywordmode;   //数据库 or 用户输入关键字模式
    QPushButton *b_start;           //开始
    QPushButton *b_pause;           //暂停
    QPushButton *b_stop;            //取消
    QTableView *tv_secretfile;      //表格显示涉密文件
    QAction *copytvrow;             //复制文件路径
    QAction *openfile;              //打开当前文件
    QAction *openfilepath;          //打开文件所属文件夹
    QMap<QString,QString> dbmap;    //数据库与路径映射表
    SecretScan * secretmodel;       //数据处理模型
    QThread *modelthread;           //模型处理线程
    QFileDialog *fd_pathselector;   //路径选择窗口
    QStringList *sl_selected;       //文件路径列表
    bool DirsOrFiles;               //false为文件路径 true为文件
    CheckedStatus now;              //标记当前状态
    QStringList *userkeyword;       //用户自定义的关键词
    void init();
    void initTable();
    QDialog *dl_addDB;              //添加涉密数据库的对话框
    QGridLayout *dl_layout;         //数据库对话框布局
    QLabel *dl_l_dbname;            //数据库名
    QLineEdit *dl_e_dbname;         //填写数据库名
    QLabel *dl_l_selectfile;        //选择数据库文件
    QLineEdit *dl_e_selectfile;     //数据库文件路径
    QPushButton *dl_b_openselector; //打开文件选择对话框
    QFileDialog *dl_fd_fileselector;//文件选择对话框
    QPushButton *dl_b_OK;           //确认按钮
    QPushButton *dl_b_cancel;       //取消按钮
    QElapsedTimer *elapsed;
    void initDialog();
private slots:
    void PathModeChanged(int id);
    void KeyWordModeChanged(int id);
    void OpenDirDialog();
    void OpenFileDialog();
    void AddNewDB();
    void ScanStart();
    void ScanPause();
    void ScanStop();
    void ScanFinished();
    void setDBList(const QStringList dblist);
    void refreshDBList();
    void copyTableData();
    void OpenSelectedFile();
    void OpenSelectedPath();
    void paintEvent(QPaintEvent *event) override;
};

#endif // SECRETSCANWIDGET_H
