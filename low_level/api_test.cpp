
/* 
    compile command
        g++ api_test.cpp -o apiTest -std=c++17 -I/home/xavi/anaconda3/include  -L/home/xavi/anaconda3/lib -lcurl

    Articles with examples for curl post
    https://gist.github.com/alghanmi/c5d7b761b2c9ab199157
    https://curl.se/libcurl/c/simplepost.html
    https://stackoverflow.com/questions/8251325/how-do-i-post-a-buffer-of-json-using-libcurl

    https://stackoverflow.com/questions/69073875/formatting-array-of-data-into-multipart-form-data
    https://stackoverflow.com/questions/48439624/python-publish-picture-attachment-to-pushover
    https://cpp.hotexamples.com/examples/-/-/curl_formadd/cpp-curl_formadd-function-examples.html
*/
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include "httplib.h"
//#include <curl/curl.h>

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main() {

    // CURL *curl;
    // CURLcode res;
    // std::string readBuffer;

    std::string picToSend = "";
    std::string picName = "";

    // Get picture
    std::string path = "../test_images";
    for(const auto &entry : std::filesystem::directory_iterator(path)) {
        std::cout << entry.path() << std::endl;
        picToSend = entry.path();
        picName = entry.path().filename();
    }

    // Read picture
    std::fstream fin("cloud.jpg", std::ios::binary);

    std::ostringstream ss;
    ss << fin.rdbuf();
    std::string picData = ss.str();

    // Send picture using Pushover api.
    httplib::Client cli("https://api.pushover.net");
    httplib::MultipartFormDataItems items = {
        { "token", "abc5kk96z5ave92xg8edp6cc89dnui" },
        { "user", "ur8ix1abhx1s8wn3s4s6m8dd3trbiu" },
        { "message", "Alert: " + picName },
        { "files", picName, picData, "image/jpeg" }
    };

    auto res = cli.Post("/1/messages.json", items);

    std::cout << res->status << std::endl;
    //std::string body = "{token : abc5kk96z5ave92xg8edp6cc89dnui , user : ur8ix1abhx1s8wn3s4s6m8dd3trbiu , message : ("Alert: " + msg_title)},files = {"attachment": (str(file), open(result_image ,"rb"), "image/jpeg")}

    //std::string payload = "{\"token\" : \"abc5kk96z5ave92xg8edp6cc89dnui\", \"user\" : \"ur8ix1abhx1s8wn3s4s6m8dd3trbiu\", message}";

    // curl = curl_easy_init();
    // if(curl) {
    //     curl_easy_setopt(curl, CURLOPT_URL, "https://api.pushover.net/1/messages.json");
    //     curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);

    // }



    fin.close();

    return 0;
}