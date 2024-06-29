#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <time.h>

#define URL "https://example.com"

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    return size * nmemb;
}

double make_request(CURL *curl, const char *url, long http_version) {
    CURLcode res;
    double time_taken;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, http_version);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return -1;
    }

    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &time_taken);
    return time_taken;
}

int main(void) {
    CURL *curl;
    double time_http1, time_http2, time_http3;

    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Curl initialization failed\n");
        return 1;
    }

    // http 1
    time_http1 = make_request(curl, URL, CURL_HTTP_VERSION_1_1);
    printf("HTTP/1.1 request took %.2f seconds\n", time_http1);

    // http 2
    time_http2 = make_request(curl, URL, CURL_HTTP_VERSION_2);
    printf("HTTP/2 request took %.2f seconds\n", time_http2);

    // http 3
    time_http3 = make_request(curl, URL, CURL_HTTP_VERSION_3);
    printf("HTTP/3 request took %.2f seconds\n", time_http3);

    printf("\nComparison:\n");
    printf("HTTP/2 is %.2f%% faster than HTTP/1.1\n", (time_http1 - time_http2) / time_http1 * 100);
    printf("HTTP/3 is %.2f%% faster than HTTP/1.1\n", (time_http1 - time_http3) / time_http1 * 100);
    printf("HTTP/3 is %.2f%% faster than HTTP/2\n", (time_http2 - time_http3) / time_http2 * 100);

    curl_easy_cleanup(curl);
    return 0;
}
