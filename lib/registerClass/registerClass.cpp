#include "registerClass.h"

registerClass::registerClass() {}
registerClass::registerClass(String r) {
    this->setRootFolder(r);
}

String registerClass::getRootFolder() {
    return _rootFolder;
}

void registerClass::setRootFolder(String s) {
    _rootFolder = s;
    if (!LittleFS.exists(_rootFolder)) {
        LittleFS.mkdir(_rootFolder);
    }
}

bool registerClass::registerInit() {
    if (!LittleFS.begin()) {
        Serial.println("Failed to mount LittleFS");
        return false;
    }
    //   Serial.println("LittleFS mounted successfully");
    return true;
}

void registerClass::saveNewFile(String fileName, String content) {
    String fullPath = _rootFolder + "/" + fileName + ".txt";
    File file = LittleFS.open(fullPath.c_str(), "w");
    if (!file) {
        Serial.println("Failed to open file for writing: " + fullPath);
        return;
    }
    file.print(content);
    file.close();
}

void registerClass::deleteFile(String fileName) {
    String fullPath = _rootFolder + "/" + fileName + ".txt";
    if (LittleFS.exists(fullPath)) {
        LittleFS.remove(fullPath);
    } else {
        Serial.println("File does not exist: " + fullPath);
    }
}

String registerClass::readFile(String fileName) {
    File f = LittleFS.open((_rootFolder + "/" + fileName + ".txt").c_str(), "r");
    if (!f) {
        Serial.println("Failed to open file for reading: " + fileName);
        return "";
    }
    String content = f.readString();
    f.close();
    return content;
}

String registerClass::readFile(String fileName, String &fileContent) {
    File f = LittleFS.open((_rootFolder + "/" + fileName + ".txt").c_str(), "r");
    if (!f) {
        Serial.println("Failed to open file for reading: " + fileName);
        return "";
    }
    String content = f.readString();
    f.close();
    fileContent = content; // Store the content in the provided reference
    return content;
}

void registerClass::listFiles() {
    File dir = LittleFS.open(_rootFolder.c_str(), "r");
    if (!dir || !dir.isDirectory()) {
        Serial.println("❌ Falha ao abrir diretório ou não é um diretório.");
        return;
    }

    Serial.println("==========================");
    Serial.println("📂 Arquivos em /registros:");
    
    File file = dir.openNextFile();
    while (file) {
      if (!file.isDirectory()) {
        Serial.print(file.name());
        Serial.print(" (");
        Serial.print(file.size());
        Serial.println(" bytes)");
      }
      file = dir.openNextFile();
    }
    Serial.println("==========================");
}
