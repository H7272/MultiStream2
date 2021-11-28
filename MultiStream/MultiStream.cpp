// TestSimpleThread.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。

#define HAVE_STRUCT_TIMESPEC // [Add] 

#include<string>
#include<fstream>
#include<iostream>

#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdlib.h>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <pthread.h>
#include <time.h>

#pragma comment(lib, "Ws2_32.lib")
using namespace cv;

// SyncSettings.
#define DEFAULT_BUFLEN 32678
#define DEFAULT_PORT 50000
#define ADDMISSIONCODE "333acv2dfert"
#define STARTCODE "START"
#define ENDCODE "QUITE"

// ImageSettings.
#define CAMERAPORT  0
#define CAMERAWIDTH 320
#define CAMERAHIGHT 240
#define CAMERAFPS   25

// AliveCheck[sec]
#define CHECKSTEP 10

static pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;

struct SocketInfo {
    std::vector<sockaddr_in*>* pClientBuffer;
    SOCKET* pListenSocket;
    int    DataBufferSize;
    int    CameraPort;
    double CameraWidth;
    double CameraHight;
    double CameraFPS;
};

/**
 * @VideoStream .
 * @brief Image Captur And Send to Client.
 */
void* VideoStream(void* arg)
{
    // Initialize Winsock
    WSADATA wsaData;
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        return 0;
    }
    // SetData.
    struct SocketInfo* TransfarDoc = (struct SocketInfo*)arg;
    std::vector<sockaddr_in*>* pClientBuffer = TransfarDoc->pClientBuffer;
    SOCKET* pListenSocket    = TransfarDoc->pListenSocket;
    const int DataBufferSize = TransfarDoc->DataBufferSize;
    const int CameraPort     = TransfarDoc->CameraPort;
    const double CameraWidth = TransfarDoc->CameraHight;
    const double CameraHight = TransfarDoc->CameraWidth;
    const double CameraFPS   = TransfarDoc->CameraFPS;
    
    while (1) {

        // If Client 0 Then Wait Continue.
        if (0 == pClientBuffer->size()) {
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
        param[1] = 85;
        std::vector<unsigned char> ibuff;
        // Create ImageData.
        char* sendbuf = NULL;
        sendbuf = new char[DataBufferSize]();

        // StratStreaming.
        while (0 != pClientBuffer->size()) {
            memset(sendbuf, 0, DataBufferSize);
            // Captre Image.
            captre >> frame;
            imencode(".jpg", frame, ibuff, param);
            // Image to SendBuffer.
            int pos = 0;
            for (unsigned char i : ibuff) { sendbuf[pos++] = i; }
            // Send ImageData to All Client.
            pthread_mutex_lock(&count_mutex);
            std::vector<sockaddr_in*>::iterator itrClient = pClientBuffer->begin();
            while (itrClient != pClientBuffer->end()) {
                sendto(*pListenSocket, sendbuf, DataBufferSize, 0, (SOCKADDR*)*itrClient, sizeof(**itrClient));
                Sleep(1);
                ++itrClient;
            }
            pthread_mutex_unlock(&count_mutex);
        }

        // Disconnect Process.
        printf("StreamTH:CloseCaptre \n");
        ibuff.shrink_to_fit();
        captre.release();
        for (sockaddr_in* pClient : *pClientBuffer) {
            delete pClient;
        }
        printf("StreamTH:Wait Next Connect...%d :\n", (int)pClientBuffer->size());
    }
    return 0;
}

/**
 * @SarchDisconnectClient .
 * @brief Image Captur And Send to Client.
 */
