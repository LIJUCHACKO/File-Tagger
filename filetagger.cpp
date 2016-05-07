/*
    filetagger.cpp is part of FILETAGGER.

    FILETAGGER is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FILETAGGER is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License.
    If not, see <http://www.gnu.org/licenses/>.

*/





#include <QThread>
#include <QTime>
#include "filetagger.h"
#include "ui_filetagger.h"
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDesktopServices>

extern QString FILE_ARG;
float wordmatching(const QString &wordq1,const QString &wordq2)
{
    char word1[200];
    char word2[200];
    strcpy(word1,wordq1.toStdString().c_str());
    strcpy(word2,wordq2.toStdString().c_str());

    int word1len, word2len ,k,i,j;

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
    float finalscore=0;
    int jstart;
    for (jstart=0; jstart<word2len;jstart++)
    {

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
            for (j=jstart; j<word2len;j++)
            {

                if ((word1[k]==word2[j]))
                {
                    if (countstop==0 )
                    {
                        if (count>=0 )
                        {  count=count*1.5+1-skipped/3;

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
        if (finalscore<score)
        {
            finalscore=score;
        }
    }
    return(finalscore);
}



FileTagger::FileTagger(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FileTagger)
{
    setAcceptDrops(true);
    ui->setupUi(this);
    ui->tabWidget->setTabText(0, "CREATE NEW TAGS");
    ui->tabWidget->setTabText(1, "BROWSE TAGS");
    stoptagsorting=false;
    ui->REMOVETAG->setEnabled(false);
    setWindowTitle("File Tagger");
#ifdef WINDOWS
    ui->version->setText("2.4 (windows)");
#else
    ui->version->setText("2.4 (linux)");
#endif
    if( FILE_ARG.size()<1){
        ui->tabWidget->setCurrentIndex(1);
    }else{
        ui->tabWidget->setCurrentIndex(0);
    }
    ui->AproximateradioButton->setChecked(true);
    ui->virtualFS->setEnabled(false);
    ui->filename->setText(FILE_ARG);
    dbdir= QDir::homePath();
    dbdir=dbdir+"/.Filetagger";
    qDebug()<<dbdir;
    QDir dir(dbdir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    //lockfile
    activityover=false;
    QFile file(dbdir+"/.lockfile");
    file.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text);
    QTextStream out(&file);

    out<<QDateTime::currentDateTime().toString();
    watcher = new QFileSystemWatcher();
    watcher->addPath(dbdir+"/.lockfile");

    OPENDATABASE();
    OPENHISTORY();
    connect(ui->ADD_TAG,SIGNAL(clicked()), this, SLOT(SAVE_TAG_ACTION()));
    connect(ui->ENTER_TAG,SIGNAL(textChanged(QString)), this, SLOT(UPDATETAGLIST()));
    connect(ui->filename,SIGNAL(textChanged(QString)), this, SLOT(UPDATETAG()));
    connect(ui->ENTER_TAG_FIND,SIGNAL(textChanged(QString)), this, SLOT(SORTFILELIST()));
    connect(ui->PREVIOUS_TAGS, SIGNAL(itemClicked(QListWidgetItem*)),this, SLOT(AUTOCOMPLETETAG()));
    connect(ui->FILE_LIST, SIGNAL(itemClicked(QListWidgetItem*)),this, SLOT(OPEN_FILE()));
    connect(ui->ENTER_TAG,SIGNAL(returnPressed()), this, SLOT(AUTOCOMPLETETAG()));
    connect(ui->REMOVETAG ,SIGNAL(clicked()), this, SLOT(REMOVEFROMDATABASE()));
    connect(ui->CHECKFILES,SIGNAL(clicked()), this, SLOT(Check_FILELIST()));
    connect(watcher,SIGNAL(fileChanged(QString)),SLOT(exitapp()));
    connect(ui->updates,SIGNAL(clicked()), this, SLOT(checkupdates()));
    connect(ui->ExactradioButton2 ,SIGNAL(pressed()),this,SLOT(enablevirtualfilesysbut()));
    connect(ui->AproximateradioButton ,SIGNAL(pressed()),this,SLOT(disablevirtualfilesysbut()));
    connect(ui->virtualFS ,SIGNAL(clicked()), this, SLOT(createvirtualfilesys()));
    UPDATETAG();

    //filename autocompletion
    completer = new QCompleter();

    QFileSystemModel *fsModel = new QFileSystemModel(completer);
    completer->setModel(fsModel);
    fsModel->setRootPath("");
    ui->filename->setCompleter(completer);

}

void FileTagger::createvirtualfilesys()
{
    QString hmdir= QDir::homePath();
    bool ok;
    QString foldername = QInputDialog::getText( this,"Virtual file system", "Enter folder name:", QLineEdit::Normal,QString::null, &ok );
    if (!foldername.isEmpty() ) {
        // user entered something and pressed OK


        QString selfolder= QFileDialog::getExistingDirectory(0,"Choose Location",hmdir);
        QDir dir(selfolder+"/"+foldername);
        if (ok && !dir.exists()) {
            dir.mkpath(".");
        }
        QRegExp rxi("#tags-:");
        QString filename;
        QStringList filesep;
        for (int i = 0; i < PRESENTDATABASE.size(); ++i)
        {
            QStringList filepath = PRESENTDATABASE.at(i).split(rxi);
            QString file=filepath.at(0);
#ifdef WINDOWS
            QRegExp fileseparator("/");
            filesep = file.replace("\\","/").split(fileseparator);
            file.replace("/","\\");
            selfolder.replace("/","\\");
            filename=filesep.at(filesep.size()-1);
            QFile::link (file,  selfolder+"\\"+foldername+"\\"+filename+".lnk" );

#else
            QRegExp fileseparator("/");
            filesep = file.split(fileseparator);
            filename=filesep.at(filesep.size()-1);
            QFile::link (file,  selfolder+"/"+foldername+"/"+filename );
#endif
        }
        QDesktopServices::openUrl(QUrl::fromLocalFile(selfolder+"/"+foldername));
        HISTORY<<"Created virtual folder "+selfolder+"/"+foldername;
        ui->history->addItem("Created virtual folder "+selfolder+"/"+foldername);
        ui->history->scrollToBottom();
        SAVEHISTORY();

    }
}

void FileTagger::disablevirtualfilesysbut()
{
    ui->virtualFS->setEnabled(false);
    SORTApproxiFILELIST();
    ui->FILELIST_LABEL->setText("FILES/FOLDERS/WEBSITES   - SORTED BY APPROXIMATE STRING MATCHING ALGORITHM   (CLICK TO OPEN)");
}
void FileTagger::enablevirtualfilesysbut()
{
    ui->virtualFS->setEnabled(false);
    SORTExactFILELIST();
    ui->FILELIST_LABEL->setText("FILES/FOLDERS/WEBSITES   (CLICK TO OPEN)");
}

void FileTagger::checkupdates()
{

#ifdef WINDOWS
    QDesktopServices::openUrl(QUrl("https://sourceforge.net/projects/file-tagger/files/MS%20Windows/"));
#else
    QDesktopServices::openUrl(QUrl("https://sourceforge.net/projects/file-tagger/files/Ubuntu14.04_amd64/"));
#endif
}

void FileTagger::exitapp()
{
    if (activityover)
        exit(1);

    activityover=true;
}
void FileTagger::OPENHISTORY()
{
    HISTORY.clear();
    QFile file(dbdir+"/Filetagger_history");
    qDebug()<<dbdir;
    QString data;
    if (file.open(QFile::ReadOnly))
    {
        data = file.readAll();
        file.close();
        QStringList items =data.split("\n");
        for (int i =(items.size()-15); i < items.size(); ++i)
        {
            if (i>=0)
            {
                if (items.at(i).size()>0)
                {
                    HISTORY<<items.at(i).trimmed();
                    ui->history->addItem(items.at(i).trimmed());

                }
            }
        }

    }
    ui->history->scrollToBottom();
}
void FileTagger::SAVEHISTORY()
{


    QFile file(dbdir+"/Filetagger_history");
    file.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text);
    QTextStream out(&file);
    for (int i = 0; i < HISTORY.size(); ++i)
    {
        out<<HISTORY.at(i)+"\n";
    }


}


void FileTagger::dropEvent(QDropEvent *ev)
{

    const QMimeData *mimeData = ev->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        QString text = urlList.at(0).path();
        if( text.at(text.length()-1) == '/' ) text.remove( text.length()-1, 1 );
#ifdef WINDOWS
        text.replace("/","\\");
        if( text.at(0) == '\\' ) text=text.right(text.size()-1);
        if( text.at(text.length()-1) == '\\' ) text.remove( text.length()-1, 1 );


#endif
        ui->filename->setText(text);

        ui->tabWidget->setCurrentIndex(0);
        qDebug()<<text;
    }




}

void FileTagger::dragEnterEvent(QDragEnterEvent *ev)
{


    ev->acceptProposedAction();
    //emit changed(ev->mimeData());
}


void FileTagger::OPEN_FILE()
{

    int LISTindex=ui->FILE_LIST->currentRow()/2;
    ui->REMOVETAG->setEnabled(false);
    if (ui->FILE_LIST->currentRow()%2==1)
        ui->REMOVETAG->setEnabled(true);
    if (ui->FILE_LIST->currentRow()%2>0)
        return;

    QRegExp rx("#tags-:");

    QStringList queryl = DATABASE.at(LISTindex).split(rx);
    QString file=queryl.at(0);
    if(file.size()>4){
        QString subString=file.left(3);
        if (subString=="htt"){
            QDesktopServices::openUrl(QUrl(file));

        } else{
            if (QDir(file).exists()||QFile(file).exists()) {
                QDesktopServices::openUrl(QUrl::fromLocalFile(file));
            } else {
                qDebug()<<"cannot open path";
                QMessageBox::StandardButton reply = QMessageBox::question(this, "File/folder missing ", file+" missing\n Is it a file ?",
                                                                          QMessageBox::Yes|QMessageBox::No);
                QString filenamenew;
                if (reply == QMessageBox::Yes) {
                    filenamenew =  QFileDialog::getOpenFileName(this,
                                                                tr("Choose File"), QDir::homePath());

                }else if (reply == QMessageBox::No){
                    filenamenew= QFileDialog::getExistingDirectory(0,"Choose Directory",QString(),QFileDialog::ShowDirsOnly);
                }else {
                    return ;
                }
#ifdef WINDOWS
                filenamenew.replace("/","\\");
                file.replace("/","\\");
#endif

                qDebug()<<filenamenew;
                if (filenamenew.size()<3)
                    return;
                /* reassigning file path */
                int sameupto=-1;bool same=true;
                for(int i=filenamenew.size()-1;i>=0;i--)
                {
                    if((file.size()-(filenamenew.size()-i))>=0) {
                        if ((filenamenew[i]==file[file.size()-(filenamenew.size()-i)])&&same)
                        {
                            sameupto=i;
                        }else{
                            same=false;
                        }
                    }
                }
                if(sameupto==-1)
                {  DATABASE<<filenamenew+"#tags-:"+queryl.at(1);
                    ui->history->addItem("Added "+filenamenew+"#tags-:"+queryl.at(1));
                    ui->history->scrollToBottom();
                    HISTORY<<"Added "+filenamenew+"#tags-:"+queryl.at(1);
                    ui->tabWidget->setCurrentIndex(0);
                    DATABASE.removeAt(LISTindex);
                    SAVEDATABASE();
                    UPDATE_FILELIST();
                    return;
                }
                QString orginalpath=file.left(file.size()-(filenamenew.size()-sameupto)+1);
                qDebug()<<"orgp "+orginalpath;
                QString newpath=filenamenew.left(sameupto+1);
                qDebug()<<"newp "+newpath;
                QRegExp rx("#tags-:");
                QStringList NEWDATABASE;
                for (int i = 0; i < DATABASE.size(); ++i)
                {
                    QStringList queryi = DATABASE.at(i).split(rx);

                    ui->FILE_LIST->addItem(queryi.at(0) +" ["+queryi.at(1)+"]");
                    if(queryi.at(0).size()>4){
                        QString subString=queryi.at(0).left(3);
                        if (subString=="htt"){
                            qDebug()<<"is a http link";
                            NEWDATABASE<<DATABASE.at(i);
                        } else{
                            if (!QDir(queryi.at(0)).exists()&& !QFile(queryi.at(0)).exists()) {
                                QString oldfilename=queryi.at(0);
#ifdef WINDOWS
                                oldfilename.replace("/","\\");
#endif
                                oldfilename.replace(orginalpath,newpath);
                                qDebug()<<"new "+oldfilename;
                                if (QDir(oldfilename).exists()|| QFile(oldfilename).exists()) {
                                    qDebug()<<"Doesnot exist, so renaming "+queryi.at(0)+" to " +oldfilename;
                                    ui->history->addItem("Corrected "+queryi.at(0)+" to " +oldfilename);
                                    ui->history->scrollToBottom();
                                    ui->tabWidget->setCurrentIndex(0);
                                    HISTORY<<"Corrected "+queryi.at(0)+" to " +oldfilename;
                                    NEWDATABASE<<oldfilename+"#tags-:"+queryi.at(1);
                                } else {
                                    NEWDATABASE<<DATABASE.at(i);
                                }


                            } else {
                                NEWDATABASE<<DATABASE.at(i);
                            }
                        }
                    }

                }
                DATABASE.clear();
                DATABASE<<NEWDATABASE;
                SAVEDATABASE();
                UPDATE_FILELIST();
            }
        }
    }

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
    SAVEHISTORY();
    ui->virtualFS->setEnabled(false);
    ui->ENTER_TAG_FIND->setText("");

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
                bool present=false;
                for(int j=0;j<DATABASE.size();j++)
                {
                    if (DATABASE.at(j)==items.at(i).trimmed())
                        present=true;
                }
                if(!present)
                {
                    DATABASE<<items.at(i).trimmed();
                    QRegExp rxi("#tags-:");
                    QStringList queryi = items.at(i).split(rxi);
                    QRegExp rx("(\\ |\\t)"); //RegEx for ' ' OR '\t'
                    QStringList query = queryi.at(1).split(rx);
                    UPDATETAGDB(query);
                }
            }
        }

    }
    UPDATE_FILELIST();

}

