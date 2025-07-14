#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <curl/curl.h>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::ofstream* out = static_cast<std::ofstream*>(userp);
    size_t totalSize = size * nmemb;
    out->write(static_cast<char*>(contents), totalSize);
    return totalSize;
}

void downloadImage(const std::string& url, const std::string& filename) {
    CURL* curl;
    CURLcode res;
    std::ofstream outFile(filename, std::ios::binary);
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outFile);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    outFile.close();
}

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
    int imageCount = 0;
    std::string directory = "images/";
    CreateDirectoryA(directory.c_str(), NULL);
    std::cout << "Press Enter to download a random image...\n";
    while (true) {
        std::cin.get();
        std::string imageUrl = (rand() % 2 == 0) ? "https://cataas.com/cat" : getDogImageUrl();
        if (!imageUrl.empty()) {
            std::string filename = directory + "img_" + std::to_string(imageCount) + ".jpg";
            downloadImage(imageUrl, filename);
            std::cout << "Saved: " << filename << std::endl;
            imageCount++;
        }
    }
    return 0;
}
