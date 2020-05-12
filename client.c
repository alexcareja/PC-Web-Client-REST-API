#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include "helpers.h"
#include "requests.h"

#define POST "POST"
#define GET "GET"
#define DELETE "DELETE"
#define REGISTER "register"
#define LOGIN "login"
#define ACCESS "enter_library"
#define GET_BOOKS "get_books"
#define GET_BOOK "get_book"
#define ADD_BOOK "add_book"
#define DELETE_BOOK "delete_book"
#define LOGOUT "logout"
#define EXIT "exit"
#define REGISTER_PATH "/api/v1/tema/auth/register"
#define APP_JSON "application/json"
#define LOGIN_PATH "/api/v1/tema/auth/login"
#define ACCESS_PATH "/api/v1/tema/library/access"
#define BOOKS_PATH "/api/v1/tema/library/books"
#define LOGOUT_PATH "/api/v1/tema/auth/logout"
#define AUTH_HEADER_ERROR "Authorization header is missing!"

int check_login();
void register_client();
void login();
void get_access();
void get_books();
void get_book();
void add_book();
void delete_book();
void logout();
int handle_error(char *);

char url[] = "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com";
int sockfd;
char *login_cookie = NULL;
char *jwt_token = NULL;

int main(int argc, char const *argv[])
{
    char host_ip[] = "3.8.116.10";
    int port = 8080;
    char *command = (char *) calloc(256, sizeof(char));

    while (1) {
    	sockfd = open_connection(host_ip, port, AF_INET, SOCK_STREAM, 0);
    	memset(command, 0, 256);
    	scanf("%s", command);
    	if (strcmp(command, REGISTER) == 0) {
    		register_client();
    		continue;
    	}
    	if (strcmp(command, LOGIN) == 0) {
    		login();
    		continue;
    	}
    	if (strcmp(command, ACCESS) == 0) {
    		get_access();
    		continue;
    	}
    	if (strcmp(command, GET_BOOKS) == 0) {
    		get_books();
    		continue;
    	}
    	if (strcmp(command, GET_BOOK) == 0) {
    		get_book();
    		continue;
    	}
    	if (strcmp(command, ADD_BOOK) == 0) {
    		add_book();
    		continue;
    	}
    	if (strcmp(command, DELETE_BOOK) == 0) {
    		delete_book();
    		continue;
    	}
    	if (strcmp(command, LOGOUT) == 0) {
    		logout();
    		continue;
    	}
    	if (strcmp(command, EXIT) == 0) {
    		break;
    	}
    	printf("Unkown command\n");
    }
    close_connection(sockfd);
    free(command);
    return 0;
}

int check_login() {
	if (login_cookie == NULL) {
		printf("In order to perform this action you must be logged in.\n");
		return 1;
	}
	return 0;
}

int check_access() {
	if (jwt_token == NULL) {
		printf("You do not have acces to the library.\n");
		return 1;
	}
	return 0;
}

void register_client() {
	char *message;
	char *response;
	char type[5] = POST;	// request type
	char **body_data = (char **) malloc(sizeof(char *));
	body_data[0] = (char *) calloc(600, sizeof(char));
	char *uname = (char *) calloc(256, sizeof(char));
	char *pass = (char *) calloc(256, sizeof(char));
	// ask for username and password
	printf("username=");
	scanf("%s", uname);
	printf("password=");
	scanf("%s", pass);
	// create JSON payload
   	sprintf(body_data[0], "{ \"username\": \"%s\", \"password\": \"%s\" }",
   		uname, pass);
   	// create POST request
	message = compute_request(type, url, REGISTER_PATH, APP_JSON, body_data,
		1, NULL, NULL, 0);
	// send message to server
    send_to_server(sockfd, message);
    // get server response
    response = receive_from_server(sockfd);
    handle_error(response);
   	free(uname);
   	free(pass);
   	free(message);
   	free(response);
   	free(body_data[0]);
   	free(body_data);
}

