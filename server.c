#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "base64.c"

#define PORT 25000 // You can change port number here

int respond(int sock);

char* usrname = "username";
char* password = "password";
#include <stdint.h>
#include <stdlib.h>
//Problem 1 of project 1:simple webserver with authentification
//Both Problem 1 and 2 was tested on WSL enviroments, Linux, and M1 mac
//But If you still have problems on running codes please mail us
//Most importantly please comment your code

//If you are using mac 
//You can install homebrew here :https://brew.sh
//And open terminal and type 
//sudo brew install gcc
//sudo brew install make
//Type make command to build server
//And server binary will be created
//Use ifconfig command to figure out your ip(usually start with 192. or 172.)
//run server with ./server and open browser and type 192.x.x.x:25000



//If you are using Linux or WSL
//You just need to run "make"(If you are using WSL you may need to install gcc and make with apt)
//And server binary will be created
//Use ifconfig command to figure out your ip(usually start with 192. or 172.)
//run server with ./server and open browser and type 192.x.x.x:25000


//It will be better if you run virtual machine or other device to run server
//But you can also test server with opening terminal and run it on local IP 


int main(int argc, char* argv[]) {
	int sockfd, newsockfd, portno = PORT;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	clilen = sizeof(cli_addr);

	printf("encoding start \n");// We have implemented base64 encoding you just need to use this function
	char* token = base64_encode(password, strlen(password));
	printf("encoding end \n");

	//browser will repond with base64 encoded "userid:password" string 
	//You should parse authentification information from http 401 responese and compare it


	/* First call to socket() function */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}

	// port reusable
	int tr = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &tr, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	/* Initialize socket structure */
	bzero((char*)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	/* TODO : Now bind the host address using bind() call. 10% of score*/
	  //it was mostly same as tutorial
	if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
		perror("bind error");
		exit(1);
	}

	/* TODO : listen on socket you created  10% of score*/
	if (listen(sockfd, 10) == -1) { // listen on sockfd allowing 10 clients
		perror("listen error");
		exit(1);
	}
		printf("Server is running on port %d\n", portno);

		//it was mostly same as tutorial
		//in the while loop every time request comes we respond with respond function if valid

		//TODO: authentication loop 40 % of score
		while (1) {
			newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
			if (newsockfd == -1) {
				perror("accept error");
				exit(1);
			}

			//TODO: accept connection
			//TODO: send 401 message(more information about 401 message : https://developer.mozilla.org/en-US/docs/Web/HTTP/Authentication) and authentificate user
			//close connection

			int offset = 0;
			int rbyte = 0;
			int total_rbyte = 0; // total read bytes
			char rdata[9000]; // for receiving request msg
			bzero(rdata, 9000); // initiallizing receiving data array
			char msg401[] = "HTTP/1.1 401 Unauthorized\r\nWWW-Authenticate: Basic\r\n"; // 401 msg header
			int msg401_len = strlen(msg401); // length of 401 msg
			int sbyte = 0; // byte of send msg
			do {
				rbyte = recv(newsockfd, rdata + offset, 1500, 0);
				offset += rbyte; // increment offset which is pointing start point of receiving in rdata as much as received bytes 
				total_rbyte = total_rbyte + rbyte;
				if (strncmp(rdata + offset - 4, "\r\n\r\n", 4) == 0) { // if detect \r\n\r\n, break receiving loop so that cut at msg header
					break;
				}
			} while (rbyte > 0); // receive data until received byte becomes 0

			if (total_rbyte < 0) { // receive error
				printf("receive error\n");
				close(newsockfd);
				continue;
			}
			else if (total_rbyte == 0) { // case client is diconnected
				printf("Client is disconnected\n");
				close(newsockfd);
				continue;
			}

			rdata[offset] = 0; // extract only msg header
			printf("%s\n", rdata); // print received msg

			while (msg401_len > 0) { // send 401 msg
				sbyte = send(newsockfd, msg401, msg401_len, 0);
				msg401_len = msg401_len - sbyte;
			}
			close(newsockfd); // after sending 401 msg, close socket

			total_rbyte = 0; // initialize total read bytes
			offset = 0;
			rbyte = 0;
			char rdata2[9000];
			bzero(rdata2, 9000);

			newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen); // accept socket newly
			if (newsockfd == -1) {
				perror("accept error");
				exit(1);
			}

			do {
				rbyte = recv(newsockfd, rdata2 + offset, 1500, 0);
				offset += rbyte; // increment offset which is pointing start point of receiving in rdata as much as received bytes 
				total_rbyte = total_rbyte + rbyte;

				if (strncmp(rdata2 + offset - 4, "\r\n\r\n", 4) == 0) { // if detect \r\n\r\n, break receiving loop so that cut at msg header
					break;
				}
			} while (rbyte > 0); // receive data until received byte becomes 0
				
			if (total_rbyte < 0) {
				printf("receive error\n");
				close(newsockfd);
				continue;
			}
			else if (total_rbyte == 0) {
				printf("Client is disconnected\n");
				close(newsockfd);
				continue;
			}

			rdata2[offset] = 0; // extract only msg header
			printf("%s\n", rdata2); // print msg header

			char* idx = strstr(rdata2, "Basic"); // to detect Basic comment in msg header of response for 401 msg
			idx = idx + 6; // shift pointer to first position of key
			int i = 0;
			while (1) {
				i++; // i is counting length of key
				idx = idx + 1;
				if (strncmp(idx, "\r", 1) == 0) { // detecting \r means that parsing key is done to the end completely
					break;
				}
			}
			idx = idx - i; // shift back to first position of key
			char* key = malloc(i);
			strncat(key, idx, i); // extract key
			
			if (strcmp(key, token) == 0) { // if key equals token, close socket and break authentication loop
				free(key);
				close(newsockfd);
				break;
			}
			else { // if key is different with token, close socket and back to authentication loop again
				free(key);
				close(newsockfd);
			}

		}

		//Respond loop
		while (1) {
			newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
			if (newsockfd == -1) {
				perror("accept error");
				exit(1);
			}
			printf("test\n");
			respond(newsockfd);
		}

	return 0;
}
//TODO: complete respond function 40% of score
int respond(int sock) {
	char htmlhd[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html;\r\n\r\n";
	char pnghd[] = "HTTP/1.1 200 OK\r\nContent-Type: image/png;\r\n\r\n";
	char csshd[] = "HTTP/1.1 200 OK\r\nContent-Type: text/css;\r\n\r\n";
	char jshd[] = "HTTP/1.1 200 OK\r\nContent-Type: text/javascript;\r\n\r\n";
	int offset, rbyte;
	int total_rbyte = 0; // total read bytes
	char* req_fp; // first point of request file
	char* req_lp; // last point of request file
	char rdata[9000];
	char* request;
	char* msghd = NULL; // msg header
	int hdlen = 0; // length of header
	bzero(rdata, 9000);

	offset = 0;
	rbyte = 0;

	do {
		rbyte = recv(sock, rdata + offset, 1500, 0);
		offset += rbyte; // increment offset which is pointing start point of receiving in rdata as much as received bytes 
		total_rbyte = total_rbyte + rbyte;

		if (strncmp(rdata + offset - 4, "\r\n\r\n", 4) == 0) { // if detect \r\n\r\n, break receiving loop so that cut at msg header
			break;
		}
	} while (rbyte > 0); // receive data until received byte becomes 0

	if (total_rbyte < 0) {
		printf("receive error\n");
		close(sock);
		return -1;
	}
	else if (total_rbyte == 0) {
		printf("Client is disconnected\n");
		close(sock);
		return 0;
	}

	rdata[offset] = 0;
	printf("%s\n", rdata);

	req_fp = strstr(rdata, "GET"); // detect GET commend
	req_fp = req_fp + 5; // shift pointer to first position of the requested file name
	req_lp = req_fp;

	while (strncmp(req_lp, " ", 1) != 0) { // to extract the requested file, shift req_lp until detecting space
		req_lp = req_lp + 1;
	}
	strcpy(req_lp, "\0"); // to extract only requested file name
	request = req_fp;
	printf("request: %s\n", request); // print requested file name

	char cd[100]; // for save current path
	char fbuff[500000]; // to save requested file
	bzero(fbuff, 500000);
	char msg404[] = "HTTP/1.1 404 Not Found\r\n"; // 404 msg
	int msg404_len = strlen(msg404);
	int fsize = 0;
	int rbytes = 0; // read bytes with requested file
	getcwd(cd, 100); // get current path to cd
	strcat(cd, "/"); // attatch "/" to cd
	strcat(cd, request); // attatch requested file name to cd so that get path of requested file

	FILE* fs = NULL;

	if (strstr(request, ".html") == NULL & strstr(request, ".png") == NULL & strstr(request, ".css") == NULL & strstr(request, ".js") == NULL ) { // only respond to request for html, png, css and js files
		send(sock, msg404, msg404_len, 0);
		printf("sent 404 msg\n");
		close(sock);
		return 0;
	}

	if (strstr(request, ".html") != NULL) { // if requested file is html file
		
		fs = fopen(cd, "r");
		
		if (fs == NULL) { // if there's no requested file, send 404 msg
			send(sock, msg404, msg404_len, 0);
			printf("sent 404 msg\n");
		}

		fseek(fs, 0, SEEK_END);
		fsize = ftell(fs) + 1; // get requested file size
		fseek(fs, 0, SEEK_SET);

		msghd = htmlhd; // assign header of response with html file to msghd
		rbytes = fread(fbuff, 1, fsize, fs); // read requested file
	}
	
	if (strstr(request, ".png") != NULL) { // if requested file is png file

		fs = fopen(cd, "rb"); // open file rb mode

		if (fs == NULL) {
			send(sock, msg404, msg404_len, 0);
			printf("sent 404 msg\n");
		}

		fseek(fs, 0, SEEK_END);
		fsize = ftell(fs) + 1;
		fseek(fs, 0, SEEK_SET);

		msghd = pnghd; // assign header of response with png file to msghd

		rbytes = fread(fbuff, 1, fsize, fs);
	}

	if (strstr(request, ".css") != NULL) { // if requested file is css file
		
		fs = fopen(cd, "r");

		if (fs == NULL) {
			send(sock, msg404, msg404_len, 0);
			printf("sent 404 msg\n");
		}

		fseek(fs, 0, SEEK_END);
		fsize = ftell(fs) + 1;
		fseek(fs, 0, SEEK_SET);

		msghd = csshd; // assign header of response with css file to msghd
		rbytes = fread(fbuff, 1, fsize, fs);
	}

	if (strstr(request, ".js") != NULL) { // if requested file is js file
		fs = fopen(cd, "r");

		if (fs == NULL) {
			send(sock, msg404, msg404_len, 0);
			printf("sent 404 msg\n");
		}

		fseek(fs, 0, SEEK_END);
		fsize = ftell(fs) + 1;
		fseek(fs, 0, SEEK_SET);

		msghd = jshd; // assign header of response with js file to msghd
		rbytes = fread(fbuff, 1, fsize, fs);
	}

	hdlen = strlen(msghd);

	int sbyte = 0;

	while (hdlen > 0) {
		sbyte = send(sock, msghd, hdlen, 0); // send header of response msg for requested file
		printf("send byte: %d\n", sbyte);
		hdlen = hdlen - sbyte;
	}
	
	sbyte = 0;
	
	while (rbytes > 0) {
		sbyte = send(sock, fbuff, rbytes, 0); // send payload of response msg for requested file
		printf("send byte: %d\n", sbyte);
		rbytes = rbytes - sbyte;
	}
	
	fclose(fs); // close file
	printf("close\n");
	shutdown(sock, SHUT_RDWR);
	close(sock);

	return 0;
}

