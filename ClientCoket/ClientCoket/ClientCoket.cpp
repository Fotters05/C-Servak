#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <Winsock2.h>
#include <WS2tcpip.h>

using namespace std;

int main()
{
    // Инициализация Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "WSAStartup failed" << endl;
        return 1;
    }

    // Настройка параметров для получения информации об адресе сервера
    ADDRINFO hints;
    ADDRINFO* addrResult;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;             // IPv4 адреса
    hints.ai_socktype = SOCK_STREAM;       // TCP соединение
    hints.ai_protocol = IPPROTO_TCP;

    // Получение информации об адресе сервера
    result = getaddrinfo("127.0.0.1", "666", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo failed" << endl;
        WSACleanup();
        return 1;
    }

    // Создание сокета для соединения с сервером
    SOCKET ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "socket failed" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Подключение к серверу
    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "connect failed" << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    cout << "Connected to server" << endl;

    // Отправка первого сообщения на сервер
    const char* sendBuffer1 = "First";
    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);
    if (result == SOCKET_ERROR) {
        cout << "send failed error" << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    cout << "Sent first message to server" << endl;

    // Получение подтверждения от сервера после отправки первого сообщения
    char recvBuffer[512];
    result = recv(ConnectSocket, recvBuffer, sizeof(recvBuffer), 0);
    if (result > 0) {
        recvBuffer[result] = '\0';
        cout << "Received " << result << " bytes: " << recvBuffer << endl;
    }
    else if (result == 0) {
        cout << "Connection closed by server" << endl;
    }
    else {
        cout << "recv failed with error" << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Отправка второго сообщения на сервер
    const char* sendBuffer2 = "Second";
    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
    if (result == SOCKET_ERROR) {
        cout << "send failed error" << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    cout << "Sent second message to server" << endl;

    // Получение подтверждения от сервера после отправки второго сообщения
    result = recv(ConnectSocket, recvBuffer, sizeof(recvBuffer), 0);
    if (result > 0) {
        recvBuffer[result] = '\0';
        cout << "Received " << result << " bytes: " << recvBuffer << endl;
    }
    else if (result == 0) {
        cout << "Connection closed by server" << endl;
    }
    else {
        cout << "recv failed with error" << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Закрытие сокета и очистка Winsock
    closesocket(ConnectSocket);
    WSACleanup();
    return 0;
}
