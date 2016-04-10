About
------------------------------------

  1. It is any easy way to tag file/folders without affecting file system
  
  2. Approximate word matching algorithm is used so that same tag name can be used for multiple files

  3. While searching for tags you need not type exact spelling of tag.

  4. Configuration file is kept in <home directory>/.Filetagger/Filetagger_db as a simple text file. You can file replace path if you have moved or renamed any folder
  

Installation
------------------------------------

  1. Compile the project in qtcreator and copy to /usr/bin
  
  2. On KDE Desktop in order to provide right click action 
  


Run following as root 
````
    cp file_tagger.desktop /usr/share/kde4/services/ServiceMenus/file_tagger.desktop
    kbuildsycoca4

```
