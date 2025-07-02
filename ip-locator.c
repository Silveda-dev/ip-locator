#include <stdio.h>
#include <curl/curl.h>
#include <string.h>

int main() {
    char full_addr[38] = "http://ip-api.com/json/";
    char ip_addr[20]; //Add additional space in case of error

    printf("Enter IP address: ");
    scanf("%s", &ip_addr);
    strcat(full_addr, ip_addr);

    CURL* handle = curl_easy_init();
    //DEBUG LINE printf("Creating connection to %s\n", full_addr);

    if (handle) {
        curl_easy_setopt(handle, CURLOPT_URL, full_addr);
        CURLcode ret_code = curl_easy_perform(handle);

        if (ret_code != CURLE_OK)
            fprintf(stderr, "curl connection failed. Error: %s\n", curl_easy_strerror(ret_code));

        curl_easy_cleanup(handle);
    } else
        fprintf(stderr, "curl setup failed\n");
}