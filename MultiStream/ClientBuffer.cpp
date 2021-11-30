#include "ClientBuffer.h"

/**
 * @SarchDisconnectClient .
 * @brief Image Captur And Send to Client.
 */
bool ClientBuffer::SarchDisconnectClient(sockaddr_in* pClientSocket) {
    pthread_mutex_lock(&count_mutex);
    char ErrorClientIP[INET6_ADDRSTRLEN] = {};
    inet_ntop(pClientSocket->sin_family, &(pClientSocket->sin_addr), ErrorClientIP, sizeof(ErrorClientIP));
    printf(" Sarch DisconnectClient In Buffer %s\n", ErrorClientIP);

    std::vector<sockaddr_in*>::iterator itrClient = pClientSocketBuffer.begin();
    while (itrClient != pClientSocketBuffer.end()) {
        char BufferIP[INET6_ADDRSTRLEN] = {};
        sockaddr_in* pInBufferClientSocket = *itrClient;
        inet_ntop(pInBufferClientSocket->sin_family, &(pInBufferClientSocket->sin_addr), BufferIP, sizeof(BufferIP));

        if (strcmp(ErrorClientIP, BufferIP) == 0) {
            itrClient = pClientSocketBuffer.erase(itrClient);
            printf(" Erase Client From Buffer %s\n", ErrorClientIP);
            printf(" Client Num %d :\n", (int)pClientSocketBuffer.size());
            delete pInBufferClientSocket;
            break;
        }
        ++itrClient;
    }
    pthread_mutex_unlock(&count_mutex);
    return true;
}

/**
 * @SarchDisconnectClient .
 * @brief Image Captur And Send to Client.
 */
bool ClientBuffer::SarchConnectedClient(sockaddr_in* pClientSocket) {
    pthread_mutex_lock(&count_mutex);

    char ErrorClientIP[INET6_ADDRSTRLEN] = {};
    inet_ntop(pClientSocket->sin_family, &(pClientSocket->sin_addr), ErrorClientIP, sizeof(ErrorClientIP));

    std::vector<sockaddr_in*>::iterator itrClient = pClientSocketBuffer.begin();
    while (itrClient != pClientSocketBuffer.end()) {
        char BufferIP[INET6_ADDRSTRLEN] = {};
        sockaddr_in* pInBufferClientSocket = *itrClient;
        inet_ntop(pInBufferClientSocket->sin_family, &(pInBufferClientSocket->sin_addr), BufferIP, sizeof(BufferIP));

        if (strcmp(ErrorClientIP, BufferIP) == 0) {
            pthread_mutex_unlock(&count_mutex);
            return true;
        }
        ++itrClient;
    }
    pthread_mutex_unlock(&count_mutex);
    return false;
}

/**
 * @SarchDisconnectClient .
 * @brief Image Captur And Send to Client.
 */
void ClientBuffer::DeleteAllClient(void) {
    pthread_mutex_lock(&count_mutex);

    std::vector<sockaddr_in*>::iterator itrClient = pClientSocketBuffer.begin();
    while (itrClient != pClientSocketBuffer.end()) {
        sockaddr_in* pInBufferClientSocket = *itrClient;
        itrClient = pClientSocketBuffer.erase(itrClient);
        delete pInBufferClientSocket;
    }
    pthread_mutex_unlock(&count_mutex);
    return;
}

void ClientBuffer::PushBuck(sockaddr_in*  pClientSocket){
    pthread_mutex_lock(&count_mutex);
    pClientSocketBuffer.push_back(pClientSocket);
    pthread_mutex_unlock(&count_mutex);
    return;
}

void ClientBuffer::SendImage(SOCKET* pListenSocket, char* sendbuf, int DataBufferSize){
    pthread_mutex_lock(&count_mutex);
    std::vector<sockaddr_in*>::iterator itrClient = pClientSocketBuffer.begin();
    while (itrClient != pClientSocketBuffer.end()) {
        sendto(*pListenSocket, sendbuf, DataBufferSize, 0, (SOCKADDR*)*itrClient, sizeof(**itrClient));
        Sleep(1);
        ++itrClient;
     }
    pthread_mutex_unlock(&count_mutex);
}