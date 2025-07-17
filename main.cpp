#include <iostream>              // для cin, cout
#include <string>                // string — для роботи з текстом (url, імена файлів)
#include <fstream>               // щоб писати картинки в файл
#include <cstdlib>               // rand(), srand() — рандом і ініціалізація
#include <ctime>                 // time() — для сидування rand
#include <windows.h>            // CreateDirectoryA — створює папку в Windows
#include <curl/curl.h>          // бібліотека для HTTP-запитів (скачування)

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::ofstream* out = static_cast<std::ofstream*>(userp);  // userp — це наш відкритий файл
    size_t totalSize = size * nmemb;                          // загальний розмір того що прийшло
    out->write(static_cast<char*>(contents), totalSize);     // пишемо в файл
    return totalSize;                                         // повертаємо скільки байт записали
}

void downloadImage(const std::string& url, const std::string& filename) {
    CURL* curl;                       // curl об’єкт для запиту
    CURLcode res;                     // для перевірки помилок
    std::ofstream outFile(filename, std::ios::binary);  // відкриваємо файл на запис (в двійковому режимі)

    curl_global_init(CURL_GLOBAL_DEFAULT);   // ініціалізація curl
    curl = curl_easy_init();                 // створюємо curl-об’єкт

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());               // задаємо URL
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);   // кажемо як писати
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outFile);            // передаємо наш файл
        res = curl_easy_perform(curl);                                  // виконуємо запит
        curl_easy_cleanup(curl);                                        // прибираємо
    }

    curl_global_cleanup();  // загальне прибирання
    outFile.close();        // закриваємо файл
}

std::string getDogImageUrl() {
    CURL* curl;
    CURLcode res;
    std::string data;   // сюди запишеться відповідь (JSON)

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://dog.ceo/api/breeds/image/random");  // URL JSON
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void* contents, size_t size, size_t nmemb, void* userp) {
            ((std::string*)userp)->append((char*)contents, size * nmemb);  // просто додаємо у рядок
            return size * nmemb;
        });
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    size_t start = data.find("\"message\":\"");  // шукаємо де починається посилання
    if (start == std::string::npos) return "";   // не знайшли — нічого не повертаємо
    start += 11;                                 // пропускаємо до самого URL
    size_t end = data.find("\"", start);
    if (end == std::string::npos) return "";
    return data.substr(start, end - start);      // витягуємо чистий URL
}

int main() {
    srand(static_cast<unsigned int>(time(0)));  // сид для рандому

    int imageCount = 0;                         // лічильник збережених картинок
    std::string directory = "images/";          // назва папки
    CreateDirectoryA(directory.c_str(), NULL);  // створюємо папку, якщо ще нема

    std::cout << "Press Enter to download a random image...\n";

    while (true) {
        std::cin.get();   // чекаємо на Enter

        // вибираємо або кота або пса
        std::string imageUrl = (rand() % 2 == 0) ? "https://cataas.com/cat" : getDogImageUrl();

        if (!imageUrl.empty()) {
            std::string filename = directory + "img_" + std::to_string(imageCount) + ".jpg";  // нове ім’я
            downloadImage(imageUrl, filename);                                                // качаємо
            std::cout << "Saved: " << filename << std::endl;
            imageCount++;  // збільшуємо лічильник
        }
    }

    return 0;
}
