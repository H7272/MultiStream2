#pragma once
#define HAVE_STRUCT_TIMESPEC // [Add] 
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <pthread.h>
static pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;

class ClientBuffer
{
	private:
	std::vector<sockaddr_in*> pClientSocketBuffer;

	public:
	bool SarchDisconnectClient(sockaddr_in* pClientSocket);

	bool SarchConnectedClient(sockaddr_in* pClientSocket);

    void DeleteAllClient(void);

	void PushBuck(sockaddr_in* pClientSocket) ;

	void SendImage(SOCKET* pListenSocket, char* sendbuf, int DataBufferSize);

	inline size_t IsSize(void){return pClientSocketBuffer.size();}


};

