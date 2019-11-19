//  x86_64-w64-mingw32-c++ -static-libgcc -static-libstdc++  main.cpp -o a.exe -lwsock32 -lws2_32 -o sock.exe

#define _WIN32_WINNT 0x501
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// 連結 Ws2_32.lib, Mswsock.lib, Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#define default_BUFLEN 2147489
#define default_PORT "8080"

char recvbuf[default_BUFLEN];
int count=0;

int __cdecl main(int argc, char **argv)
{
 WSADATA wsaData;
 SOCKET ConnectSocket = INVALID_SOCKET;
 addrinfo *result=NULL,*ptr = NULL,hints;
 char *ID = "nn";



 while(1)
 { 
  Sleep(500);
  int iResult;
  int recvbuflen=default_BUFLEN;

  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

  if (iResult != 0) 
  {
   printf("WSAStartup failed with error: %d\n", iResult);
   return 1;
  }

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  char *a="127.0.0.1";
  iResult = getaddrinfo(a, default_PORT, &hints, &result);
  if ( iResult != 0 ) 
  {
   printf("getaddrinfo failed with error: %d\n", iResult);
   WSACleanup();
   return 1;
  }
  for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) 
  {
   ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
   if (ConnectSocket == INVALID_SOCKET) 
   {
    printf("socket failed with error: %ld\n", WSAGetLastError());
    WSACleanup();
    return 1;
   }
   // Connect to server.
   iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
   if (iResult == SOCKET_ERROR) 
   {
    closesocket(ConnectSocket);
    ConnectSocket = INVALID_SOCKET;
    continue;
   }
   break;
  }

  freeaddrinfo(result);
  if (ConnectSocket == INVALID_SOCKET) 
  {
   printf("Unable to connect to server!\n");
   WSACleanup();
   return 1;
  }
  
  
  iResult = send( ConnectSocket, ID, (int)strlen(ID), 0 );
  if (iResult == SOCKET_ERROR) 
  {
   printf("send failed with error: %d\n", WSAGetLastError());
   closesocket(ConnectSocket);
   WSACleanup();
   return 1;
  }
  printf("Bytes Sent: %ld\n", iResult);

  
  Sleep(10);
  iResult = shutdown(ConnectSocket, SD_SEND);
  if (iResult == SOCKET_ERROR) 
  {
   printf("shutdown failed with error: %d\n", WSAGetLastError());
   closesocket(ConnectSocket);
   WSACleanup();
   return 1;
  }


  do 
  {
   iResult = recv(ConnectSocket, recvbuf, 214748, 0);
   if ( iResult > 0 ) printf("Bytes received: %d\n", iResult);
   else if ( iResult == 0 ) printf("Connection closed\n");
   else printf("recv failed with error: %d\n", WSAGetLastError());
  } 
  while( iResult > 0 );

  for(int tt = 0;tt < strlen(recvbuf);tt++) printf("%c",recvbuf[tt]);
  printf("==========================================\n");

  // cleanup
  closesocket(ConnectSocket);
  WSACleanup();
 }
 return 0;
}