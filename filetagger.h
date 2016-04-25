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
//#define WINDOWS

#include <QUrl>
#include <QDebug>
#include <QMainWindow>
#include <QFileSystemWatcher>
#include <QFileSystemModel>
#include <QCompleter>

QT_BEGIN_NAMESPACE
class QMimeData;

QT_END_NAMESPACE

namespace Ui {
class FileTagger;
}

class FileTagger : public QMainWindow
{
    Q_OBJECT

public:
    explicit FileTagger(QWidget *parent = 0);
    ~FileTagger();
    QStringList DATABASE;
    QStringList HISTORY;
    QStringList TAGS;
    void SAVEDATABASE();
    void OPENDATABASE();
    void SAVEHISTORY();
    void OPENHISTORY();

    void UPDATE_FILELIST();
    QString dbdir;
    bool activityover;

private slots:
    void dropEvent(QDropEvent *ev) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *ev) Q_DECL_OVERRIDE;
    void ADD_TAG_ACTION();
    void ADDTODATABASE(QString data );
    void ADDTAGS(QStringList tags);
    void UPDATETAGLIST();
    void UPDATETAG();
    void SORTFILELIST();
    void AUTOCOMPLETETAG();
    void REMOVEFROMDATABASE();
    void OPEN_FILE();
    void Check_FILELIST();
    void exitapp();
    void checkupdates();
private:
    Ui::FileTagger *ui;
    QFileSystemWatcher *watcher;
     QCompleter *completer;

};

#endif // FILETAGGER_H
