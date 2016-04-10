About
------------------------------------

  1. This is a simple gui utility written in qt to tag file/folders without affecting files.
  
  2. Approximate word matching algorithm is used for searching tags.

  3. Application will also help in keeping consistency in tag names(Reusing the same terms).

  4. Configuration file is kept in <home directory>/.Filetagger/Filetagger_db as a simple text file. You can find replace path if you have moved or renamed any folder.

  5. File or Folder can be opened by clicking on the link listed in "Browse Tags" tab

  6. Screenshot of the application is also uploaded.(TAB1.png, TAB2.png)

  

Installation
------------------------------------

  1. Compile the project in qtcreator and copy FILETAGGER to /usr/bin. Executable file for 64 bit machine is uploaded(Compile on ubuntu14.04LTS). 
  
  2. On KDE Desktop file_tag action can be added on file browsers. Do as follows 
  


Run following as root 
````
    cp file_tagger.desktop /usr/share/kde4/services/ServiceMenus/file_tagger.desktop
    kbuildsycoca4

```
  Close all instance of Dolphin browser and start again. Tag file option will appear in the action list.