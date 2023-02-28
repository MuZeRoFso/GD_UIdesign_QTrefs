#pragma execution_character_set("utf-8")
#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include <QMap>
#include <QFile>
#include <QTextStream>
#include <QByteArray>
#include <QMimeDatabase>
#include <QMimeType>
#include <QMutex>
#include <QThread>
#include <QRegularExpression>

class SecretScan;
class SecretInfo;
typedef QMap<unsigned int,SecretInfo*> SecretMap;
class FileHandler : public QThread
{
    friend class SecretScan;
    enum FileType{
        TXT=0,      //txt文档
        Word=1,     //word/Rtf文档
        PDF=2,      //PDF文档
        None,
    };
    Q_ENUM(FileType)
public:
    FileHandler(SecretScan *secretmodel);
    void run() override;

    static bool isAnalysable(const QString &filename);
    static void FormatFileSize(qint64 filesize, QString &formatstr);
signals:

private:
    QString filename;
    FileType type;
    SecretScan *secretmodel;
    QMutex pause;
    QMimeDatabase mime;
    QMimeType filetype;
    const QRegularExpression re_rn=QRegularExpression("\r\n");
    bool isStoped;
    bool isPause;
    void setType();
    void startExtract();
    void HandleTXT();
    void HandleWord();
    void HandlePdf();
    void InsertOneRow(SecretMap &sm);
};

#endif // FILEHANDLER_H
