#pragma execution_character_set("utf-8")
#ifndef SECRETFILEMODEL_H
#define SECRETFILEMODEL_H
#include <QAbstractTableModel>
#include <QDir>
struct _SecretFile;
class SecretFileModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit SecretFileModel(QObject *parent = nullptr);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    void insertRow(const _SecretFile &newitem, const QModelIndex &parent = QModelIndex());
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void clearData(const QModelIndex &parent = QModelIndex());
private:
    QStringList headerlist{"文件名","文件大小","命中关键词","文件路径"};
    QList<_SecretFile> modeldata;
};

struct _SecretFile{
    _SecretFile(QString filename,QString &filesize,
    QString &hitkeyword,QString filepath,unsigned int count):
    filename(filename),filesize(filesize),
    hitkeyword(hitkeyword),filepath(filepath),count(count){}
    QString filename,filesize,hitkeyword,filepath;
    unsigned int count;
};

#endif // SECRETFILEMODEL_H
