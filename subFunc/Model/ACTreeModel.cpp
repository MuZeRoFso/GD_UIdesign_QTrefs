#include "ACTreeModel.h"

SecretInfo::SecretInfo(const QString &keyword)
    :KeyWord(keyword),K_Count(1)
{
}

void SecretInfo::addCount() { K_Count++; }

QString SecretInfo::JoinKeyWord(SecretMap &sm, unsigned int &count)
{
    QList<SecretInfo *> sl;
    sl=sm.values();
    QString temp;
    count=0;
    foreach(SecretInfo * si,sl){
        temp.append(si->KeyWord).append(QString("-%1").arg(si->K_Count)).append(',');
        count+=si->K_Count;
        delete si;
    }
    sm.clear();
    temp.remove(-1,1);
    return temp;
}

AC_DAT::AC_DAT()
{
    KEY_WORD=new QStringList;
    root=new TrieNode;
    root->word=(QChar)-1;
    root->status=0;
    totalStatus=0;
    DATsize=0;
    DAT=nullptr;
    decode=new ushort[UNICODE_TOTAL]{0};
    adtostr=new QMap<TrieNode*,QString>;
    statoad=new QMap<unsigned int,TrieNode*>;
    output=new QMap<unsigned int,QString>;
    nodelist=new QList<TrieNode*>;
}

void AC_DAT::resetVar()
{
    for(int index=0;index<UNICODE_TOTAL;index++)
        decode[index]=0;
    if(DAT!=nullptr){
        delete [] DAT;
        DAT=nullptr;
    }
    totalStatus=0;
    DATsize=0;
    adtostr->clear();
    statoad->clear();
    output->clear();
}

void AC_DAT::LoadKeyWord(const QString &dbpath)
{
    QSqlDatabase db_manager;
    if(db_manager.contains("SICSDB"))
        db_manager= QSqlDatabase::database("SICSDB");
    else
        db_manager=QSqlDatabase::addDatabase("QSQLITE","SICSDB");
    db_manager.setDatabaseName(dbpath);
    db_manager.open();
    QSqlQuery query=QSqlQuery("SELECT key_str FROM secret_key_word;",db_manager);
    query.exec();
    KEY_WORD->clear();
    while(query.next())
        KEY_WORD->append(query.value(0).toString());
    query.clear();
    db_manager.close();
}

void AC_DAT::createAC(QStringList *patterns)
{
    QElapsedTimer timer;
    timer.start();
    resetVar();
    if(patterns==nullptr)
        patterns=KEY_WORD;
    int psize=patterns->size();
    qDebug()<<psize;
    for(int i=0;i<psize;i++)
        this->insertPattern(patterns->at(i));
    InitTrieInfo();
    qDebug()<<totalStatus+1;
    createBaseAndCheck();
    adjustStrMap();
    clearCache();
    resizeDAT();
    qDebug()<<output->size();
    qDebug()<<DATsize;
    qDebug()<<1.0*timer.nsecsElapsed()/1000000;
}

void AC_DAT::insertPattern(const QString &pattern)
{
    TrieNode *cur=root;
    int strlen=pattern.length();                    //记录当前模式串长度
    for(int index=0;index<strlen;index++){
        QChar c=pattern.at(index);                  //获取index位置上的QChar字符
        TrieNode* temp=cur;
        if(temp->child==nullptr){           //若无子节点，则表示当前字符不存在对应节点 创建
            temp->child=createNode(c);
            cur=temp->child;
        }else{      //如果存在子节点，则开始在子节点的bro节点中寻找相应的字符
            temp=temp->child;
            while(temp!=nullptr){
                if(temp->word==c){          //如果寻找到相同字符的节点 则不需要重新建立节点
                    cur=temp;
                    break;
                }else{      //不等于c字符时
                    if(temp->bro==nullptr){ //如果兄弟节点不存在 则直接创建
                        temp->bro=createNode(c);
                        cur=temp->bro;
                        break;
                    }else temp=temp->bro;   //兄弟节点存在 所以接下来遍历下一个
                }
            }
        }
    }
    cur->istail=true;       //输入字符串结束 标记istail为true
    adtostr->insert(cur,pattern);   //输入映射表
}

