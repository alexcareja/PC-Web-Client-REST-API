#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

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

void register_client();
void login();
void get_access();
void get_books();
void get_book();
void add_book();
void delete_book();
void logout();

char url[] = "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com";
int sockfd;
char login_cookie[200];
char jwt_token[400];

int main(int argc, char const *argv[])
{
    char host_ip[] = "3.8.116.10";
    int port = 8080;
    char *command = (char *) calloc(256, sizeof(char));

    sockfd = open_connection(host_ip, port, AF_INET, SOCK_STREAM, 0);

    
    while (1) {
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
    // Ex 1.1: GET dummy from main server
    // Ex 1.2: POST dummy and print response from main server
    // Ex 2: Login into main server
    // char **login = (char **) malloc(2 * sizeof(char *));
    // login[0] = (char *) calloc(20, sizeof(char));
    // login[1] = (char *) calloc(20, sizeof(char));
    // strcpy(login[0], "username=student");
    // strcpy(login[1], "password=student");
    // message = compute_post_request(url, "/api/v1/auth/login", "application/x-www-form-urlencoded", 
    //     login, 2, NULL, 0);
    // printf("\n\nExercitiul 3\n\nMy login POST request:\n %s \n", message);
    // send_to_server(sockfd, message);
    // response = receive_from_server(sockfd);
    // printf("Response message:\n%s\n", response);
    // // Ex 3: GET weather key from main server
    // char **cookies = (char **) malloc(sizeof(char *));
    // cookies[0] = (char *) calloc(300, sizeof(char));
    // strcpy(cookies[0], "connect.sid=s%3ANmXboqaehq0OdklfxPp_L6zqkh0i9ZVK.cyeL5qS1Lvv70xQ0MDKs4I7UI3ovpftfPGjyf50YcUQ");
    // message = compute_get_request(url, "/api/v1/weather/key", NULL, cookies, 1);
    // printf("\n\nExercitiul 4\n\nMy login GET request:\n%s\n", message);
    // send_to_server(sockfd, message);
    // response = receive_from_server(sockfd);
    // printf("Response message:\n%s\n", response);
    // // Ex 4: GET weather data from OpenWeather API
    // // Ex 5: POST weather data for verification to main server
    // // Ex 6: Logout from main server
    // message = compute_get_request(url, "/api/v1/auth/logout", NULL, NULL, 0);
    // printf("\n\nExercitiul 5\n\nMy logout GET request:\n%s\n", message);
    // send_to_server(sockfd, message);
    // response = receive_from_server(sockfd);
    // printf("Response message:\n%s\n", response);

    // BONUS: make the main server return "Already logged in!"

    // free the allocated data at the end!
    //free(message);
    close_connection(sockfd);
    free(command);
    return 0;
}

void register_client() {
	char *message;
	char *response;
	char **body_data = (char **) malloc(sizeof(char *));
	body_data[0] = (char *) calloc(600, sizeof(char));
	char *uname = (char *) calloc(256, sizeof(char));
	char *pass = (char *) calloc(256, sizeof(char));
	printf("username=");
	scanf("%s", uname);
	printf("password=");
	scanf("%s", pass);
   	sprintf(body_data[0], "{ \"username\": \"%s\", \"password\": \"%s\" }",
   		uname, pass);
	message = compute_post_request(url, REGISTER_PATH, APP_JSON, body_data, 1,
		NULL, NULL, 0);
    printf("\nMy register POST request:\n%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
   	printf("Response message:\n%s\n", response);
   	free(uname);
   	free(pass);
   	free(message);
   	free(response);
   	free(body_data[0]);
   	free(body_data);
}

void login() {
	char *message;
	char *response;
	char *aux;
	char *cookie;
	char **body_data = (char **) malloc(sizeof(char *));
	body_data[0] = (char *) calloc(600, sizeof(char));
	char *uname = (char *) calloc(256, sizeof(char));
	char *pass = (char *) calloc(256, sizeof(char));
	// cere username si parola
	printf("username=");
	scanf("%s", uname);
	printf("password=");
	scanf("%s", pass);
	// creeaza payload-ul JSON
   	sprintf(body_data[0], "{ \"username\": \"%s\", \"password\": \"%s\" }",
   		uname, pass);
   	// creeaza mesajul de tip POST
	message = compute_post_request(url, LOGIN_PATH, APP_JSON, body_data, 1,
		NULL, NULL, 0);
    printf("\nMy register POST request:\n%s\n", message);
    // trimite mesajul la server
    send_to_server(sockfd, message);
    // primeste raspuns de la server
    response = receive_from_server(sockfd);
   	printf("Response message:\n%s\n", response);
   	// TODO IN CAZ DE EROARE
   	// caut "Set-Cookie"
   	aux = response;
   	while (strncmp(aux, "Set-Cookie", 10)) {
   		aux++;
   	}
   	// ajung cu pointerul la inceputul cookie-ului
   	aux += 12;
   	cookie = strtok(aux, ";");
   	// copiaza cookie-ul in variabila globala
   	strcpy(login_cookie, cookie);
   	// elibereaza memoria alocata
   	free(uname);
   	free(pass);
   	free(message);
   	free(response);
   	free(body_data[0]);
   	free(body_data);
}

void get_access() {
	char *message;
	char *response;
	char *token;
	char *aux;
	char **cookies = (char **) malloc(sizeof(char *));
	cookies[0] = (char *) calloc(200, sizeof(char));
	// strcpy(cookies[0], login_cookie);
	strcpy(cookies[0], "connect.sid=s%3AZ3Ksh2_CxpD66lmrIB7FN-apBFKuBF36.EttV3R4%2BK4ucbovMVvhNIeVYOY5WtG%2FAjj7O0hWUSXQ");
	message = compute_get_request(url, ACCESS_PATH, NULL, NULL, cookies, 1);
    printf("\nMy register POST request:\n%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // TODO TRATEAZA EROAREA
   	printf("Response message:\n%s\n", response);
   	aux = response;
   	while(strncmp(aux, "token", 5)) {
   		aux++;
   		// printf("%s\n", response);
   	}
   	aux += 8;
   	token = strtok(aux, "}");
   	strncpy(jwt_token, token, strlen(token) - 1);
   	printf("token=%s\n", jwt_token);
   	free(message);
   	free(response);
   	free(cookies[0]);
   	free(cookies);
}

void get_books() {
	char *message;
	char *response;
	char *aux;
	strcpy(jwt_token, "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VyX2lkIjoxOTI5NDIsImlhdCI6MTU4OTIwNDE4NywiZXhwIjoxNTg5MjA3Nzg3LCJhdWQiOiJzdHVkZW50aSIsImlzcyI6IlByb3RvY29hbGUgZGUgQ29tdW5pY2F0aWUiLCJzdWIiOiJUb2tlbiBwZW50cnUgdGVtYSBkZSBwcm90b2NvYWxlIn0.ehk85USGmiYc_59SiTTpnV0mc0tuf1pCw16H966_ks8");
	char **cookies = (char **) malloc(sizeof(char *));
	cookies[0] = (char *) calloc(200, sizeof(char));
	// strcpy(cookies[0], login_cookie);
	strcpy(cookies[0], "connect.sid=s%3AZ3Ksh2_CxpD66lmrIB7FN-apBFKuBF36.EttV3R4%2BK4ucbovMVvhNIeVYOY5WtG%2FAjj7O0hWUSXQ");
	message = compute_get_request(url, BOOKS_PATH, NULL, jwt_token, cookies, 1);
    printf("\nMy register POST request:\n%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // TODO TRATEAZA EROAREA
   	printf("Response message:\n%s\n", response);
   	aux = response + strlen(response) - 1;
   	while (strncmp(aux, "[", 1)) {
   		aux--;
   	}
   	printf("%s\n", aux);
   	free(message);
   	free(response);
   	free(cookies[0]);
   	free(cookies);
}

void get_book() {
	char *message;
	char *response;
	char *path = (char *) calloc(200, sizeof(char));
	char *aux;
	char id[10];
	printf("id=");
	scanf("%s", id);
	strcpy(path, BOOKS_PATH);
	strcat(path, "/");
	strcat(path, id);
	strcpy(jwt_token, "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VyX2lkIjoxOTI5NDIsImlhdCI6MTU4OTIwNDE4NywiZXhwIjoxNTg5MjA3Nzg3LCJhdWQiOiJzdHVkZW50aSIsImlzcyI6IlByb3RvY29hbGUgZGUgQ29tdW5pY2F0aWUiLCJzdWIiOiJUb2tlbiBwZW50cnUgdGVtYSBkZSBwcm90b2NvYWxlIn0.ehk85USGmiYc_59SiTTpnV0mc0tuf1pCw16H966_ks8");
	char **cookies = (char **) malloc(sizeof(char *));
	cookies[0] = (char *) calloc(200, sizeof(char));
	// strcpy(cookies[0], login_cookie);
	strcpy(cookies[0], "connect.sid=s%3AZ3Ksh2_CxpD66lmrIB7FN-apBFKuBF36.EttV3R4%2BK4ucbovMVvhNIeVYOY5WtG%2FAjj7O0hWUSXQ");
	message = compute_get_request(url, path, NULL, jwt_token, cookies, 1);
    printf("\nMy register POST request:\n%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // TODO TRATEAZA EROAREA
   	printf("Response message:\n%s\n", response);
   	aux = response + strlen(response) - 1;
   	while (strncmp(aux, "[", 1)) {
   		aux--;
   	}
   	printf("%s\n", aux);
   	free(message);
   	free(response);
   	free(cookies[0]);
   	free(cookies);
}

void add_book() {
	char *message;
	char *response;
	char **body_data = (char **) malloc(sizeof(char *));
	body_data[0] = (char *) calloc(600, sizeof(char));
	char type[10] = "POST";
	char title[100], author[100], genre[100], publisher[100], pages[10];
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
	sprintf(body_data[0], "{ \"title\": \"%s\", \"author\": \"%s\", \"genre\": \"%s\", \"page_count\": \"%s\", \"publisher\": \"%s\" }",
   		title, author, genre, pages, publisher);
	strcpy(jwt_token, "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VyX2lkIjoxOTI5NDIsImlhdCI6MTU4OTIwNDE4NywiZXhwIjoxNTg5MjA3Nzg3LCJhdWQiOiJzdHVkZW50aSIsImlzcyI6IlByb3RvY29hbGUgZGUgQ29tdW5pY2F0aWUiLCJzdWIiOiJUb2tlbiBwZW50cnUgdGVtYSBkZSBwcm90b2NvYWxlIn0.ehk85USGmiYc_59SiTTpnV0mc0tuf1pCw16H966_ks8");
	char **cookies = (char **) malloc(sizeof(char *));
	cookies[0] = (char *) calloc(200, sizeof(char));
	// strcpy(cookies[0], login_cookie);
	strcpy(cookies[0], "connect.sid=s%3AZ3Ksh2_CxpD66lmrIB7FN-apBFKuBF36.EttV3R4%2BK4ucbovMVvhNIeVYOY5WtG%2FAjj7O0hWUSXQ");
	// message = compute_post_request(url, BOOKS_PATH, APP_JSON, body_data, 1,
	// 	jwt_token, cookies, 1);
	message = compute_request(type, url, BOOKS_PATH, APP_JSON, body_data, 1,
		jwt_token, cookies, 1);
    printf("\nMy register POST request:\n%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // TODO TRATEAZA EROAREA
   	printf("Response message:\n%s\n", response);
   	free(message);
   	free(response);
   	free(cookies[0]);
   	free(cookies);
   	free(body_data[0]);
   	free(body_data);
}

void delete_book() {
	char *message;
	char *response;
	char *path = (char *) calloc(200, sizeof(char));
	char type[10] = "DELETE";
	char id[10];
	printf("id=");
	scanf("%s", id);
	strcpy(path, BOOKS_PATH);
	strcat(path, "/");
	strcat(path, id);
	strcpy(jwt_token, "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VyX2lkIjoxOTI5NDIsImlhdCI6MTU4OTIwNDE4NywiZXhwIjoxNTg5MjA3Nzg3LCJhdWQiOiJzdHVkZW50aSIsImlzcyI6IlByb3RvY29hbGUgZGUgQ29tdW5pY2F0aWUiLCJzdWIiOiJUb2tlbiBwZW50cnUgdGVtYSBkZSBwcm90b2NvYWxlIn0.ehk85USGmiYc_59SiTTpnV0mc0tuf1pCw16H966_ks8");
	char **cookies = (char **) malloc(sizeof(char *));
	cookies[0] = (char *) calloc(200, sizeof(char));
	// strcpy(cookies[0], login_cookie);
	strcpy(cookies[0], "connect.sid=s%3AZ3Ksh2_CxpD66lmrIB7FN-apBFKuBF36.EttV3R4%2BK4ucbovMVvhNIeVYOY5WtG%2FAjj7O0hWUSXQ");
	message = compute_request(type, url, path, NULL, NULL, 0, jwt_token, cookies, 1);
    printf("\nMy register POST request:\n%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // TODO TRATEAZA EROAREA
   	printf("Response message:\n%s\n", response);
   	free(message);
   	free(response);
   	free(cookies[0]);
   	free(cookies);
}

void logout() {
	char *message;
	char *response;
	char type[4] = "GET";
	strcpy(jwt_token, "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VyX2lkIjoxOTI5NDIsImlhdCI6MTU4OTIwNDE4NywiZXhwIjoxNTg5MjA3Nzg3LCJhdWQiOiJzdHVkZW50aSIsImlzcyI6IlByb3RvY29hbGUgZGUgQ29tdW5pY2F0aWUiLCJzdWIiOiJUb2tlbiBwZW50cnUgdGVtYSBkZSBwcm90b2NvYWxlIn0.ehk85USGmiYc_59SiTTpnV0mc0tuf1pCw16H966_ks8");
	char **cookies = (char **) malloc(sizeof(char *));
	cookies[0] = (char *) calloc(200, sizeof(char));
	// strcpy(cookies[0], login_cookie);
	strcpy(cookies[0], "connect.sid=s%3AZ3Ksh2_CxpD66lmrIB7FN-apBFKuBF36.EttV3R4%2BK4ucbovMVvhNIeVYOY5WtG%2FAjj7O0hWUSXQ");
	message = compute_request(type, url, LOGOUT_PATH, NULL, NULL, 0, NULL, cookies, 1);
    printf("\nMy register POST request:\n%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // TODO TRATEAZA EROAREA
   	printf("Response message:\n%s\n", response);
   	free(message);
   	free(response);
   	free(cookies[0]);
   	free(cookies);
}
