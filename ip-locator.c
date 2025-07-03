#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include <regex.h>
#include "cJSON/cJSON.h"

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

void parse_and_print(struct response_data* rd) {
    cJSON* parsed_JSON = cJSON_Parse(rd->response);

    if (parsed_JSON == NULL)
        fprintf(stderr, "JSON parsing failed\n");
    else {
        if (!strcmp(cJSON_GetObjectItemCaseSensitive(parsed_JSON, "status")->valuestring, "success")) {
            printf("Location: %s, %s, %s\n",
                    cJSON_GetObjectItemCaseSensitive(parsed_JSON, "city")->valuestring,
                    cJSON_GetObjectItemCaseSensitive(parsed_JSON, "regionName")->valuestring,
                    cJSON_GetObjectItemCaseSensitive(parsed_JSON, "country")->valuestring);
            printf("Coordinates: %f, %f\n",
                    cJSON_GetObjectItemCaseSensitive(parsed_JSON, "lat")->valuedouble,
                    cJSON_GetObjectItemCaseSensitive(parsed_JSON, "lon")->valuedouble);
            printf("ISP: %s\n",
                    cJSON_GetObjectItemCaseSensitive(parsed_JSON, "isp")->valuestring);
            printf("Organisation: %s\n",
                    cJSON_GetObjectItemCaseSensitive(parsed_JSON, "org")->valuestring);
        } else
            printf("IP could not be located\n");
    }

    cJSON_Delete(parsed_JSON);
}

int main() {
    char full_addr[38] = "http://ip-api.com/json/";
    char ip_addr[20]; //Add additional space in case of incorrect input

    printf("Enter IP address: ");
    int ch, i = 0;

    while ((ch = getchar()) != '\n' && i < 19) {
        ip_addr[i] = ch;
        i++;
    }

    strcat(full_addr, ip_addr);

    if(ip_validator(ip_addr) != 0)
        exit(1);

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

        parse_and_print(&rd);
        free(rd.response);

        curl_easy_cleanup(handle);
    } else
        fprintf(stderr, "curl setup failed\n");
}