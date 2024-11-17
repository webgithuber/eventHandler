#include <winsock2.h>
#include <windows.h>
#include<ws2tcpip.h>
#include <iostream>
#include <unistd.h>
#include<thread>
#include <bits/stdc++.h>
#include "PriceUpdate.hpp"
using namespace std;



void send_update_to_client(SOCKET clientSOCKET)
{
    while (1)
    {
    PriceUpdate update=create_dynamic_test_price_update();
    string buff=update.to_json().dump();

    int iResult = send(clientSOCKET, buff.c_str(), buff.length(), 0);
    if (iResult == SOCKET_ERROR) {
        cerr << "Send failed: " << WSAGetLastError() << endl;
        closesocket(clientSOCKET);
        WSACleanup();
    }
    //this_thread::sleep_for(chrono::milliseconds(1));
    }
}

int main() {
    WSADATA wsaData;
    SOCKET ListenSocket = INVALID_SOCKET;
    struct sockaddr_in serverAddr;
    vector<SOCKET> Client;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed" << endl;
        return 1;
    }

    // Create a socket
    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        cerr << "Error creating socket: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    // Set up the sockaddr_in structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(8080);

    // Bind the socket
    if (bind(ListenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Bind failed: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    listen(ListenSocket, 5);// maximum number of connections that can be queued for acceptance.
    cout << "exchange is listening on port 8080..." << endl;

    

    while (true) {

    struct sockaddr_in clientAddr;
    int clientLen=sizeof(clientAddr);
    int *ptr=&clientLen;

        // Check if a new connection is requested
       
            SOCKET ClientSocket = accept(ListenSocket, (struct sockaddr*)&clientAddr, ptr);
            if (ClientSocket == INVALID_SOCKET) {
                cerr << "Accept failed: " << WSAGetLastError() << endl;
            } else {
                cout << "New client connected: " << ClientSocket << "\n";
                Client.push_back(ClientSocket);  // Add new client
                send_update_to_client(ClientSocket);
            }
       
     
    }

    // Cleanup
    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}
