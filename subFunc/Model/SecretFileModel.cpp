#include "SecretFileModel.h"

SecretFileModel::SecretFileModel(QObject *parent) : QAbstractTableModel(parent)
{
    for(int col=0;col<headerlist.size();col++)
        this->setHeaderData(col,Qt::Horizontal,headerlist.at(col));
}

QVariant SecretFileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role==Qt::DisplayRole && orientation==Qt::Horizontal)
        if(section<headerlist.size())
            return headerlist.at(section);
    return QVariant();
}

int SecretFileModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return modeldata.size();
}

int SecretFileModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return headerlist.size();
}

QVariant SecretFileModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if(role==Qt::DisplayRole && index.row()<modeldata.size() && index.column()<headerlist.size()){
        switch(index.column()){ //filename,filesize,hitkeyword,filepath
        case 0:
            return modeldata.at(index.row()).filename;
        case 1:
            return modeldata.at(index.row()).filesize;
        case 2:
            return modeldata.at(index.row()).hitkeyword;
        case 3:
            return modeldata.at(index.row()).filepath;
        }
    }else if(role==Qt::ToolTipRole && index.column()==2){
        return QString("命中总数: %1\n").arg(modeldata.at(index.row()).count).append(modeldata.at(index.row()).hitkeyword);
    }else if(role==Qt::WhatsThisPropertyRole){
        return modeldata.at(index.row()).filepath + '/' + modeldata.at(index.row()).filename;
    }else if(role==Qt::DisplayPropertyRole){
        return modeldata.at(index.row()).filepath;
    }
    return QVariant();
}

void SecretFileModel::clearData(const QModelIndex &parent)
{
    if(rowCount()>0){
        beginRemoveRows(parent,0,rowCount()-1);
        modeldata.clear();
        endRemoveRows();
    }
}

void SecretFileModel::insertRow(const _SecretFile &newitem, const QModelIndex &parent)
{
    int currentRow=modeldata.size();
    this->beginInsertRows(parent,currentRow,currentRow);
    this->modeldata.append(newitem);
    this->endInsertRows();
}
