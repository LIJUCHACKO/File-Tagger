About
------------------------------------

  1. This is a simple gui utility to tag/bookmark file/folders/websites without affecting filesystem.
  
  2. Approximate word matching algorithm is used for searching tags. Files with approximate matching tags will come on the top.

  3. Application will also help in keeping consistency in tag names(Reusing the same terms).

  4. It will auto correct folder path if multiple files belong to same folder and that folder got renamed or relocated. 

  5. Database is stored in <home directory>/.Filetagger/Filetagger_db as a simple text file.

  5. File/Folder/website can be opened by clicking on the link listed in "Browse Tags" tab

  6. Screenshot of the application is also uploaded.(TAB1.png, TAB2.png)

  

Installation
------------------------------------

  1. Compile the project in qtcreator and copy FILETAGGER to /usr/bin. Executable file for 64 bit linux machine is uploaded(Compile on ubuntu14.04LTS). 
  
  2. On KDE Desktop file_tag action can be added on file browsers. Do as follows 
  


Run following as root 
````
    cp file_tagger.desktop /usr/share/kde4/services/ServiceMenus/file_tagger.desktop
    kbuildsycoca4

```
  Close all instance of Dolphin browser and start again. Tag file option will appear in the action list.

Usage (for linux user)
------------------------------------
  1. In Dolphin file browser, right click on the folder/file and select "ADD TAG" in actions menu. Enter the tags to bookmark the file and press "ADD TAG" button.Similar tags used before will be listed below. By clicking on them or by pressing enter, last tag can be replaced.
  
  2. In the "Browse Tag" tab, enter tags you want to search and files with approximate matching tags will come on the top.By clicking on file path/websites they will open in appropriate program.

  3. It will auto correct folder path if multiple files belong to same folder and the folder got renamed or relocated.