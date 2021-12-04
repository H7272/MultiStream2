#pragma once
#include<string>
typedef struct ReadWord {
    unsigned short  UsePort;
    int    TimeOut;
    int    JPGQuality;
    int    DataBufferSize;
    int    CameraPort;
    double CameraWidth;
    double CameraHight;
    double CameraFPS;
} ReadWord;

/**
 * @FileReader
 *
 * @brief:Read the file text and get the value corresponding to each keyword. 
 *
 */
class FileReader
{
    public:
    FileReader(std::string fiename);

   ~ FileReader(){};

	private:
	std::string  MyFileName;
	bool  GetLineToValueString(std::string LineString, std::string& ValueString);

	public:
    void GetFileElement(ReadWord &ReadInfo);
};