AC_DAT::TrieNode* AC_DAT::createNode(QChar c)
{
    TrieNode *temp=new TrieNode;
    nodelist->append(temp);     //加入节点记录
    temp->word=c;
    totalStatus++;              //此处不需要更改status值 因为当前的status值不影响计算结果 后续计算会直接重置status值
    return temp;
}

void AC_DAT::createBaseAndCheck()
{
    statoad->clear();
    statoad->insert(0,root);        //先将根节点加入其中 0-root
    DATsize=10*totalStatus;         //防止数组下标溢出 先默认分配10*总状态的存储空间
    DAT=new DATNode[DATsize];
    QQueue<TrieNode*> tq;
    TrieNode *temp=root;
    TrieNode *tempchild=temp->child;
    int firstcount=0;
    while(tempchild!=nullptr){      //先处理第一层的节点
        tq.enqueue(tempchild);
        firstcount++;
        unsigned int nextbase=decode[tempchild->word.unicode()];    //第一层的位置就是decode内的次序编号
        tempchild->status=nextbase;     //利用节点记录base中的下标 在遍历子节点时可直接搜索到base父节点的下标位置
        statoad->insert(tempchild->status,tempchild);
        if(tempchild->istail)   //先处理成第一层就结尾的字符串base
            changeBase(tempchild);
        DAT[nextbase].check=-1;
        tempchild=tempchild->bro;
    }
    while(!tq.isEmpty())        //接下来层序遍历剩下的层
        calNextBase(tq);
    for(int index=1;index<=firstcount;index++)
        DAT[index].check=0;
}

void AC_DAT::InitTrieInfo()
{
    QQueue<QChar> *cq=new QQueue<QChar>;              //字符存储列表
    bool *charFlag=new bool[UNICODE_TOTAL]{false};  //已读入标记 初始值设定全为false
    QQueue<TrieNode*> Tq;                           //层序遍历队列
    Tq.enqueue(root);                               //先将root节点压入队列中
    TrieNode *temp,*tempchild;
    while(!Tq.isEmpty()){
        temp=Tq.dequeue();              //读出下一个节点
        tempchild=temp->child;          //读取该节点的孩子节点
        while(tempchild!=nullptr){
            Tq.enqueue(tempchild);      //将子节点加入队列
            if(!charFlag[tempchild->word.unicode()]){       //当该子节点对应的字符c未读入时
                charFlag[tempchild->word.unicode()]=true;   //标记读入
                cq->enqueue(tempchild->word);    //并将节点加入cl队列 层序顺序加入
            }
            tempchild=tempchild->bro;   //获取temp子节点的下一个子节点
        }
    }
    int maxlen=cq->size();
    for(ushort i=1;i<=maxlen;i++)
        decode[cq->dequeue().unicode()]=i;    //i表示读入的先后次序 cl第一个就是1 最后一个则是maxlen
    delete [] charFlag;
    cq->clear();
    delete cq;
}

void AC_DAT::calNextBase(QQueue<TrieNode*> &tq)
{
    TrieNode* father=tq.dequeue(),*child=father->child;
    if(child==nullptr) return;      //不存在子节点则不需要计算
    QList<unsigned int> c_decode;
    while(child!=nullptr){
        c_decode.append(decode[child->word.unicode()]);     //将子节点的decode值保存
        tq.enqueue(child);          //同时将该节点加入队列
        child=child->bro;
    }
    unsigned int nextbase=0;
    int maxlen=c_decode.size();
    for(unsigned int k=1;k<DATsize;k++){      //寻找K值 能够让base[k+c_decode]==0且check[k+c_decode]==0
        bool flag=true;
        for(int i=0;i<maxlen;i++)
            if(DAT[k+c_decode.at(i)].base!=0||DAT[k+c_decode.at(i)].check!=0){
                flag=false;     //如果对应位置的base值或者是check值有其中一个不为0 则表示位置已经被使用掉了
                break;
            }
        if(flag){
            nextbase=k;
            break;
        }
    }
    unsigned int preindex=father->status;       //获取父节点的下标
    long long prebase=DAT[preindex].base;
    if(prebase<0)
        DAT[preindex].base=-static_cast<long long>(nextbase); //父节点base数组标记前面取到的k值
    else
        DAT[preindex].base=static_cast<long long>(nextbase);
    child=father->child;
    while(child!=nullptr){
        unsigned int newbase=nextbase+decode[child->word.unicode()];
        DAT[newbase].check=preindex;        //check记录父节点下标 作为后续验证使用
        child->status=newbase;
        findFail(father,child);
        if(child->istail)
            changeBase(child);
        statoad->insert(child->status,child);
        child=child->bro;
        }
}