void FileTagger::Check_FILELIST()
{

    ui->FILE_LIST->clear();
    QRegExp rx("#tags-:");int count=0;
    for (int i = 0; i < DATABASE.size(); ++i)
    {
        QStringList queryi = DATABASE.at(i).split(rx);
        ui->FILE_LIST->addItem(queryi.at(0));
        ui->FILE_LIST->addItem("Tags-: "+queryi.at(1)+"\n");


        if(queryi.at(0).size()>4){
            QString subString=queryi.at(0).left(3);
            if (subString=="htt"){
                qDebug()<<"is a http link";

            } else{
                if (QDir(queryi.at(0)).exists()||QFile(queryi.at(0)).exists()) {
                    ui->FILE_LIST->item(ui->FILE_LIST->count()-2)->setForeground(*(new QBrush(Qt::green)));
                    ui->FILE_LIST->item(ui->FILE_LIST->count()-1)->setForeground(*(new QBrush(Qt::green)));
                } else {
                    ui->FILE_LIST->item(ui->FILE_LIST->count()-2)->setForeground(*(new QBrush(Qt::red)));
                    ui->FILE_LIST->item(ui->FILE_LIST->count()-1)->setForeground(*(new QBrush(Qt::red)));
                    count=count+1;
                }
            }
        }

    }
    ui->FILE_LIST->scrollToTop();
    if (count>0) {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Some files are missing (highlighted in red)");
        messageBox.setFixedSize(500,200);
    }
    ui->virtualFS->setEnabled(false);
    ui->ENTER_TAG_FIND->setText("");
}

