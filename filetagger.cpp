#include "filetagger.h"
#include "ui_filetagger.h"
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDesktopServices>
extern QString FILE_ARG;
float wordmatching(const QString &wordq1,const QString &wordq2)
{  // float c=2.45;
    //qDebug()<<c;
    char word1[200];
    char word2[200];
    strcpy(word1,wordq1.toStdString().c_str());
    strcpy(word2,wordq2.toStdString().c_str());

    int word1len, word2len ,consder1[200], consder2[200],k,i,j;
    for(i=0;i<200;i++)
    {
        consder1[i]=0;
        consder2[i]=0;
    }
    //tolower
    for(i=0;i<=strlen(word1);i++){
        if(word1[i]>=65&&word1[i]<=90)
            word1[i]=word1[i]+32;
    }
    //tolower
    for(i=0;i<=strlen(word2);i++){
        if(word2[i]>=65&&word2[i]<=90)
            word2[i]=word2[i]+32;
    }

    word1len=strlen(word1);
    word2len=strlen(word2);

    float score=0,skipped,count;
    int inik=0;
    int inij=0;
    int countstop=0;
    for (i=0; i<word1len;i++)
    {
        count=-1;
        k=i;
        countstop=0;
        skipped=0;
        for (j=0; j<word2len;j++)
        {

            if ((word1[k]==word2[j])&&(consder1[k]==0)&&(consder2[j]==0))
            {
                if (countstop==0 )
                {
                    if (count>=0 )
                    {  count=count*1.5+1-skipped/3;

                        consder1[k]=1 ;  consder2[j]=1;
                        consder1[inik]=1 ;  consder2[inij]=1;

                        skipped=0; }
                    else
                    {  inik=k;
                        inij=j;
                        count=count+1;
                    }
                    k=k+1;
                    if (k>word1len)
                        countstop=1;
                }
            }
            else
            {
                if(count>=0)
                    skipped=skipped+1;

            }
        }
        if(count>=0)
            score=score+count;
    }
    return(score);
}



FileTagger::FileTagger(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FileTagger)
{
    ui->setupUi(this);

    ui->tabWidget->setTabText(0, "CREATE NEW TAGS");
    ui->tabWidget->setTabText(1, "BROWSE TAGS");
   setWindowTitle("File Tagger (version 1.0)");

    ui->filename->setText(FILE_ARG);

    dbdir= QDir::homePath();
    dbdir=dbdir+"/.Filetagger";
    qDebug()<<dbdir;
    QDir dir(dbdir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    OPENDATABASE();



    //connect(ui->openfile,SIGNAL(clicked()), this, SLOT(GETFILENAME()));
    //connect(ui->openfolder,SIGNAL(clicked()), this, SLOT(GETFOLDERNAME()));
    connect(ui->ADD_TAG,SIGNAL(clicked()), this, SLOT(ADD_TAG_ACTION()));


    connect(ui->ENTER_TAG,SIGNAL(textChanged(QString)), this, SLOT(UPDATETAGLIST()));
    connect(ui->filename,SIGNAL(textChanged(QString)), this, SLOT(UPDATETAG()));
    connect(ui->ENTER_TAG_FIND,SIGNAL(textChanged(QString)), this, SLOT(SORTFILELIST()));
    connect(ui->PREVIOUS_TAGS, SIGNAL(itemClicked(QListWidgetItem*)),this, SLOT(AUTOCOMPLETETAG()));
    connect(ui->FILE_LIST, SIGNAL(itemClicked(QListWidgetItem*)),this, SLOT(OPEN_FILE()));


    connect(ui->ENTER_TAG,SIGNAL(returnPressed()), this, SLOT(AUTOCOMPLETETAG()));
    connect(ui->REMOVETAG ,SIGNAL(clicked()), this, SLOT(REMOVEFROMDATABASE()));
    UPDATETAG();
}

void FileTagger::OPEN_FILE()
{
    int LISTindex=ui->FILE_LIST->currentRow();

    QRegExp rx("#tags-:");

        QStringList queryi = DATABASE.at(LISTindex).split(rx);
        QString file=queryi.at(0);
        QDesktopServices::openUrl(QUrl::fromLocalFile(file));
}

void FileTagger::SAVEDATABASE()
{


 QFile file(dbdir+"/Filetagger_db");
 file.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text);
 QTextStream out(&file);
 for (int i = 0; i < DATABASE.size(); ++i)
 {
     out<<DATABASE.at(i)+"\n";
 }

}
void FileTagger::OPENDATABASE()
{
    DATABASE.clear();
    QFile file(dbdir+"/Filetagger_db");
    qDebug()<<dbdir;
    QString data;
    if (file.open(QFile::ReadOnly))
    {
        data = file.readAll();
        file.close();
        QStringList items =data.split("\n");
        for (int i = 0; i < items.size(); ++i)
        {
            if (items.at(i).size()>0)
            {
                DATABASE<<items.at(i);
                QRegExp rxi("#tags-:");
                QStringList queryi = items.at(i).split(rxi);
                QRegExp rx("(\\ |\\t)"); //RegEx for ' ' OR '\t'
                QStringList query = queryi.at(1).split(rx);
                ADDTAGS(query);
            }
        }

    }
    UPDATE_FILELIST();

}
void FileTagger::UPDATE_FILELIST()
{
    ui->FILE_LIST->clear();
    QRegExp rx("#tags-:");
    for (int i = 0; i < DATABASE.size(); ++i)
    {
        QStringList queryi = DATABASE.at(i).split(rx);

        ui->FILE_LIST->addItem(queryi.at(0) +" ["+queryi.at(1)+"]");

    }
    ui->FILE_LIST->scrollToTop();
}

