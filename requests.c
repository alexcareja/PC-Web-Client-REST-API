#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_request(char *type, char *host, char *url, char* content_type, 
                            char **body_data, int body_data_fields_count,
                            char *authorization, char **cookies, int cookies_count) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    int i;

    // write the method type, URL and protocol type
    sprintf(line, "%s %s HTTP/1.1", type, url);
    compute_message(message, line);
    
    // add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    // add necessary headers (Content-Type and Content-Length if neccessary)
    if (content_type != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Content-Type: %s", content_type);
        compute_message(message, line);
        int content_length = 0;
        for (i = 0; i < body_data_fields_count; i++) {
            content_length += strlen(body_data[i]);
        }
        memset(line, 0, LINELEN);
        sprintf(line, "Content-Length: %d", content_length);
        compute_message(message, line);
    }

    // add authorization header if given
    if (authorization != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", authorization);
        compute_message(message, line);
    }

    // add cookies if given
    if (cookies != NULL) {
        memset(line, 0, LINELEN);  
        sprintf(line, "Cookie: %s", cookies[0]);
        int i;
        for (i = 1; i < cookies_count; i++) {
            strcat(line, "; ");
            strcat(line, cookies[i]);
        }
        compute_message(message, line);
    }
    // add new line at end of header
    compute_message(message, "");
    // add the actual payload data
    if (body_data != NULL) {
        memset(line, 0, LINELEN);
        for (i = 0; i < body_data_fields_count; i++) {
            strcat(message, body_data[i]);
        }
    }
    free(line);
    return message;
}
