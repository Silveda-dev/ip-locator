#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include <regex.h>

struct response_data {
    char* response;
    size_t size;
};

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

size_t writefunc(char* data, size_t sz, size_t n, struct response_data* rd) {
    size_t new_size = rd->size + sz * n;
    rd->response = realloc(rd->response, new_size+1);

    if (rd->response == NULL) {
        fprintf(stderr, "realloc() error\n");
        exit(1);
    }

    memcpy(rd->response + rd->size, data, sz*n);
    rd->size = new_size;
    rd->response[rd->size] = '\0';

    return sz*n;
}

int main() {
    char full_addr[38] = "http://ip-api.com/json/";
    char ip_addr[20]; //Add additional space in case of error

    printf("Enter IP address: ");
    scanf("%s", &ip_addr);
    strcat(full_addr, ip_addr);

    if(ip_validator(ip_addr) != 0)
        return 0;

    //DEBUG LINE printf("Creating connection to %s\n", full_addr);

    CURL* handle = curl_easy_init();

    if (handle) {
        struct response_data rd;
        rd.size = 0;
        rd.response = calloc(1, sizeof(char));

        curl_easy_setopt(handle, CURLOPT_URL, full_addr);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &rd);

        CURLcode ret_code = curl_easy_perform(handle);

        if (ret_code != CURLE_OK) {
            fprintf(stderr, "curl connection failed. Error: %s\n", curl_easy_strerror(ret_code));
            return 1;
        }

        printf("%s\n", rd.response);
        free(rd.response);

        curl_easy_cleanup(handle);
    } else
        fprintf(stderr, "curl setup failed\n");
}