void login() {
	if (login_cookie != NULL) {	// check if already logged in
		printf("You already are logged in.\n");
		return;
	}
	char *message;
	char *response;
	char *aux;
	char *cookie;
	char type[5] = POST;	// request type
	char **body_data = (char **) malloc(sizeof(char *));
	body_data[0] = (char *) calloc(600, sizeof(char));
	char *uname = (char *) calloc(256, sizeof(char));
	char *pass = (char *) calloc(256, sizeof(char));
	// ask for username and password
	printf("username=");
	scanf("%s", uname);
	printf("password=");
	scanf("%s", pass);
	// create JSON payload
   	sprintf(body_data[0], "{ \"username\": \"%s\", \"password\": \"%s\" }",
   		uname, pass);
   	// create POST request
	message = compute_request(type, url, LOGIN_PATH, APP_JSON, body_data,
		1, NULL, NULL, 0);
    // send message to server
    send_to_server(sockfd, message);
    // get server response
    response = receive_from_server(sockfd);
   	if (handle_error(response) == 0) {
    	// request was successful
   		// look for "Set-Cookie"
   		aux = response;
   		while (strncmp(aux, "Set-Cookie", 10)) {
   			aux++;
   		}
   		// pointer points to start of cookie
   		aux += 12;
   		cookie = strtok(aux, ";");
   		login_cookie = (char *) calloc(200, sizeof(char));
   		// copy cookie in global variable
   		strcpy(login_cookie, cookie);
   	}
   	// free allocated memory
   	free(uname);
   	free(pass);
   	free(message);
   	free(response);
   	free(body_data[0]);
   	free(body_data);
}

void get_access() {
	if (check_login()) { // check if logged in
		return;
	}
	if (jwt_token != NULL) {
		printf("You have already entered the library.\n");
		return;
	}
	char *message;
	char *response;
	char *token;
	char *aux;
	char type[4] = GET;	// request type
	char **cookies = (char **) malloc(sizeof(char *));
	cookies[0] = (char *) calloc(200, sizeof(char));
	// add the login cookie
	strcpy(cookies[0], login_cookie);
	// create GET request
	message = compute_request(type, url, ACCESS_PATH, NULL, NULL, 0, NULL,
		cookies, 1);
	// send message to sever
    send_to_server(sockfd, message);
    // get server response
    response = receive_from_server(sockfd);
    // handle error if failed or get the jwt token if successful
    if (handle_error(response) == 0) {
    	// request was successful
    	aux = response;
    	// look for token keyword
   		while(strncmp(aux, "token", 5)) {
   			aux++;
   		}
   		aux += 8;
   		// trim excess characters
   		token = strtok(aux, "\"");
   		jwt_token = (char *) calloc(400, sizeof(char));
   		// copy token value
   		strncpy(jwt_token, token, strlen(token));
    }
   	// free allocated memory
   	free(message);
   	free(response);
   	free(cookies[0]);
   	free(cookies);
}

void get_books() {
	if (check_access()) {
		return;
	}
	char *message;
	char *response;
	char *aux;
	char type[4] = GET;	// request type
	char **cookies = (char **) malloc(sizeof(char *));
	cookies[0] = (char *) calloc(200, sizeof(char));
	// add login cookie
	strcpy(cookies[0], login_cookie);
	// create GET request
	message = compute_request(type, url, BOOKS_PATH, NULL, NULL, 0, jwt_token,
		cookies, 1);
	// send message to server
    send_to_server(sockfd, message);
    // get server response
    response = receive_from_server(sockfd);
    // handle error if failed or print books if successful
    if (handle_error(response) == 0) {
    	// request was successful
   		aux = response;
   		// search for start of JSON object payload
   		while (strncmp(aux, "[", 1)) {
   			aux++;
   		}
   		printf("%s\n", aux);
    }
   	// free allocated memory
   	free(message);
   	free(response);
   	free(cookies[0]);
   	free(cookies);
}

void get_book() {
	if (check_access()) {
		return;
	}
	int i = 0, negative = 0;
	char *message;
	char *response;
	char *path = (char *) calloc(200, sizeof(char));
	char *aux;
	char type[4] = GET;	// request type
	char id[10];
	// ask for book id
	printf("id=");
	scanf("%s", id);
	// check if id is a number
	if (id[i] == '-') {
		negative = 1;
		i++;
	}
	for (; i < strlen(id); i++) {
		if (isdigit(id[i]) == 0) {
			// return and print message if NaN
			printf("Request failed. 'id' must be a number.\n");
			return;
		}
	}
	if (negative) {
		printf("Request failed. 'id' must be positive.\n");
		return;
	}
	// adapt path
	strcpy(path, BOOKS_PATH);
	strcat(path, "/");
	strcat(path, id);
	char **cookies = (char **) malloc(sizeof(char *));
	// add login cookie
	cookies[0] = (char *) calloc(200, sizeof(char));
	strcpy(cookies[0], login_cookie);
	// compute GET request
	message = compute_request(type, url, path, NULL, NULL, 0, jwt_token,
		cookies, 1);
	// send message to server
    send_to_server(sockfd, message);
    // get server response
    response = receive_from_server(sockfd);
    // handle error if failed or print book details if successful
    if (handle_error(response) == 0) {
    	// request was successful
    	aux = response;
    	// search for start of JSON payload
   		while (strncmp(aux, "[", 1)) {
   			aux++;
   		}
   		printf("%s\n", aux);
    }
   	// free allocated memory
   	free(message);
   	free(response);
   	free(cookies[0]);
   	free(cookies);
}

