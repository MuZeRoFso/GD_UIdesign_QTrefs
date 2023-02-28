#pragma execution_character_set("utf-8")
#ifndef GENDB_WIDGET_H
#define GENDB_WIDGET_H
#include "Model/KeyWordDbModel.h"

class GenDB_Widget : public QWidget
{
    Q_OBJECT
public:
    explicit GenDB_Widget(QWidget *parent = nullptr);
    ~GenDB_Widget() override;
public slots:
    void OpenDirDialog();
    void OpenSourceFile();
    void StartGenDB();
    void GenDBdone(const QString &destfile,int insertcount);
    void DeleteCurrentRow();
    void GenDBError(const QString &errormsg);
    void ClearSetting();
signals:
    void SourceFileChanged(const QString &filename);
    void GenerateDest(const QString &destfile);
    void DeleteRows(const QModelIndexList &indexlist);
    void clearModelData();
private:
    QGridLayout *mainlayout;
    QLabel *selfdesc;
    QLabel *l_dbname;
    QLineEdit *e_dbname;
    QLabel *l_dbsavedir;
    QLineEdit *e_dbsavedir;
    QAction *act_delete;
    QPushButton *b_opendir;
    QLabel *l_sourcefile;
    QLineEdit *e_sourcefile;
    QPushButton *b_dbsource;
    QPushButton *b_ok;
    QPushButton *b_cancel;
    QTableView *tv_dbitem;
    QFileDialog *fd_selectdir;
    QThread *modelthread;
    GenDB *gendbmodel;
    void init();
    void initTable();
    void paintEvent(QPaintEvent *event) override;
};

#endif // GENDB_WIDGET_H