void AC_DAT::findFail(const TrieNode *node,const TrieNode *child)
{
    long long newfail;
    TrieNode *prefail=statoad->value(DAT[node->status].fail);       //Fail节点
    do{
        newfail=haveChild(prefail,child->word);
        if(newfail==-1){
            prefail=statoad->value(DAT[prefail->status].fail);
        }else{
            DAT[child->status].fail=static_cast<unsigned int>(newfail);
            return;
        }
    }while(prefail!=root);
    DAT[child->status].fail=0;
}

long long AC_DAT::haveChild(const TrieNode *node,QChar c)
{
    TrieNode *child=node->child;
    while(child!=nullptr)
        if(child->word==c)
            return static_cast<long long>(child->status);
        else
            child=child->bro;
    return -1;
}

void AC_DAT::changeBase(TrieNode *node)
{
    unsigned int status=node->status;
    if(DAT[status].base==0)
        DAT[status].base=-static_cast<long long>(status);
    else
        DAT[status].base=-DAT[status].base;
}

void AC_DAT::adjustStrMap()
{   //利用statoad 和 adtostr 转换为 statostr
    output->clear();
    QList<TrieNode*> tempkeys=adtostr->keys();
    int keyscount=tempkeys.size();
    for(int i=0;i<keyscount;i++){
        TrieNode *temp=tempkeys.at(i);
        output->insert(statoad->key(temp),adtostr->value(temp));
    }
}

void AC_DAT::clearCache()
{   //清空树节点
    root->bro=nullptr;
    root->child=nullptr;
    int nodecount=nodelist->size();
    for(int i=0;i<nodecount;i++){
        TrieNode *tempdelete=nodelist->value(i);
        delete tempdelete;
    }
    nodelist->clear();
    statoad->clear();       //清空状态转节点地址映射表
    adtostr->clear();       //清空地址转字符串映射表
}

void AC_DAT::match(const QString &str, unsigned int &nowstatus, unsigned int &prestatus, SecretMap &sm)
{
    int strsize=str.size();
    long long tempbase;
    ushort code;
    for(int index=0;index<strsize;index++){
        code=decode[str.at(index).unicode()];     //获取对应的编号
        tempbase=DAT[nowstatus].base;
        nowstatus=(tempbase>0?tempbase:-tempbase)+code;      //base+code=下一状态
        if(nowstatus>=DATsize || DAT[nowstatus].check!=prestatus)     //跳转错误时 使用fail指针
            nowstatus=gotoFail(prestatus,code);  //读取fail指针
        if(DAT[nowstatus].base<0){     //如果是base值是负数 则表示当前状态属于一个模式串的末尾
            if(sm.contains(nowstatus)){
                SecretInfo *temp=sm.value(nowstatus);
                temp->addCount();
            }else{
                SecretInfo *secret=new SecretInfo(output->value(nowstatus));    //创建新的涉密信息对象
                sm.insert(nowstatus,secret);     //加入涉密信息列表
            }
        }
        prestatus=nowstatus;       //保存上一状态
    }
}

unsigned int AC_DAT::gotoFail(unsigned int status,ushort code)
{
    if(code==0)     //不存在的字符集 则返回根节点
        return 0;
    long long tempbase;
    unsigned int fail=DAT[status].fail,teststatus;     //获取当前的fail指针
    do{     //fail指针不是根节点的话就继续判断
        tempbase=DAT[fail].base;
        teststatus=(tempbase>0?tempbase:-tempbase)+code;
        if(DAT[teststatus].check!=fail)         //不是从fail节点跳转过来的/读入字符不是fail节点的孩子
            fail=DAT[fail].fail;        //fail指针指向当前节点的fail指针
        else       //如果存在子节点 那么就返回fail指针作为新状态
            return teststatus;
    }while(fail!=0);
    return 0;       //返回root节点
}

