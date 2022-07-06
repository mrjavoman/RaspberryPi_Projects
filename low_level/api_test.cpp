#include <string>
#include <iostream>
#include <filesystem>
// #include "httplib.h"
#include <curl/curl.h>


int main() {

    std::string picToSend = "";

    // Get picture
    std::string path = "../test_images";
    for(const auto &entry : std::filesystem::directory_iterator(path)) {
        std::cout << entry.path() << std::endl;
        picToSend = entry.path();
    }

    // Send picture using Pushover api.
    //httplib::Client cli("https://api.pushover.net");

    //cli
    //std::string body = "{token : abc5kk96z5ave92xg8edp6cc89dnui , user : ur8ix1abhx1s8wn3s4s6m8dd3trbiu , message : ("Alert: " + msg_title)},files = {"attachment": (str(file), open(result_image ,"rb"), "image/jpeg")}

    /* 
        compile command
         g++ api_test.cpp -o apiTest -std=c++17 -I/home/xavi/anaconda3/include  -L/home/xavi/anaconda3/lib -lcurl

        Articles with examples for curl post
        https://gist.github.com/alghanmi/c5d7b761b2c9ab199157
        https://curl.se/libcurl/c/simplepost.html
        https://stackoverflow.com/questions/8251325/how-do-i-post-a-buffer-of-json-using-libcurl


    */


    return 0;
}