bool SarchDisconnectClient(sockaddr_in* pClientSocket, std::vector<sockaddr_in*>* pClientSocketBuffer) {

    char ErrorClientIP[INET6_ADDRSTRLEN] = {};
    inet_ntop(pClientSocket->sin_family, &(pClientSocket->sin_addr), ErrorClientIP, sizeof(ErrorClientIP));
    printf(" Sarch DisconnectClient In Buffer %s\n", ErrorClientIP);

    std::vector<sockaddr_in*>::iterator itrClient = pClientSocketBuffer->begin();
    while (itrClient != pClientSocketBuffer->end()) {
        char BufferIP[INET6_ADDRSTRLEN] = {};
        sockaddr_in* pInBufferClientSocket = *itrClient;
        inet_ntop(pInBufferClientSocket->sin_family, &(pInBufferClientSocket->sin_addr), BufferIP, sizeof(BufferIP));

        if (strcmp(ErrorClientIP, BufferIP) == 0) {
            itrClient = pClientSocketBuffer->erase(itrClient);
            printf(" Erase Client From Buffer %s\n", ErrorClientIP);
            printf(" Client Num %d :\n", (int)pClientSocketBuffer->size());
            delete pInBufferClientSocket;
            break;
        }
        ++itrClient;
    }
    return true;
}

/**
 * @SarchDisconnectClient .
 * @brief Image Captur And Send to Client.
 */
bool SarchConnectedClient(sockaddr_in* pClientSocket, std::vector<sockaddr_in*>* pClientSocketBuffer) {

    char ErrorClientIP[INET6_ADDRSTRLEN] = {};
    inet_ntop(pClientSocket->sin_family, &(pClientSocket->sin_addr), ErrorClientIP, sizeof(ErrorClientIP));

    std::vector<sockaddr_in*>::iterator itrClient = pClientSocketBuffer->begin();
    while (itrClient != pClientSocketBuffer->end()) {
        char BufferIP[INET6_ADDRSTRLEN] = {};
        sockaddr_in* pInBufferClientSocket = *itrClient;
        inet_ntop(pInBufferClientSocket->sin_family, &(pInBufferClientSocket->sin_addr), BufferIP, sizeof(BufferIP));

        if (strcmp(ErrorClientIP, BufferIP) == 0) {
            return true;
        }
        ++itrClient;
    }
    return false;
}

/**
 * @SarchDisconnectClient .
 * @brief Image Captur And Send to Client.
 */
void DeleteAllClient(std::vector<sockaddr_in*>* pClientSocketBuffer) {
    std::vector<sockaddr_in*>::iterator itrClient = pClientSocketBuffer->begin();
    while (itrClient != pClientSocketBuffer->end()) {
        sockaddr_in* pInBufferClientSocket = *itrClient;
        itrClient = pClientSocketBuffer->erase(itrClient);
        delete pInBufferClientSocket;
    }
    return;
}

/**
 * @GetLineToValueString
 *
 * @brief Choose ValueString From ReadLineString.
 *
 */
bool GetLineToValueString(std::string LineString, std::string &ValueString){
    // 文字列が0でないならIntに変換.
    if (0 == LineString.size()) {
        return false;
    }
    // "=" の位置を取得.
    const size_t startpos = LineString.find("=");
    if (std::string::npos == startpos) {
        return false;
    }
    // ";" の位置を取得.
    const size_t endpos = LineString.find(";");
    if (std::string::npos == endpos) {
        return false;
    }
    if (startpos > endpos) {
        return false;
    }
    // "="から";"の文字列を抽出.
    std::string strNumber = LineString.substr(startpos + (size_t)1, endpos - (startpos + (size_t)1));
    // 文字列が0でないならIntに変換.
   if (0 == strNumber.size()) {
        return false;
   }
   ValueString = strNumber;
   return true;
}

/**
 * @MultiCameraStream(Main)
 *
 * @brief WaitClient.
 *
 */