void FileTagger::AUTOCOMPLETETAG()
{
    //qDebug()<<"autocomplete";
    QRegExp rx("(\\ |\\t)");
    QString tag= ui->ENTER_TAG->text().trimmed();
    QStringList query = tag.split(rx);
    QString finaltag;
    for(int j=0;j<query.size()-1;j++)
    {
        finaltag=finaltag+query.at(j)+" ";
    }
    int LISTindex=ui->PREVIOUS_TAGS->currentRow();
    if(LISTindex>=0 && LISTindex<TAGS.size() )
    {
        finaltag=finaltag+TAGS.at(LISTindex);
    }else if(TAGS.size() >0) {
        finaltag=finaltag+TAGS.at(0)+" ";
    }
    ui->ENTER_TAG->setText(finaltag);
}

void  FileTagger::UPDATETAG()
{
    QString tag;
    QString filename=ui->filename->text().trimmed();
    if (filename.size()<1){
        return;
    }

    filename.replace("file:///","/");

    if( filename.at(filename.length()-1) == '/' ) filename.remove( filename.length()-1, 1 );

    QRegExp rx("#tags-:");
    for(int j=0;j<DATABASE.size();j++)
    {
        QString Line=DATABASE.at(j);

        QStringList query = Line.split(rx);
        if (query.size()==2){
            if(query.at(0).trimmed()==filename.trimmed())
            {
                tag=query.at(1);
            }
        }

    }
    ui->ENTER_TAG->setText(tag);
}
void  FileTagger::SORTFILELIST()
{
    ui->PREVIOUS_TAGS->clear();
    QString find= ui->ENTER_TAG_FIND->text();

    QRegExp rx("#tags-:");
    if (find.length()>0){

        QString replacest;
        for (int i = 0; i < DATABASE.size()-1; ++i)
        {
            for (int j = i+1; j < DATABASE.size(); ++j)
            {
                QStringList queryi = DATABASE.at(i).split(rx);
                QStringList queryj = DATABASE.at(j).split(rx);

                if (wordmatching(queryj.at(1),find) > wordmatching(queryi.at(1),find))
                {
                    replacest=DATABASE.at(j);
                    DATABASE.replace(j,DATABASE.at(i) );
                    DATABASE.replace(i,replacest );

                }

            }
        }
        //update FILE_LIST IN SECOND TAB
        UPDATE_FILELIST();

    }
}
void  FileTagger::UPDATETAGLIST()
{
    ui->PREVIOUS_TAGS->clear();
    QString tag= ui->ENTER_TAG->text();
    QString find;
    ui->PREVIOUS_TAGS->clear();

    if (tag.length()>0){
        // QCharRef c = tag[tag.length()-1];
        // if(c==' '||c=='\t'){
        QRegExp rx("(\\ |\\t)");
        QStringList query = tag.split(rx);
        int i=query.size();
        find=query.at(i-1);


        QString replacest;
        for (int i = 0; i < TAGS.size()-1; ++i)
        {
            for (int j = i+1; j < TAGS.size(); ++j)
            {
                if (wordmatching(TAGS.at(j),find) > wordmatching(TAGS.at(i),find))
                {
                    replacest=TAGS.at(j);
                    TAGS.replace(j,TAGS.at(i) );
                    TAGS.replace(i,replacest );

                }

            }
        }
        ui->PREVIOUS_TAGS->addItems(TAGS);
        ui->PREVIOUS_TAGS->scrollToTop();


    }
}
void  FileTagger::ADDTAGS(QStringList tags)
{

    for(int j=0;j<tags.size();j++)
    {
        bool found=false;
        for(int i=0;i<TAGS.size();i++)
        {
            if(TAGS.at(i).trimmed()==tags.at(j).trimmed()){
                found=true;
            }
        }
        if (!found && tags.at(j).trimmed().size()>1 ) {
            TAGS<<tags.at(j).trimmed();
        }
    }

}
void FileTagger::REMOVEFROMDATABASE()
{
    QRegExp rx("#tags-:");
    int LISTindex=ui->FILE_LIST->currentRow();
    QStringList queryd = DATABASE.at(LISTindex).split(rx);
    if( LISTindex>=0 && LISTindex<DATABASE.size())
    {
        DATABASE.removeAt(LISTindex);
    }
    ui->history->addItem("REMOVED "+queryd.at(0)+"  -["+queryd.at(1)+"]");
    SAVEDATABASE();
    //update FILE_LIST IN SECOND TAB
    UPDATE_FILELIST();
}

