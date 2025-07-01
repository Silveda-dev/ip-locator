#include <stdio.h>
#include <curl/curl.h>
#include <string.h>

int main() {
    char full_addr[38] = "http://ip-api.com/json/";
    char ip_addr[16];
    printf("Enter IP address: ");
    scanf("%s", &ip_addr);

    curl_global_init(CURL_GLOBAL_ALL);
    CURL* handle = curl_easy_init();
    CURLcode ret_code;

    strcat(full_addr, ip_addr);

    printf("Creating connection to %s\n", full_addr);

    if (handle) {
        curl_easy_setopt(handle, CURLOPT_URL, full_addr);
        ret_code = curl_easy_perform(handle);

        if (ret_code != CURLE_OK)
            fprintf(stderr, "curl failed. Error: %s\n", curl_easy_strerror(ret_code));
        else
            printf("Successful GET request\n");

        curl_easy_cleanup(handle);
    } else
        fprintf(stderr, "curl setup failed\n");
}