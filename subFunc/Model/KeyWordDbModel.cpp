#include "KeyWordDbModel.h"

KeyWordDbModel::KeyWordDbModel(QObject *parent) : QAbstractTableModel(parent)
{
    this->setHeaderData(0,Qt::Horizontal,header);
}

QVariant KeyWordDbModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation==Qt::Horizontal && role==Qt::DisplayRole)
        if(section==0) return header;
    return QVariant();
}

int KeyWordDbModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return modeldata.size();
}

int KeyWordDbModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 1;
}

QVariant KeyWordDbModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if(role==Qt::DisplayRole){
        if(index.row()<modeldata.size() && index.column()==0)
            return modeldata.at(index.row());
    }else if(role==Qt::ToolTipRole || role==Qt::WhatsThisPropertyRole)
        return modeldata.at(index.row());
    return QVariant();
}

void KeyWordDbModel::insertRow(const QString &newItem, const QModelIndex &parent)
{
    if(newItem.size()==0 || modeldata.contains(newItem)) return;
    currentRow=modeldata.size();
    beginInsertRows(parent,currentRow,currentRow);
    modeldata.append(newItem);
    endInsertRows();
}

void KeyWordDbModel::removeRow(int row, const QModelIndex &parent)
{
    if(row>rowCount()-1) return;
    beginRemoveColumns(parent,row,row);
    modeldata.removeAt(row);
    endRemoveColumns();
}

void KeyWordDbModel::clearData(const QModelIndex &parent)
{
    if(rowCount()>0){
        beginRemoveRows(parent,0,rowCount()-1);
        modeldata.clear();
        endRemoveRows();
    }
}

void KeyWordDbModel::DeleteRows(int row, int count, const QModelIndex &parent)
{
    if(row+count-1>rowCount()) return;
    beginRemoveRows(parent,row,row+count-1);
    for(int i=0;i<count;i++)
        modeldata.removeAt(row);
    endRemoveRows();
}

GenDB::GenDB(QObject *parent) : QObject(parent)
{
    model=new KeyWordDbModel(this);
    if(db_manager.contains("GenDB"))
        db_manager= QSqlDatabase::database("GenDB");
    else
        db_manager=QSqlDatabase::addDatabase("QSQLITE","GenDB");
    deleteTable=QString("drop table if exists secret_key_word;");
    createTable=QString("create table if not exists secret_key_word(key_str text not null "
    "constraint secret_key_word_pk primary key);");
    createIndex=QString("create unique index secret_key_word_key_str_uindex "
    "on secret_key_word (key_str);");
}

KeyWordDbModel *GenDB::getModel(){ return model; }

void GenDB::ReadTxtFile(const QString &filename)
{
    QFile input(filename);
    QString temp;
    if(input.open(QIODevice::ReadOnly)){
        temp=input.readAll().simplified();
        input.close();
    }else
        return;
    model->clearData();
    QStringList templist=temp.split(' ');
    foreach(QString word,templist)
        model->insertRow(word);
}

void GenDB::GenerateDBFile(const QString &destfile)
{
    QFile outputfile(destfile);
    if(!outputfile.exists()){
        outputfile.open(QIODevice::NewOnly);
        outputfile.close();
    }else{
        outputfile.remove();
        outputfile.open(QIODevice::NewOnly);
        outputfile.close();
    }
    db_manager.setDatabaseName(destfile);
    db_manager.open();
    QSqlQuery query(db_manager);
    if(!query.exec(deleteTable) || !query.exec(createTable) || !query.exec(createIndex)){
        emit GenDBError(query.lastError().text());
        return;
    }
    QStringList &modeldata=model->modeldata;
    int insertcount=modeldata.size();
    foreach(const QString &temp,modeldata)
        query.exec(QString("insert into secret_key_word (key_str) values (\"%1\");").arg(temp));
    db_manager.close();
    emit GenerateDBdone(destfile,insertcount);
}

void GenDB::DeleteModelRows(const QModelIndexList &indexlist)
{
    model->DeleteRows(indexlist.at(0).row(),indexlist.size());
}

void GenDB::clearModelData()
{
    model->clearData();
}
