#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <regex.h>

int ip_validator(const char* ip) {
    if (8 <= sizeof(ip)/sizeof(ip[0]) <= 16) { //Check IP length is within bounds
        regex_t regex_var;
        int regex_result = regcomp(&regex_var, "([0-9]{1,3}[.]){3}([0-9]{1,3})", REG_EXTENDED);

        if (!regex_result) {
            regex_result = regexec(&regex_var, ip, 0, NULL, 0);
            if (!regex_result)
                return 0; //Valid IP format
            else {
                fprintf(stderr, "Invalid IP format\n");
                return -1;
            }
        } else {
            fprintf(stderr, "REGEX compilation error\n");
            return -1;
        }
    } else
        return -1;
}

int main() {
    char full_addr[38] = "http://ip-api.com/json/";
    char ip_addr[20]; //Add additional space in case of error

    printf("Enter IP address: ");
    scanf("%s", &ip_addr);
    strcat(full_addr, ip_addr);

    if(ip_validator(ip_addr) != 0)
        return 0;

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