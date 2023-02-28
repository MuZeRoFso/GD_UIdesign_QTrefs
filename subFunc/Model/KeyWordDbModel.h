#pragma execution_character_set("utf-8")
#ifndef KEYWORDDBMODEL_H
#define KEYWORDDBMODEL_H
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QFile>
#include <QTableView>
#include <QGridLayout>
#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QPainter>
#include <QThread>
#include <QDebug>
#include <QHeaderView>
#include <QMessageBox>
#include <QAction>
#include <QAbstractTableModel>

class KeyWordDbModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class GenDB;
public:
    explicit KeyWordDbModel(QObject *parent = nullptr);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void insertRow(const QString &newItem,const QModelIndex &parent = QModelIndex());
    void removeRow(int row, const QModelIndex &parent = QModelIndex());
    void clearData(const QModelIndex &parent = QModelIndex());
    void DeleteRows(int row, int count, const QModelIndex &parent = QModelIndex());
private:
    QString header=QString("关键词");
    QStringList modeldata;
    int currentRow;
};

class GenDB :public QObject
{
    Q_OBJECT
public:
    explicit GenDB(QObject *parent=nullptr);
    KeyWordDbModel *getModel();
signals:
    void GenerateDBdone(const QString &destfile,int insertcount);
    void GenDBError(const QString &errormsg);
public slots:
    void ReadTxtFile(const QString &filename);
    void GenerateDBFile(const QString &destfile);
    void DeleteModelRows(const QModelIndexList &indexlist);
    void clearModelData();
private:
    QSqlDatabase db_manager;
    KeyWordDbModel *model;
    QString deleteTable;
    QString createTable;
    QString createIndex;
};

#endif // KEYWORDDBMODEL_H
