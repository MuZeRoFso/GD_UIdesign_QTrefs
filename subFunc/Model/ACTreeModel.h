#ifndef ACTREEMODEL_H
#define ACTREEMODEL_H
#include <QApplication>
#include <QStringList>
#include <QQueue>
#include <QMap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDir>
#include <QThread>
#include <QWidget>
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QComboBox>
#include <QTableView>
#include <QHeaderView>
#include <QDialog>
#include <QAction>
#include <QPainter>
#include <QMessageBox>
#include <QClipboard>
#include <QElapsedTimer>
#include <QDesktopServices>
#include <QDebug>
#include"SecretFileModel.h"
#include"FileHandler.h"
typedef QMap<unsigned int,SecretInfo*> SecretMap;       //涉密列表
class SecretInfo
{
public:
    SecretInfo(const QString &keyword);
    void addCount();
    static QString JoinKeyWord(SecretMap &sm, unsigned int &count);
private:
    QString KeyWord;
    unsigned int K_Count;
};

class AC_DAT
{
public:
    AC_DAT();
    ~AC_DAT();
    void createAC(QStringList *patterns=nullptr);  //构造前缀树
    void LoadKeyWord(const QString &dbpath);     //读取关键词数据库
    void match(const QString &str,unsigned int &nowstatus,unsigned int &prestatus,SecretMap &sm);   //模式匹配算法
private:
    struct TrieNode{
        QChar word;         //Unicode存储中文
        bool istail=false;        //是否是末尾字符
        TrieNode *bro=nullptr;      //兄弟节点 同前缀的模式串分支
        TrieNode *child=nullptr;     //模式串的下一个字符
        unsigned int status;      //状态
    };
    struct DATNode{
        long long base=0;             //存储状态
        unsigned int fail=0;        //fail指针
        unsigned int check=0;            //检验跳转是否合法
    };
    const ushort UNICODE_TOTAL=USHRT_MAX;
    QStringList *KEY_WORD;   //关键字存储列表
    TrieNode *root;         //AC自动机根节点  goto表
    unsigned int totalStatus;     //总状态
    unsigned int DATsize,finalsize;
    ushort *decode;             //读入字符序列
    DATNode *DAT;               //DAT 双数组Trie树
    QMap<TrieNode*,QString> *adtostr;      //地址键值映射字符串地址
    QMap<unsigned int,TrieNode*> *statoad;                //状态统计映射表
    QMap<unsigned int,QString> *output;         //DAT树下标映射字符串地址
    QList<TrieNode*> *nodelist;                     //用于存储所有节点 最后释放
    void resetVar();                                //重置各种变量指针 避免内存泄露
    void insertPattern(const QString &pattern);       //插入一条新的模式串
    TrieNode *createNode(QChar c);
    void InitTrieInfo();                            //层序遍历更新节点信息，同时读入字符并做字符编码
    void createBaseAndCheck();                      //构造Base、Check数组
    void calNextBase(QQueue<TrieNode*> &tq);
    void changeBase(TrieNode *node);                //根据是否为尾节点更新base值
    void findFail(const TrieNode *node,const TrieNode *child);   //搜寻fail指针
    long long haveChild(const TrieNode *node,QChar c);                //判定节点是否有c的节点
    void adjustStrMap();
    void clearCache();                  //清空缓存 如节点和指针 防止内存泄露
    unsigned int gotoFail(unsigned int status,ushort code);     //寻找fail指针
    void resizeDAT();                   //重新分配DAT数组
    void printTAT();
};

class SecretScan : public QObject
{
    Q_OBJECT
    friend class FileHandler;
public:
    SecretScan();
    QAbstractTableModel * getSecretFileModel() const;
    void ScanSicsDb();
    void setFilePath(const QStringList &filenames);
    void setDirPath(const QString &dir);
    void setAllFile(bool type);
    void addKeyWordDataBase(const QString &dbname,const QString &dbpath);
    void setUserKeyWord(QStringList *sl);
    void setKeyWordDB(const QString &key);
signals:
    void matchStart();
    void DBadded(const QStringList dblist);
    void CheckDone();
public slots:
    void Start();
    void insertRow(QString &filename,SecretMap &sm_file);
    void MatchThreadStart();
    void MatchThreadPause();
    void MatchThreadResume();
    void MatchThreadStop();
private:
    SecretFileModel *datamodel;     //检测到涉密信息的文件模型
    QQueue<QString> *qs_allfilepath;       //所有被检测的文件
    QStringList *sl_userkeyword;
    QDir *dir_selected;
    const QString DB_DIR=QString("SICSDB");
    QDir *dir_handler;
    bool b_isAllFile;
    bool b_UserOrDB;
    bool b_DirOrFiles;
    bool b_AllFileCatch;      //标志读取状态是否完成 false为未读取完成 true表示读取完成 子线程消费者读取处理
    bool b_isStoped;
    bool b_isPaused;
    AC_DAT *actree;
    QMap<QString,QString> *m_keyworddb;   //关键字数据库映射 数据库名->数据库文件绝对路径
    QString k_nowkey;
    const unsigned short u_threadcount=4;
    unsigned short finishedcount;
    QList<FileHandler*> *filehandlerthreads;
    QMutex *matchMutex;
    QMutexLocker<QMutex> *matchlocker;
    QMutex *insertMutex;
    void FindFilePathinBFS();
    void AllFileinLocal();
    void FinishedAdd();
};

#endif // ACTREEMODEL_H