void FileTagger::ADDTODATABASE(QString data)
{
    QRegExp rx("#tags-:");
    QStringList queryd = data.split(rx);
    int remove=-1;
    for(int j=0;j<DATABASE.size();j++)
    {
        QString Line=DATABASE.at(j);

        QStringList query = Line.split(rx);
        if (query.size()==2){
            if(query.at(0).trimmed()==queryd.at(0).trimmed())
            {
                remove=j;
            }
        }

    }
    QString prefix="Added ";
    if (remove>=0)
    {    DATABASE.removeAt(remove);
        prefix="Modified ";
    }

    DATABASE<<data;


    ui->history->addItem(prefix+" "+queryd.at(0)+"  -["+queryd.at(1)+"]");
    //update FILE_LIST IN SECOND TAB
    SAVEDATABASE();
    UPDATE_FILELIST();


}
void FileTagger::ADD_TAG_ACTION()
{
    OPENDATABASE();
    QString NEW_TAG= ui->ENTER_TAG->text().trimmed();
    QString file=ui->filename->text().trimmed();
    file.replace("file:///","/");
    if( file.at(file.length()-1) == '/' ) file.remove( file.length()-1, 1 );
    if(file.size()<2)
     {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Give File/Folder name");
        messageBox.setFixedSize(500,200);
        return;
    }
    if (!QDir(file).exists()&&!QFile(file).exists())
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","File/Folder does not exists");
        messageBox.setFixedSize(500,200);
        return;
    }
    if ( NEW_TAG.count(QRegExp("#tags-:"))>0)
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Tag contains string #tags-:");
        messageBox.setFixedSize(500,200);
        return;


    }
    QRegExp rx("(\\ |\\t)"); //RegEx for ' ' OR '\t'
    QStringList query = NEW_TAG.split(rx);
    int i=query.size();
    for(int j=0;j<i;j++)
    {

        QString STRING= query.at(j);
        if (NEW_TAG.size()<2) {
            QMessageBox messageBox;
            messageBox.critical(0,"Error","Tag is too small");
            messageBox.setFixedSize(500,200);

            return;
        }
        if ( STRING.count(QRegExp("[a-zA-Z]"))==0)
        {
            if (STRING.size()>1){
                QMessageBox messageBox;
                messageBox.critical(0,"Error","Tags should contain atleast one character");
                messageBox.setFixedSize(500,200);

                return;
            }
        }
    }

    ADDTAGS(query);


    if (QDir(file).exists()||QFile(file).exists()) {
        //qDebug()<<file;

        ADDTODATABASE(file+"#tags-:"+NEW_TAG);
    }





}

FileTagger::~FileTagger()
{
    delete ui;
}
