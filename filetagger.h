/*
    filetagger.h is part of FILETAGGER.

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

#ifndef FILETAGGER_H
#define FILETAGGER_H
/*Choose operating system
    1. In windows file separator have to be changed from '/' to '\'
    2. In Linux shared memory is used to avoid reloading of database file.
*/
//#define WINDOWS
#define LINUX

#include <QUrl>
#include <QDebug>
#include <QMainWindow>
#include <QFileSystemWatcher>
#include <QFileSystemModel>
#include <QCompleter>
#include <QListWidgetItem>
#include <QInputDialog>
#include <QTimer>

#ifdef LINUX
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define SHMSZ 90000000
#define SHMKEY 1367
#endif

QT_BEGIN_NAMESPACE
class QMimeData;

QT_END_NAMESPACE

#ifdef LINUX
typedef struct
{

    char filetaggerdb[10000000];
    int filetaggerdb_size;
    char history[10000];
    int history_size;

}DATA ;
#endif
namespace Ui {
class FileTagger;
}

class FileTagger : public QMainWindow
{
    Q_OBJECT

public:
    explicit FileTagger(QWidget *parent = 0);
    ~FileTagger();

    void SAVEDATABASE();
    void OPENDATABASE();
    void SAVEHISTORY();
    void OPENHISTORY();
    void SORTExactFILELIST();
    void SORTApproxiFILELIST();
    void UPDATE_FILELIST();
    void UPDATETAGLIST();
    void SORTFILELIST();
#ifdef LINUX
    void WRITETOSHARED_DB();
    bool READFROMSHARED_DB();
    void WRITETOSHARED_history();
    bool READFROMSHARED_history();
#endif

private slots:
    void dropEvent(QDropEvent *ev) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *ev) Q_DECL_OVERRIDE;
    void SAVE_TAG_ACTION();
    void ADDTODATABASE(QString data );
    void UPDATETAGDB(QStringList tags);
    void UPDATETAG();
    void AUTOCOMPLETETAG();
    void REMOVEFROMDATABASE();
    void OPEN_FILE();
    void Check_FILELIST();
    void exitapp();
    void checkupdates();
    void enablevirtualfilesysbut();
    void disablevirtualfilesysbut();
    void createvirtualfilesys();
    void anytagexactsearch();
    void anytagreleaseexactsearch();
    void Entertagfind_counterupdate();
    void Entertag_counterupdate();
    void timeexceeded();

private:
    Ui::FileTagger *ui;
    QFileSystemWatcher *watcher;
    QCompleter *completer;
    bool stoptagsorting;
    QString dbdir;
    bool activityover;
    bool anytag;
    int Entertag_counter;
    int Entertagfind_counter;
    QStringList DATABASE;
    QStringList PRESENTDATABASE;
    QStringList HISTORY;
    QStringList TAGS;

};


#endif // FILETAGGER_H