void add_book() {
	if (check_access()) {
		return;
	}
	char *message;
	char *response;
	int i;
	char **body_data = (char **) malloc(sizeof(char *));
	body_data[0] = (char *) calloc(600, sizeof(char));
	char type[10] = POST;	// request type
	char title[100], author[100], genre[100], publisher[100], pages[10];
	// ask for title, author, genre, publisher, and page_count
	printf("title=");
	scanf("%s", title);
	printf("author=");
	scanf("%s", author);
	printf("genre=");
	scanf("%s", genre);
	printf("publisher=");
	scanf("%s", publisher);
	printf("page_count=");
	scanf("%s", pages);
	// check if page_count is a number
	for (i = 0; i < strlen(pages); i++) {
		if (isdigit(pages[i]) == 0) {
			// print message and return if failed
			printf("Request failed. 'page_count' must be a number.\n");
			return;
		}
	}
	// create JSON payload
	sprintf(body_data[0],
		"{ \"title\": \"%s\", \"author\": \"%s\", \"genre\": \"%s\", \"page_count\": \"%s\", \"publisher\": \"%s\" }",
   		title, author, genre, pages, publisher);
	char **cookies = (char **) malloc(sizeof(char *));
	cookies[0] = (char *) calloc(200, sizeof(char));
	// add login cookie
	strcpy(cookies[0], login_cookie);
	// create POST request
	message = compute_request(type, url, BOOKS_PATH, APP_JSON, body_data, 1,
		jwt_token, cookies, 1);
	// send message to server
    send_to_server(sockfd, message);
    // get server response
    response = receive_from_server(sockfd);
    // handle error if request failed
    handle_error(response);
   	// free allocated memory
   	free(message);
   	free(response);
   	free(cookies[0]);
   	free(cookies);
   	free(body_data[0]);
   	free(body_data);
}

void delete_book() {
	if (check_access()) {
		return;
	}
	int i;
	char *message;
	char *response;
	char *path = (char *) calloc(200, sizeof(char));
	char type[10] = DELETE;	// request type
	char id[10];
	// ask for book id
	printf("id=");
	scanf("%s", id);
	// check if id is a number
	for (i = 0; i < strlen(id); i++) {
		if (isdigit(id[i]) == 0) {
			// print message and return if NaN
			printf("Request failed. 'id' must be a number.\n");
			return;
		}
	}
	// adapt path
	strcpy(path, BOOKS_PATH);
	strcat(path, "/");
	strcat(path, id);
	char **cookies = (char **) malloc(sizeof(char *));
	// add login cookie
	cookies[0] = (char *) calloc(200, sizeof(char));
	strcpy(cookies[0], login_cookie);
	// compute DELETE request
	message = compute_request(type, url, path, NULL, NULL, 0, jwt_token,
		cookies, 1);
	// send message to server
    send_to_server(sockfd, message);
    // get server response
    response = receive_from_server(sockfd);
    // handle error if request failed
    handle_error(response);
   	// free allocated memory
   	free(message);
   	free(response);
   	free(cookies[0]);
   	free(cookies);
}

void logout() {
	if (check_login()) {
		return;
	}
	char *message;
	char *response;
	char type[4] = GET;	// request type
	char **cookies = (char **) malloc(sizeof(char *));
	cookies[0] = (char *) calloc(200, sizeof(char));
	// add login cookie
	strcpy(cookies[0], login_cookie);
	// compute GET request
	message = compute_request(type, url, LOGOUT_PATH, NULL, NULL, 0,
		NULL, cookies, 1);
	// send message to sever
    send_to_server(sockfd, message);
    // get server response
    response = receive_from_server(sockfd);
    // handle error if failed or delete credentials if successful
    if (handle_error(response) == 0) {
    	// request was successful
    	// get rid of stored credentials
    	free(login_cookie);
    	login_cookie = NULL;
    	if (jwt_token != NULL) {
    		free(jwt_token);
    		jwt_token = NULL;
    	}
    }
   	// free allocated memory
   	free(message);
   	free(response);
   	free(cookies[0]);
   	free(cookies);
}

int handle_error(char *response) {
	char *token;
	token = strtok(response, " ");
	token = strtok(NULL, " ");
	if (token[0] == '2') {
		// request was successful
		return 0;
	} else {
		// request failed
		if (token[0] == '4') {
			// bad request
			char *aux;
			aux = response;
			// search for error message
			while(strncmp(aux, "error", 5)) {
				aux++;
			}
			aux += 8;
			token = strtok(aux, "\"");
			printf("%s\n", token);
		}
	}
	return 1;
}