void FileTagger::UPDATE_FILELIST()
{
    ui->FILE_LIST->clear();
    QRegExp rx("#tags-:");

    for (int i = 0; i < DATABASE.size(); ++i)
    {
        QStringList queryi = DATABASE.at(i).split(rx);
        ui->FILE_LIST->addItem(queryi.at(0));
        ui->FILE_LIST->addItem("Tags-: "+queryi.at(1)+"\n");
        ui->FILE_LIST->item(ui->FILE_LIST->count()-1)->setForeground(*(new QBrush(Qt::darkGreen)));



    }
    ui->FILE_LIST->scrollToTop();
}

void FileTagger::AUTOCOMPLETETAG()
{
    //qDebug()<<ui->PREVIOUS_TAGS->count();
    if(ui->PREVIOUS_TAGS->count()<=0.0)
        return;
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
    stoptagsorting=true;
    ui->ENTER_TAG->setText(finaltag);
    stoptagsorting=false;
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
#ifdef WINDOWS
    if( filename.at(filename.length()-1) == '\\' ) filename.remove( filename.length()-1, 1 );
    filename.replace("/","\\");
#endif
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

void FileTagger::SORTFILELIST()
{

    if(ui->AproximateradioButton->isChecked())
    {
        SORTApproxiFILELIST();
    }else {
        SORTExactFILELIST();
    }


}
void  FileTagger::SORTExactFILELIST()
{   ui->virtualFS->setEnabled(false);
    ui->FILE_LIST->clear();
    QString find= ui->ENTER_TAG_FIND->text();
    PRESENTDATABASE.clear();
    QStringList ABSENTDATABASE;
    QRegExp rxdb("#tags-:");
    QRegExp rx("(\\ |\\t)");

    QStringList tags = find.split(rx);
    for (int i = 0; i < DATABASE.size();i++)
    {

        bool alltagpresent=true;
        QStringList dbtaglist = DATABASE.at(i).split(rxdb);
        QStringList dbtags=dbtaglist.at(1).split(rx);

        for (int j = 0; j < tags.size(); j++)
        {
            bool present=false;
            for (int k = 0; k < dbtags.size(); k++)
            {
                QString dbtag=dbtags.at(k);

                if ((dbtag.toLower()).contains(tags.at(j).toLower().trimmed()))
                {
                    present=true;
                }

            }
            alltagpresent=alltagpresent && present;


        }
        if (alltagpresent)
        {
            PRESENTDATABASE<<DATABASE.at(i);
        } else {
            ABSENTDATABASE<<DATABASE.at(i);
        }

    }



    DATABASE.clear();
    DATABASE<<PRESENTDATABASE;
    DATABASE<<ABSENTDATABASE;
    if( PRESENTDATABASE.size()>0 && find.size()>0)
        ui->virtualFS->setEnabled(true);
    for (int i = 0; i < PRESENTDATABASE.size();i++)
    {
        QStringList dbtaglist = DATABASE.at(i).split(rxdb);
        ui->FILE_LIST->addItem(dbtaglist.at(0));
        ui->FILE_LIST->addItem("Tags-: "+dbtaglist.at(1)+"\n");
        ui->FILE_LIST->item(ui->FILE_LIST->count()-1)->setForeground(*(new QBrush(Qt::darkGreen)));

    }

    ui->FILE_LIST->scrollToTop();

}

void  FileTagger::SORTApproxiFILELIST()
{

    ui->PREVIOUS_TAGS->clear();
    QString find= ui->ENTER_TAG_FIND->text();
    float *score=new float[DATABASE.size()];

    QRegExp rxdb("#tags-:");
    QRegExp rx("(\\ |\\t)");
    QStringList tags = find.split(rx);

    for (int i = 0; i < DATABASE.size();i++)
    {
        QStringList dbtaglist = DATABASE.at(i).split(rxdb);
        QStringList dbtags=dbtaglist.at(1).split(rx);
        score[i]=0.0;
        for (int j = 0; j < tags.size(); j++)
        {
            for (int k = 0; k < dbtags.size(); k++)
            {
                if (tags.at(j).trimmed().size()>0)
                {
                    float scoring=wordmatching(dbtags.at(k),tags.at(j))/tags.at(j).size();
                    ///taking weight  of highly matching tag
                    if (score[i]<scoring)
                        score[i]=scoring;
                }
            }
        }

    }

    if (find.length()>0){

        QString replacest;
        float scoretmp;

        for (int i = 0; i < DATABASE.size()-1; i++)
        {
            for (int j = i+1; j < DATABASE.size(); j++)
            {


                if (score[j] > score[i])
                {

                    replacest=DATABASE.at(j);
                    DATABASE.replace(j,DATABASE.at(i) );
                    DATABASE.replace(i,replacest );
                    scoretmp=score[j];
                    score[j]=score[i];
                    score[i]=scoretmp;
                }

            }
        }

        //update FILE_LIST IN SECOND TAB
        UPDATE_FILELIST();

    }
}
void  FileTagger::UPDATETAGLIST()
{

    if (stoptagsorting)
        return;
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
        float *score=new float[TAGS.size()];
        for (int j = 0; j < TAGS.size(); ++j)
        {
            score[j]=wordmatching(TAGS.at(j),find);
        }
        float scoretmp;
        QString replacest;
        for (int i = 0; i < TAGS.size()-1; ++i)
        {
            for (int j = i+1; j < TAGS.size(); ++j)
            {
                if (score[j] > score[i])
                {
                    replacest=TAGS.at(j);
                    TAGS.replace(j,TAGS.at(i) );
                    TAGS.replace(i,replacest );
                    scoretmp=score[j];
                    score[j]=score[i];
                    score[i]=scoretmp;
                }

            }
        }

        for (int i = 0; i < TAGS.size(); ++i)
        {
            if( score[i]>0)
            {
                QListWidgetItem *item = new QListWidgetItem();
                item->setData(Qt::DisplayRole, TAGS.at(i));
                item->setTextAlignment(Qt::AlignRight);
                ui->PREVIOUS_TAGS->addItem(item);
                ui->PREVIOUS_TAGS->item(ui->PREVIOUS_TAGS->count()-1)->setForeground(*(new QBrush(Qt::darkGreen)));

            }

        }
        ui->PREVIOUS_TAGS->scrollToTop();



    }
}
void  FileTagger::UPDATETAGDB(QStringList tags)
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
    int LISTindex=ui->FILE_LIST->currentRow()/2;

    if( LISTindex>=0 && LISTindex<DATABASE.size())
    {
        QMessageBox::StandardButton reply;
        QStringList queryd = DATABASE.at(LISTindex).split(rx);
        reply = QMessageBox::question(this, "Confirmation", queryd.at(0)+"\n Do you want to Remove Tags?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {


            DATABASE.removeAt(LISTindex);
            ui->history->addItem("REMOVED "+queryd.at(0)+"  -["+queryd.at(1)+"]");
            ui->history->scrollToBottom();
            HISTORY<<"REMOVED "+queryd.at(0)+"  -["+queryd.at(1)+"]";
            ui->tabWidget->setCurrentIndex(0);
            SAVEDATABASE();
            //update FILE_LIST IN SECOND TAB
            UPDATE_FILELIST();
        } else {
            qDebug() << "Yes was *not* clicked";
        }



    }

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
    ui->history->scrollToBottom();
    HISTORY<<prefix+" "+queryd.at(0)+"  -["+queryd.at(1)+"]";
    ui->tabWidget->setCurrentIndex(0);
    //update FILE_LIST IN SECOND TAB
    SAVEDATABASE();
    UPDATE_FILELIST();


}
void FileTagger::SAVE_TAG_ACTION()
{
    OPENDATABASE();
    QString NEW_TAG= ui->ENTER_TAG->text().trimmed();
    QString file=ui->filename->text().trimmed();
    bool ishttp=false;
    if(file.size()<2)
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Give File/Folder name");
        messageBox.setFixedSize(500,200);
        return;
    }
    file.replace("file:///","/");
    if( file.at(file.length()-1) == '/' ) file.remove( file.length()-1, 1 );

#ifdef WINDOWS
    if( file.at(file.length()-1) == '\\' ) file.remove( file.length()-1, 1 );
    file.replace("/","\\");
#endif
    if(file.size()>4){
        QString subString=file.left(3);
        if (subString=="htt")
            ishttp=true;
    }
    if ((!QDir(file).exists()&&!QFile(file).exists())&&!ishttp)
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
        if ( STRING.count(QRegExp("[a-zA-Z0-9]"))==0)
        {
            if (STRING.size()>1){
                QMessageBox messageBox;
                messageBox.critical(0,"Error","Tags should contain atleast one alphabet or number");
                messageBox.setFixedSize(500,200);

                return;
            }
        }
    }

    UPDATETAGDB(query);


    if (QDir(file).exists()||QFile(file).exists()||ishttp) {
        //qDebug()<<file;

        ADDTODATABASE(file+"#tags-:"+NEW_TAG);
    }





}

FileTagger::~FileTagger()
{
    delete ui;
}
