/*
    main.cpp is part of FILETAGGER.

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
#include "filetagger.h"
#include <QApplication>
#include <QDebug>
QString FILE_ARG;
int main(int argc, char *argv[])
{
    FILE_ARG=argv[1];
    qDebug()<<FILE_ARG;
    QApplication a(argc, argv);
    FileTagger w;
    w.show();

    return a.exec();
}
