#include "ACTreeModel.h"

Q_GLOBAL_STATIC_WITH_ARGS(QByteArray,UTF8_HEAD,("\xef\xbb\xbf",3))
Q_GLOBAL_STATIC_WITH_ARGS(const QByteArray,UCS_BE_HEAD,("\xfe\xff",2))
Q_GLOBAL_STATIC_WITH_ARGS(const QByteArray,UCS_LE_HEAD,("\xff\xfe",2))

FileHandler::FileHandler(SecretScan *secretmodel)
{
    this->secretmodel=secretmodel;
    QDir licdir;
    if(!licdir.exists("lic"))
        licdir.mkdir("lic");
    this->setStackSize(31457280);       //设置栈大小30M
    connect(this,&FileHandler::finished,this,&FileHandler::deleteLater);        //自我销除
}

void FileHandler::run()
{
    isStoped=false,isPause=false;
    while(!secretmodel->b_AllFileCatch || !secretmodel->qs_allfilepath->isEmpty()){
        {
            QMutexLocker locker(secretmodel->matchMutex);
            if(!secretmodel->qs_allfilepath->isEmpty())
                filename=secretmodel->qs_allfilepath->dequeue(); 
        }
        while(isPause && !isStoped) yieldCurrentThread();
        if(isStoped) break;
        if(filename.size()!=0){
            setType();
            startExtract();
            filename.clear();
        }
    }
    QMutexLocker locker(secretmodel->matchMutex);
    secretmodel->FinishedAdd();
}

void FileHandler::startExtract()
{
    switch(type){
    case 0:     //txt
        HandleTXT();
        break;
    case 1:     //Word
        HandleWord();
        break;
    case 2:     //PDF
        HandlePdf();
        break;
    default:
        break;
    }
}

bool FileHandler::isAnalysable(const QString &filename)
{
    if(filename.size()==0)
        return false;
    static QMimeDatabase mime;
    QMimeType filetype=mime.mimeTypeForFile(filename,QMimeDatabase::MatchContent);
    QString filetypename=filetype.name();
    if(filetypename.startsWith("text/") ||
    filetypename.compare("application/msword")==0 ||
    filetypename.startsWith("application/vnd.openxmlformats-officedocument.wordprocessingml") ||
    filetypename.compare("application/rtf")==0 ||
    filetypename.compare("application/zip")==0 ||
    filetypename.startsWith("application/pdf"))
        return true;
    else
        return false;
}

void FileHandler::FormatFileSize(qint64 filesize, QString &formatstr)
{
    static const QStringList units{"B","KB","MB","GB","TB","PB"};     //容量单位
    static const qint64 mod=1024;
    static const int indexspan=units.size()-1;
    qint64 size=filesize,rest=0;
    ushort index=0;
    while(size>=mod && index<indexspan){
        rest=size%mod;
        size/=mod;
        index++;
    }
    if(rest>0)
        formatstr=QString("%1.%2%3").arg(size).arg(rest).arg(units.at(index));
    else
        formatstr=QString("%1%2").arg(size).arg(units.at(index));
}

void FileHandler::setType()
{
    QFileInfo file(filename);
    if(!file.exists()){
        type=None;
        return;
    }
    filetype=mime.mimeTypeForFile(filename,QMimeDatabase::MatchContent);
    QString filetypename=filetype.name();
    if(filetypename.startsWith("text/"))
        type=TXT;
    else if(filetypename.compare("application/msword")==0 ||
    filetypename.startsWith("application/vnd.openxmlformats-officedocument.wordprocessingml") ||
    filetypename.compare("application/rtf")==0 ||
    filetypename.compare("application/zip")==0)
        type=Word;
    else if(filetypename.startsWith("application/pdf"))
        type=PDF;
}

void FileHandler::HandleTXT()
{
    QFile txt(filename);
    QString Decode;
    bool useSystem=false;
    if(txt.exists() && txt.open(QIODevice::ReadOnly)){
        QByteArray decode=txt.read(3);
        txt.close();
        if(decode.startsWith(*UTF8_HEAD))
            Decode="UTF-8";
        else if(decode.startsWith(*UCS_BE_HEAD))
            Decode="UTF-16BE";
        else if(decode.startsWith(*UCS_LE_HEAD))
            Decode="UTF-16LE";
        else{
            Decode="UTF-8";
            useSystem=true;
        }
    }else
        return;
    SecretMap sm_txt;
    if(txt.open(QIODevice::ReadOnly)){
        QString tempstr;
        unsigned int nowstatus=0,prestatus=0;
        tempstr.reserve(4096);
        QTextStream ts_file(&txt);
        while(!txt.atEnd()){
            tempstr=ts_file.read(4096).simplified();
            secretmodel->actree->match(tempstr,nowstatus,prestatus,sm_txt);
        }
    }
    txt.close();
    if(useSystem){
        if(txt.open(QIODevice::ReadOnly)){
            QString tempstr;
            unsigned int nowstatus=0,prestatus=0;
            tempstr.reserve(4096);
            QTextStream ts_file(&txt);
            while(!txt.atEnd()){
                tempstr=ts_file.read(4096).simplified();
                secretmodel->actree->match(tempstr,nowstatus,prestatus,sm_txt);
            }
        }
        txt.close();
    }
    if(sm_txt.size()!=0)
        this->InsertOneRow(sm_txt);
}

void FileHandler::HandleWord()
{/*
    using Aspose::QtHelpers::Convert;
    using namespace Aspose::Words;
    using namespace System;
    unsigned int nowstatus=0,prestatus=0;
    SecretMap sm_word;
    try{
        SmartPtr<License> WordLic=MakeObject<License>();
        String LicFileName=Convert(QString("lic/Aspose.Total.Product.Family.lic"));
        WordLic->SetLicense(LicFileName);
        String asposefilestr(Convert(filename));
        SharedPtr<Document> doc=MakeObject<Document>(asposefilestr);
        QString tempstr=Convert(doc->GetText()).simplified();
        secretmodel->actree->match(tempstr,nowstatus,prestatus,sm_word);
    }catch (const Exception& exception){
        QString errmsg=Convert(exception.ToString());
        if(errmsg.compare("Aspose::Words::UnsupportedFileFormatException: Unsupported file format: Unknown")!=0)
            qDebug()<<errmsg;
    }
    if(sm_word.size()!=0)
        this->InsertOneRow(sm_word);*/
}

void FileHandler::HandlePdf()
{/*
    unsigned int nowstatus=0,prestatus=0;
    SecretMap sm_pdf;
    QString tempstr;
    using namespace foxit;
    using namespace pdf;
    PDFDoc doc(filename.toStdWString().c_str());
    ErrorCode pdfstatus=doc.Load();
    if(pdfstatus!=e_ErrSuccess)
        return;
    int pagecount=doc.GetPageCount(),charcount;
    for(int index=0;index<pagecount;index++){
        PDFPage page(doc.GetPage(index));
        page.StartParse();
        TextPage text(page);
        charcount=text.GetCharCount();
        if(charcount>0){
            tempstr=tempstr.fromUtf8(text.GetChars().UTF8Encode()).remove(re_rn);
            secretmodel->actree->match(tempstr,nowstatus,prestatus,sm_pdf);
        }
    }
    if(sm_pdf.size()!=0)
        this->InsertOneRow(sm_pdf);*/
}

void FileHandler::InsertOneRow(SecretMap &sm)
{
    QMutexLocker locker(secretmodel->insertMutex);
    secretmodel->insertRow(filename,sm);
}