int main(int argc, char* argv[])
{
    int    TimeOut        = CHECKSTEP;
    int    DataBufferSize = DEFAULT_BUFLEN;
    int    CameraPort     = CAMERAPORT; 
    double CameraWidth    = CAMERAWIDTH;
    double CameraHight    = CAMERAHIGHT;
    double CameraFPS      = CAMERAFPS;
    // Read Ini File.
    std::ifstream file("Setting.ini");
    const int buf_size = 60;
    char readdata[buf_size];
    // ファイルがあれば1行ずつ読み込む.
    if (!file.fail()) {  
        std::string ValueString;
        while (file.getline(readdata, buf_size)) {    
            ValueString = "";
            std::string str1 = readdata;
            if ( str1.empty()){
                continue;
            }
            else if (0 == str1.find("*")){
                continue;
            }
            else if (std::string::npos != str1.find("TimeOut")){
                if(false == GetLineToValueString(str1, ValueString )){
                    continue;
                }
                TimeOut = std::stoi(ValueString);
                std::cout <<"Set Ini Value TimeOut:" << TimeOut << std::endl;
            }
            else if (std::string::npos != str1.find("DataBufferSize")) {
                if (false == GetLineToValueString(str1, ValueString)) {
                    continue;
                }
                DataBufferSize = std::stoi(ValueString);
                std::cout << "Set Ini Value DataBufferSize:" << DataBufferSize << std::endl;
            }
            else if (std::string::npos != str1.find("CameraPort")) {
                if (false == GetLineToValueString(str1, ValueString)) {
                    continue;
                }
                CameraPort = std::stoi(ValueString);
                std::cout << "Set Ini Value CameraPort:" << CameraPort << std::endl;
            }
            else if (std::string::npos != str1.find("CameraWidth")) {
                if (false == GetLineToValueString(str1, ValueString)) {
                    continue;
                }
                CameraWidth = std::stod(ValueString);
                std::cout << "Set Ini Value CameraWidth:" << CameraWidth << std::endl;
            }
            else if (std::string::npos != str1.find("CameraHight")) {
                if (false == GetLineToValueString(str1, ValueString)) {
                    continue;
                }
                CameraHight = std::stod(ValueString);
                std::cout << "Set Ini Value CameraHight:" << CameraHight << std::endl;
            }
            else if (std::string::npos != str1.find("CameraFPS")) {
                if (false == GetLineToValueString(str1, ValueString)) {
                    continue;
                }
                CameraFPS = std::stod(ValueString);
                std::cout << "Set Ini Value CameraFPS:" << CameraFPS << std::endl;
            }
            else {
                continue;
            }
            continue;
        }
    }

    // Winsock Setting. 
    WSADATA wsaData;
    SOCKET ListenSocket = INVALID_SOCKET;
    struct sockaddr_in ListenAddr;

    const unsigned short Port = DEFAULT_PORT;

    // Initialize Winsock
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        return 1;
    }

    ListenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (ListenSocket == INVALID_SOCKET) {
        printf("Socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    ListenAddr.sin_family = AF_INET;
    ListenAddr.sin_port = htons(Port);
    ListenAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (SOCKET_ERROR == bind(ListenSocket, (SOCKADDR*)&ListenAddr, sizeof(ListenAddr))) {
        printf("Bind failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    u_long val = 1;
    ioctlsocket(ListenSocket, FIONBIO, &val);

    struct SocketInfo TransfarDoc;
    std::vector<sockaddr_in*> pClientSocketBuffer;
    pthread_t handle;
    TransfarDoc.pClientBuffer = &pClientSocketBuffer;
    TransfarDoc.pListenSocket = &ListenSocket;
    TransfarDoc.CameraPort =  CameraPort;
    TransfarDoc.CameraWidth = CameraWidth;
    TransfarDoc.CameraHight = CameraHight;
    TransfarDoc.CameraFPS   = CameraFPS;
    TransfarDoc.DataBufferSize = DataBufferSize;

    pthread_create(&handle, NULL, VideoStream, &TransfarDoc);

    char RecvMessage[1024] = {};
    char Sendemssage[32] = {};
    int  iResult = 0;

    time_t LastAckTime = 0;

    // カメラデータ.
    std::snprintf(Sendemssage, sizeof(Sendemssage), "%d,%d,%d,", CAMERAWIDTH, CAMERAHIGHT, CAMERAFPS);
    printf("Main;Waiting ... Stop Ctr + Brk :\n");
    while (true) {
        // クライアントの受け入れ、削除処理.
        char ClientIP[INET6_ADDRSTRLEN] = {};
        struct sockaddr_in* pClientSocket = NULL;
        pClientSocket = new struct sockaddr_in();
        int  ClientSocketSize = sizeof(*pClientSocket);

        memset(RecvMessage, 0, sizeof(RecvMessage));
        int ErroCode = 0;
        // Wait Client.
        while (true) {
            recvfrom(ListenSocket, RecvMessage, sizeof(RecvMessage), 0, (SOCKADDR*)pClientSocket, &ClientSocketSize);
            ErroCode = WSAGetLastError();
            if (WSAEWOULDBLOCK == ErroCode) {
                // 再待機.
                Sleep(10);
                // クライアントがいるはずなのにLastAckTimeを確認最終結果から1sec経過していたら全クライアントを削除.
                if (pClientSocketBuffer.size() > 0) {
                    time_t NowAckTime = time(NULL);
                    __int64 TimeDiff = NowAckTime - LastAckTime;
                    if (TimeDiff > TimeOut) {
                        printf("Main;All Client Bye...: %lld , %lld \n", (__int64)NowAckTime, (__int64)LastAckTime);
                        pthread_mutex_lock(&count_mutex);
                        DeleteAllClient(&pClientSocketBuffer);
                        pthread_mutex_unlock(&count_mutex);
                    }
                }
                continue;
            }
            else {
                break;
            }
        }

        // エラー処理.
        if (0 != ErroCode) {
            wprintf(L"Main;Recvfrom failed with error %d\n", ErroCode);
            // 切断処理.
            if (WSAECONNRESET == ErroCode) {
                pthread_mutex_lock(&count_mutex);
                if (AF_INET == pClientSocket->sin_family) {
                    SarchDisconnectClient(pClientSocket, &pClientSocketBuffer);
                }
                else {
                    printf("Main;Connect Unknown Client Request\n");
                }
                pthread_mutex_unlock(&count_mutex);
            }
            delete pClientSocket;
            continue;
        }

        // 正常受信.
        // IPV_4のみ対応.
        if (AF_INET == pClientSocket->sin_family) {
            inet_ntop(pClientSocket->sin_family, &(pClientSocket->sin_addr), ClientIP, sizeof(ClientIP));
        }
        else {
            printf("Main;Sorry IPV4 Only \n");
            delete pClientSocket;
            continue;
        }

        // クライアント終了通知.
        if (0 == strcmp(RecvMessage, ENDCODE)) {
            printf("Main;Quit Client %s\n", ClientIP);
            // 終了通知受信元を削除.
            pthread_mutex_lock(&count_mutex);
            if (0 != pClientSocketBuffer.size()) {
                // ベクトル内の同じクライアントを検索して削除する.
                SarchDisconnectClient(pClientSocket, &pClientSocketBuffer);
            }
            pthread_mutex_unlock(&count_mutex);
            delete pClientSocket;
            continue;
        }

        // 承認確認.
        if (0 == strcmp(RecvMessage, ADDMISSIONCODE)) {
            printf("Main;Permission Connection from %s \n", ClientIP);
            // カメラのデータをクライアントに渡す.
            sendto(ListenSocket, Sendemssage, sizeof(Sendemssage), 0, (struct sockaddr*)pClientSocket, ClientSocketSize);
            // 接続先の sockaddr_in オブジェクトを登録する.
            pthread_mutex_lock(&count_mutex);
            pClientSocketBuffer.push_back((pClientSocket));
            pthread_mutex_unlock(&count_mutex);
            LastAckTime = time(NULL);
            continue;
        }

        // 通信あり.
        if (true == SarchConnectedClient(pClientSocket, &pClientSocketBuffer)) {
            LastAckTime = time(NULL);
            continue;
        }

        delete pClientSocket;
        continue;
    }

    pthread_join(handle, NULL);
    closesocket(ListenSocket);
    WSACleanup();
    pthread_cancel(handle);

    return 0;
}
