#ifndef FILETAGGER_H
#define FILETAGGER_H

#include <QMainWindow>

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
   QStringList TAGS;
    void SAVEDATABASE();
    void OPENDATABASE();
    void UPDATE_FILELIST();
    QString dbdir;

private slots:

    void ADD_TAG_ACTION();
    void ADDTODATABASE(QString data );
    void ADDTAGS(QStringList tags);
    void UPDATETAGLIST();
    void UPDATETAG();
    void SORTFILELIST();
    void AUTOCOMPLETETAG();
     void REMOVEFROMDATABASE();
     void OPEN_FILE();
private:
    Ui::FileTagger *ui;

};

#endif // FILETAGGER_H
