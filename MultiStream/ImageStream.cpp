#include "ImageStream.h"
#pragma comment(lib, "Ws2_32.lib")
using namespace cv;
/**
 * @VideoStream .
 * @brief:Image Captur And Send to Client.
 */
void* ImageStream::VideoStream(void* arg)
{
    // Initialize Winsock
    WSADATA wsaData;
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        return 0;
    }
    // SetData.
    struct TransfarDoc* TransfarDoc = (struct TransfarDoc*)arg;
    ClientBuffer* pClientBuffer = TransfarDoc->pClientBuffer;
    SOCKET* pListenSocket       = TransfarDoc->pListenSocket;
          int    JPGQuality     = TransfarDoc->JPGQuality;
    const int    DataBufferSize = TransfarDoc->DataBufferSize;
    const int    CameraPort     = TransfarDoc->CameraPort;
    const double CameraWidth    = TransfarDoc->CameraWidth;
    const double CameraHight    = TransfarDoc->CameraHight;
    const double CameraFPS      = TransfarDoc->CameraFPS;
  
    while (1) {

        // If Client 0 Then Wait Continue.
        if (0 == pClientBuffer->IsSize()) {
            Sleep(1);
            continue;
        }

        //  Connect Camera.
        cv::VideoCapture captre(CameraPort, CAP_DSHOW);
        if (!captre.isOpened()) {
            printf("StreamTH:open camera  failed \n");
            continue;
        }

        // Setting GetImage Info.
        cv::Mat frame;
        captre.set(CAP_PROP_FRAME_WIDTH, CameraWidth);
        captre.set(CAP_PROP_FRAME_HEIGHT, CameraHight);
        captre.set(CAP_PROP_FPS, CameraFPS);
        // Setting Encode Data Info.
        std::vector<int> param = std::vector<int>(2);
        param[0] = cv::IMWRITE_JPEG_QUALITY;
        param[1] = JPGQuality;
        std::vector<unsigned char> ibuff;
        // Create ImageData.
        char* sendbuf = NULL;
        sendbuf = new char[DataBufferSize]();

        // StratStreaming.
        while (0 != pClientBuffer->IsSize()) {
            memset(sendbuf, 0, DataBufferSize);
            ibuff.clear();
            ibuff.resize(DataBufferSize);
            // Captre Image.
            captre >> frame;
            imencode(".jpg", frame, ibuff, param);
            // Image to SendBuffer.
            int pos = 0;
            for (unsigned char i : ibuff) { sendbuf[pos++] = i; }
            //Send Imag Buffer.
            pClientBuffer->SendImage(pListenSocket, sendbuf, DataBufferSize);
        }

        // Disconnect Process.
        printf("StreamTH:CloseCaptre \n");
        ibuff.shrink_to_fit();
        captre.release();
        pClientBuffer->DeleteAllClient();
        printf("StreamTH:Wait Next Connect...%d :\n", (int)pClientBuffer->IsSize());
    }
    return 0;
}