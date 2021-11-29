// TestSimpleThread.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。

#define HAVE_STRUCT_TIMESPEC // [Add] 

#include <string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#include "FileReader.h"
#include "ImageStream.h"

#pragma comment(lib, "Ws2_32.lib")

// SyncSettings.
#define DEFAULT_BUFLEN 32678
#define DEFAULT_PORT 50000
#define STARTCODE "START"
#define ENDCODE "QUITE"

// ImageSettings.
#define CAMERAPORT  0
#define CAMERAWIDTH 320
#define CAMERAHIGHT 240
#define CAMERAFPS   25

// AliveCheck[sec]
#define CHECKSTEP 10

/**
 * @SarchDisconnectClient .
 * @brief: Erase Specific Client from Buffer.
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
 * @SarchConnectedClient .
 * @brief:Return true If Specific Client in Buffer.
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
 * @DeleteAllClient.
 * @brief: All Client Erase.
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
 * @MultiCameraStream(Main)
 *
 * @brief:RealTimeImageStreaming to multiple clients. 
 *
 */
int main(int argc, char* argv[])
{
    // Read Ini File.
    ReadWord readword;
    readword.TimeOut = CHECKSTEP;
    readword.DataBufferSize = DEFAULT_BUFLEN;
    readword.CameraPort  = CAMERAPORT;
    readword.CameraWidth = CAMERAWIDTH;
    readword.CameraHight = CAMERAHIGHT;
    readword.CameraFPS   = CAMERAFPS;
    readword.UsePort     = DEFAULT_PORT;

    FileReader fileread("Setting.ini");
    fileread.GetFileElement(readword);
    int    TimeOut = readword.TimeOut;

    // Winsock Setting. 
    WSADATA wsaData;
    SOCKET ListenSocket = INVALID_SOCKET;
    struct sockaddr_in ListenAddr;

    const unsigned short Port = readword.UsePort;

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

    struct TransfarDoc TransfarDoc;
    std::vector<sockaddr_in*> pClientSocketBuffer;
    pthread_t handle;
    TransfarDoc.pClientBuffer = &pClientSocketBuffer;
    TransfarDoc.pListenSocket = &ListenSocket;
    TransfarDoc.CameraPort  = readword.CameraPort;
    TransfarDoc.CameraWidth = readword.CameraWidth;
    TransfarDoc.CameraHight = readword.CameraHight;
    TransfarDoc.CameraFPS   = readword.CameraFPS;
    TransfarDoc.DataBufferSize = readword.DataBufferSize;
    ImageStream ImageStream;
    pthread_create(&handle, NULL, ImageStream.VideoStream, &TransfarDoc);

    char RecvMessage[1024] = {};
    char Sendemssage[32] = {};
    int  iResult = 0;

    time_t LastAckTime = 0;

    // Image Property Message. 
    std::snprintf(Sendemssage, sizeof(Sendemssage), "%d,%d,%d,", CAMERAWIDTH, CAMERAHIGHT, CAMERAFPS);

    // Start Process.
    printf("Main;Waiting ... Stop Ctr + Brk :\n");
    while (true) {
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
                // ReWait.
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

        // Error Process.
        if (0 != ErroCode) {
            wprintf(L"Main;Recvfrom failed with error %d\n", ErroCode);
            // Disconnect.
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

        // Only Ipv4.
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
        if (0 == strcmp(RecvMessage, STARTCODE)) {
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
