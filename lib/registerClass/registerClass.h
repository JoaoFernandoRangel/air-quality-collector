#include "Arduino.h"
#include "LittleFS.h"




class registerClass{
    private:
    String _rootFolder;


    public:
    registerClass();
    registerClass(String r);
    bool registerInit();
    String getRootFolder();
    void setRootFolder(String s);
    void saveNewFile(String fileName, String content);
    void deleteFile(String fileName);
    String readFile(String fileName);
    String readFile(String fileName, String &fileContent);
    void listFiles();
};



