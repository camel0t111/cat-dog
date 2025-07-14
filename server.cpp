#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <curl/curl.h>

#pragma comment(lib, "ws2_32.lib")

std::string getDogImageUrl() {
    CURL* curl;
    CURLcode res;
    std::string data;
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://dog.ceo/api/breeds/image/random");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void* contents, size_t size, size_t nmemb, void* userp) {
            ((std::string*)userp)->append((char*)contents, size * nmemb);
            return size * nmemb;
        });
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    size_t start = data.find("\"message\":\"");
    if (start == std::string::npos) return "";
    start += 11;
    size_t end = data.find("\"", start);
    if (end == std::string::npos) return "";
    return data.substr(start, end - start);
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5000);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, SOMAXCONN);
    while (true) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        std::string imageUrl = (rand() % 2 == 0) ? "https://cataas.com/cat" : getDogImageUrl();
        if (!imageUrl.empty()) {
            CURL* curl;
            CURLcode res;
            std::string imageData;
            curl = curl_easy_init();
            if (curl) {
                curl_easy_setopt(curl, CURLOPT_URL, imageUrl.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void* contents, size_t size, size_t nmemb, void* userp) {
                    ((std::string*)userp)->append((char*)contents, size * nmemb);
                    return size * nmemb;
                });
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &imageData);
                res = curl_easy_perform(curl);
                curl_easy_cleanup(curl);
            }
            send(clientSocket, imageData.c_str(), static_cast<int>(imageData.size()), 0);
        }
        closesocket(clientSocket);
    }
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
