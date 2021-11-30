#pragma once
#define HAVE_STRUCT_TIMESPEC // [Add] 
#include<string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdlib.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <pthread.h>

#include "ClientBuffer.h"

struct TransfarDoc {
    //std::vector<sockaddr_in*>* pClientBuffer;
    ClientBuffer* pClientBuffer;
    SOCKET* pListenSocket;
    int    DataBufferSize;
    int    CameraPort;
    double CameraWidth;
    double CameraHight;
    double CameraFPS;
};

/**
 * @ImageStream
 *
 * @brief:Image Captur And Send to Client.
 *
 */
class ImageStream
{
	public:
	ImageStream(){};
	~ImageStream(){};

    static void* VideoStream(void* arg);
};