void AC_DAT::resizeDAT()
{
    for(finalsize=DATsize-1;finalsize>0;finalsize--)
        if(DAT[finalsize].base!=0)
            break;
    finalsize++;
    DATNode *temp=DAT;
    DAT=new DATNode[finalsize];
    memcpy(DAT,temp,finalsize*sizeof(DATNode));
    delete [] temp;
    DATsize=finalsize;
    //printTAT();
}

void AC_DAT::printTAT()
{
    for(unsigned int index=0;index<DATsize;index++)
        qDebug()<<DAT[index].base<<' '<<DAT[index].fail<<' '<<DAT[index].check;
}

AC_DAT::~AC_DAT()
{
    delete [] DAT;
    delete [] decode;
    adtostr->clear();
    delete adtostr;
    statoad->clear();
    delete statoad;
    output->clear();
    delete output;
    nodelist->clear();
    delete nodelist;
}

SecretScan::SecretScan()
{
    matchMutex=new QMutex;
    insertMutex=new QMutex;
    datamodel=new SecretFileModel;
    qs_allfilepath=new QQueue<QString>;
    sl_userkeyword=new QStringList;
    dir_selected=new QDir;
    dir_handler=new QDir;
    b_isAllFile=false;
    actree=new AC_DAT;
    m_keyworddb=new QMap<QString,QString>;
    filehandlerthreads=new QList<FileHandler*>;
    filehandlerthreads->reserve(u_threadcount);
}

void SecretScan::Start()
{
    datamodel->clearData();
    finishedcount=0;
    b_isStoped=false;
    b_isPaused=false;
    if(b_UserOrDB==false)     //判断如果是用户输入则利用keyword词集构建
        actree->createAC(sl_userkeyword);
    else if(b_UserOrDB==true)
        actree->createAC(nullptr);     //在界面选择数据库时已经载入对应的关键字数据库了 这里直接开始构建AC树即可
    b_AllFileCatch=false;       //标记上并未获取所有文件
    MatchThreadStart();
    if(b_isAllFile==true){        //如果是全盘检查则开始获取所有的文件
        AllFileinLocal();
    }else if(b_isAllFile==false){
        if(b_DirOrFiles){
            b_AllFileCatch=true;            //由于文件已经选定好了 所以直接标记所有文件已经读入队列
        }else{
            FindFilePathinBFS();        //层序遍历目录下所有文件
            b_AllFileCatch=true;        //执行完文件检索之后 标记所有文件得到读入
        }
    }
}

void SecretScan::insertRow(QString &filename, SecretMap &sm_file)
{
    QFileInfo fileinfo(filename);
    QString filesize,hitkeyword;
    FileHandler::FormatFileSize(fileinfo.size(),filesize);
    unsigned int count;
    hitkeyword=SecretInfo::JoinKeyWord(sm_file,count);
    _SecretFile sf_temp(fileinfo.fileName(),filesize,hitkeyword,fileinfo.absolutePath(),count);
    datamodel->insertRow(sf_temp);
}

void SecretScan::MatchThreadStart()
{
    finishedcount=0;
    datamodel->removeRows(0,datamodel->rowCount());
    datamodel->clearData();
    filehandlerthreads->clear();
    FileHandler *temp;
    for(unsigned short index=0;index<u_threadcount;index++){
        temp=new FileHandler(this);
        filehandlerthreads->append(temp);
        temp->start(/*QThread::HighPriority*/);
    }
}

void SecretScan::MatchThreadPause()
{
    FileHandler *temp;
    for(unsigned short index=0;index<u_threadcount;index++){
        temp=filehandlerthreads->at(index);
        temp->isPause=true;
    }
    b_isPaused=true;
    matchlocker=new QMutexLocker(matchMutex);
}

void SecretScan::MatchThreadResume()
{
    FileHandler *temp;
    for(unsigned short index=0;index<u_threadcount;index++){
        temp=filehandlerthreads->at(index);
        temp->isPause=false;
    }
    b_isPaused=false;
    delete matchlocker;
}

void SecretScan::MatchThreadStop()
{
    if(b_isPaused)
        delete matchlocker;
    b_isStoped=true;
    FileHandler *temp;
    for(unsigned short index=0;index<u_threadcount;index++){
        temp=filehandlerthreads->at(index);
        temp->isStoped=true;
    }
    QMutexLocker locker(matchMutex);
    qs_allfilepath->clear();
}

