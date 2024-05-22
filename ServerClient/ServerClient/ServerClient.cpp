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
        cerr << "WSAStartup failed with error: " << result << endl;
        return 1;
    }

    // Настройка параметров для получения информации об адресе сервера
    ADDRINFO hints;
    ADDRINFO* addrResult;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;             // IPv4 адреса
    hints.ai_socktype = SOCK_STREAM;       // TCP соединение
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;           // Автоматически определить IP-адрес хоста

    // Получение информации об адресе сервера
    result = getaddrinfo(NULL, "666", &hints, &addrResult);
    if (result != 0) {
        cerr << "getaddrinfo failed with error: " << result << endl;
        WSACleanup();
        return 1;
    }

    // Создание сокета для прослушивания входящих соединений
    SOCKET ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        cerr << "socket failed with error: " << WSAGetLastError() << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    // Привязка сокета к адресу и порту
    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cerr << "bind failed with error: " << WSAGetLastError() << endl;
        freeaddrinfo(addrResult);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(addrResult);

    // Прослушивание входящих соединений
    result = listen(ListenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        cerr << "listen failed with error: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    cout << "Waiting for client connection..." << endl;

    // Принятие входящего соединения от клиента
    SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        cerr << "accept failed with error: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Закрытие сокета для прослушивания входящих соединений
    closesocket(ListenSocket);

    // Буферы для приема и отправки данных
    char recvBuffer[512];
    const char* sendBuffer = "Tokio style";

    // Прием и отправка данных в цикле, пока есть данные от клиента
    do {
        // Обнуление буфера приема
        ZeroMemory(recvBuffer, sizeof(recvBuffer));

        // Прием данных от клиента
        result = recv(ClientSocket, recvBuffer, sizeof(recvBuffer), 0);
        if (result > 0) {
            // Вывод принятых данных
            cout << "Received " << result << " bytes: " << recvBuffer << endl;
        }
        else if (result == 0) {
            // Закрытие соединения клиентом
            cout << "Connection closed by client" << endl;
        }
        else {
            // Ошибка при приеме данных
            cerr << "recv failed with error: " << WSAGetLastError() << endl;
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

        // Отправка данных клиенту в ответ
        result = send(ClientSocket, sendBuffer, (int)strlen(sendBuffer), 0);
        if (result == SOCKET_ERROR) {
            // Ошибка при отправке данных
            cerr << "send failed with error: " << WSAGetLastError() << endl;
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

    } while (result > 0);

    // Закрытие соединения с клиентом и очистка Winsock
    closesocket(ClientSocket);
    WSACleanup();
    return 0;
}
