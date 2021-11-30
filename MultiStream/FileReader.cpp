#include "FileReader.h"
#include<fstream>
#include<iostream>

FileReader::FileReader(std::string filename)
{
    MyFileName = filename;
}

/**
 * @GetLineToValueString
 *
 * @brief Get Value  from line.
 *
 */
bool  FileReader::GetLineToValueString(std::string LineString, std::string& ValueString)
{
    if (0 == LineString.size()) {
        return false;
    }
    // Get"="Pos.
    const size_t startpos = LineString.find("=");
    if (std::string::npos == startpos) {
        return false;
    }
    // Get";"Pos.
    const size_t endpos = LineString.find(";");
    if (std::string::npos == endpos) {
        return false;
    }
    if (startpos > endpos) {
        return false;
    }
    // Get String from "=" to ";"
    std::string strNumber = LineString.substr(startpos + (size_t)1, endpos - (startpos + (size_t)1));
    if (0 == strNumber.size()) {
        return false;
    }

    ValueString = strNumber;
    
    return true;
}

/**
 * @GetFileElement
 *
 * @brief Read the file text and get the value corresponding to each keyword. 
 *
 */
 void FileReader::GetFileElement(ReadWord& ReadInfo)
 {
     std::ifstream file(MyFileName);
     const int buf_size = 255;
     char readdata[buf_size];
      
     if (!file.fail()) {
         std::string ValueString;
         while (file.getline(readdata, buf_size)) {
             ValueString = "";
             std::string str1 = readdata;
             if (str1.empty()) {
                 continue;
             }
             
             if (0 == str1.find("*")) {
                 continue;
             }
             
             if (std::string::npos != str1.find("TimeOut")) {
                 if (false == GetLineToValueString(str1, ValueString)) {
                     continue;
                 }
                 ReadInfo.TimeOut = std::stoi(ValueString);
                 std::cout << "Set Ini Value TimeOut:" << ReadInfo.TimeOut << std::endl;
                 continue;
             }

             if (std::string::npos != str1.find("DataBufferSize")) {
                 if (false == GetLineToValueString(str1, ValueString)) {
                     continue;
                 }
                 ReadInfo.DataBufferSize = std::stoi(ValueString);
                 std::cout << "Set Ini Value DataBufferSize:" << ReadInfo.DataBufferSize << std::endl;
                 continue;
             }
             
             if (std::string::npos != str1.find("CameraPort")) {
                 if (false == GetLineToValueString(str1, ValueString)) {
                     continue;
                 }
                 ReadInfo.CameraPort = std::stoi(ValueString);
                 std::cout << "Set Ini Value CameraPort:" << ReadInfo.CameraPort << std::endl;
                 continue;
             }

             if (std::string::npos != str1.find("CameraWidth")) {
                 if (false == GetLineToValueString(str1, ValueString)) {
                     continue;
                 }
                 ReadInfo.CameraWidth = std::stod(ValueString);
                 std::cout << "Set Ini Value CameraWidth:" << ReadInfo.CameraWidth << std::endl;
                 continue;
             }

             if (std::string::npos != str1.find("CameraHight")) {
                 if (false == GetLineToValueString(str1, ValueString)) {
                     continue;
                 }
                 ReadInfo.CameraHight = std::stod(ValueString);
                 std::cout << "Set Ini Value CameraHight:" << ReadInfo.CameraHight << std::endl;
                 continue;
             }

             if (std::string::npos != str1.find("CameraFPS")) {
                 if (false == GetLineToValueString(str1, ValueString)) {
                     continue;
                 }
                 ReadInfo.CameraFPS = std::stod(ValueString);
                 std::cout << "Set Ini Value CameraFPS:" << ReadInfo.CameraFPS << std::endl;
                 continue;
             }

             if (std::string::npos != str1.find("UsePort")) {
                 if (false == GetLineToValueString(str1, ValueString)) {
                     continue;
                 }
                 ReadInfo.UsePort = (unsigned short)std::stoi(ValueString);
                 std::cout << "Set Ini Value UsePort:" << ReadInfo.UsePort << std::endl;
                 continue;
             }

             continue;
         }
     }
  
  return;

 }