QAbstractTableModel * SecretScan::getSecretFileModel() const{return datamodel;}

void SecretScan::setFilePath(const QStringList &filenames)
{
    dir_selected->setPath("");
    qs_allfilepath->clear();       //清空
    foreach(QString temp,filenames)
        if(FileHandler::isAnalysable(temp))
            qs_allfilepath->enqueue(temp);
    b_DirOrFiles=true;
}

void SecretScan::setDirPath(const QString &dir)
{
    dir_selected->setPath(dir);
    b_DirOrFiles=false;
    qs_allfilepath->clear();       //清空 后续判断
}

void SecretScan::FindFilePathinBFS()
{
    qs_allfilepath->clear();
    QQueue<QString> dirqueue;
    QFileInfoList tempinfolist;
    dirqueue.enqueue(dir_selected->absolutePath());
    while(!dirqueue.isEmpty()){
        if(b_isStoped) return;
        dir_handler->setPath(dirqueue.dequeue());
        tempinfolist=dir_handler->entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
        {
            QMutexLocker locker(matchMutex);        //互斥锁
            foreach(QFileInfo temp,tempinfolist)
                if(FileHandler::isAnalysable(temp.absoluteFilePath()))
                    qs_allfilepath->enqueue(temp.absoluteFilePath());
        }
        tempinfolist=dir_handler->entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
        foreach(QFileInfo temp,tempinfolist)
            dirqueue.enqueue(temp.absoluteFilePath());
    }
}

void SecretScan::setAllFile(bool type){b_isAllFile=type;}

void SecretScan::AllFileinLocal()
{
    qs_allfilepath->clear();
    QQueue<QString> dirqueue;
    QFileInfoList tempinfolist;
    tempinfolist=QDir::drives();        //获取所有分区
    foreach(QFileInfo temp,tempinfolist)
        dirqueue.enqueue(temp.absoluteFilePath());  //当做文件目录压入队列
    while(!dirqueue.isEmpty()){
        if(b_isStoped)
            return;
        dir_handler->setPath(dirqueue.dequeue());
        tempinfolist=dir_handler->entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
        {
            QMutexLocker locker(matchMutex);        //互斥锁
            foreach(QFileInfo temp,tempinfolist)
                if(FileHandler::isAnalysable(temp.absoluteFilePath()))
                    qs_allfilepath->enqueue(temp.absoluteFilePath());
        }
        tempinfolist=dir_handler->entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
        foreach(QFileInfo temp,tempinfolist)
            dirqueue.enqueue(temp.absoluteFilePath());
    }
}

void SecretScan::FinishedAdd()
{
    finishedcount++;
    if(finishedcount==u_threadcount){
        filehandlerthreads->clear();
        qs_allfilepath->clear();
        emit CheckDone();
    }
}

void SecretScan::setUserKeyWord(QStringList *sl)
{
    b_UserOrDB=false;     //false为user模式
    sl_userkeyword->clear();
    sl_userkeyword->append(*sl);
}

void SecretScan::setKeyWordDB(const QString &key)
{
    b_UserOrDB=true;
    actree->LoadKeyWord(m_keyworddb->value(key));
}

void SecretScan::addKeyWordDataBase(const QString &dbname, const QString &dbpath)
{
    QDir db_dir=QDir();
    if(!db_dir.exists(DB_DIR))
        db_dir.mkdir(DB_DIR);
    QFile dbfile(dbpath);
    if(!dbfile.exists())
        return;
    QString newdbfile=DB_DIR + QDir::separator() + dbname;
    if(!dbfile.copy(newdbfile))
        return;
    m_keyworddb->insert(dbname,dbpath);
    emit DBadded(m_keyworddb->keys());
}

void SecretScan::ScanSicsDb()
{
    m_keyworddb->clear();
    QDir db_dir=QDir(DB_DIR);
    if(!db_dir.exists())
        return;
    QFileInfoList dbinfo=db_dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    foreach(QFileInfo temp,dbinfo)
        m_keyworddb->insert(temp.fileName(),temp.absoluteFilePath());
    emit DBadded(m_keyworddb->keys());
}
