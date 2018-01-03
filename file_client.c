/*
 * file_client.c
 *
 *  Created on: Nov 12, 2017
 *      Author: root
 */
#include <netdb.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "constants.h"
/*
 *

#define DEFPORT 1337
#define INPUTLENGTH 25 //pass,ID
#define MAXIP 100
#define MAXPORT 20
#define MAXLINE 500
#define MAXPATH 500
#define MAXFILENAME 100
#define MAXFILESIZE 512
#define CONSTINPUT 10
#define USERPASSLINE (CONSTINPUT + INPUTLENGTH)
*/
int recvAll(int client_fd, void * store , size_t  strlen, int flag){
	int sumGot = 0, sumNeeded = strlen, retCode = 0,stillNeed = sumNeeded;
	while(sumGot < sumNeeded){
		retCode = recv(client_fd, store + sumGot, stillNeed, flag);
		if(retCode < 0){
			printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
			printf("Bytes involved: %d\n",retCode);
			return -1;
		}
		sumGot += retCode;
		stillNeed -= retCode;
	}
	return 1;
}

int sendAll(int client_fd, const void * store , size_t  strlen, int flag){
	int sumGot = 0, sumNeeded = strlen, retCode = 0,stillNeed = sumNeeded;
	while(sumGot < sumNeeded){
		retCode = send(client_fd, store + sumGot, stillNeed, flag);
		if(retCode < 0){
			printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
			printf("Bytes involved: %d\n",retCode);
			return -1;
		}
		sumGot += retCode;
		stillNeed -= retCode;
	}
	return 1;
}

int main(int argc, char* argv []){
	char* hostname = "localhost";//nul..
	char func [MAXPATH];
	int wrote = 0;
	char funcname [MAXLINE], filename[MAXFILENAME], filepath[MAXPATH];
	char user [INPUTLENGTH], pass[INPUTLENGTH], fileContent[MAXFILESIZE], tmpUser[USERPASSLINE], tmpPassword[USERPASSLINE];
	int port = DEFPORT, code = -1, op = -1, len = 0, retCode = -1, howManyFiles = -1, success = -1, fd = -1, howMany = 0;
	struct addrinfo hints, *res;

	if(argc == 2){
		hostname = argv[1];
	}
	if(argc == 3){
		hostname = argv[1];
		port = atoi(argv[2]);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	char portNum[MAXPORT];
	retCode = sprintf(portNum,"%d",port);
	if(retCode < 0){
		printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
		return -1;
	}
	retCode = getaddrinfo(hostname, portNum, &hints, &res);
	if(retCode != 0){
		printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
		return -1;
	}
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock < 0){
		printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
		return -1;
	}
	retCode= connect(sock, res->ai_addr, res->ai_addrlen);
	if(retCode < 0){
		printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
		return -1;
	}
	//printf("Connected\n");
	retCode = recvAll(sock, &op, sizeof(op),0);
	if(retCode < 0){
		printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
		return -1;
	}
	//printf("Connected and op: %d \n", op);
	if(op == 1){
		printf("Welcome! Please log in.\n");
	}
	while(1){
		// comment this For testing only!

		/*
		retCode = scanf("User: %s ", &user);
		printf("retCode after user :%d user: %s\n",retCode,user);
		if(retCode <= 0){
			printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
			return -1;
		}
		retCode = scanf("Password: %s ", &pass);
		printf("retCode after pass :%d, pass: %s\n",retCode,pass);
		if(retCode <= 0){
			printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
			return -1;
		}
		 */
		memset(tmpUser, 0, sizeof(tmpUser));
		memset(tmpPassword, 0, sizeof(tmpPassword));
		fgets(tmpUser,sizeof(tmpUser),stdin);
		if(strcmp(strtok(tmpUser, " "), "User:") == 0){
			memset(user, 0, sizeof(user));
			strcpy(user, strtok(NULL, "\n"));
		}
		else{
			printf("INCORRECT FORMAT DETECTED, TRY AGAIN! \n");
			continue;
		}
		//printf("user: %s\n",user);
		fgets(tmpPassword,sizeof(tmpPassword),stdin);
		if(strcmp(strtok(tmpPassword, " "), "Password:") == 0){
			memset(pass, 0, sizeof(pass));
			strcpy(pass, strtok(NULL, "\n"));
		}
		else{
			printf("INCORRECT FORMAT DETECTED, TRY AGAIN! , %s \n" , pass);
			continue;
		}
		len = strlen(user);
		//printf("INFO I GOT : USER: %s, PASS: %s\n", user,pass);
		retCode = sendAll(sock, &len,sizeof(int),0);
		if(retCode < 0){
			printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
			printf("Bytes involved: %d\n",retCode);
			return -1;
		}

		retCode = sendAll(sock, user ,len,0);
		if(retCode < 0){
			printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
			printf("Bytes involved: %d\n",retCode);
			return -1;
		}
		len = strlen(pass);
		//printf("LEN BEFORE SEND: %d\n",len);
		retCode = sendAll(sock, &len,sizeof(int),0);
		if(retCode < 0){
			printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
			printf("Bytes involved: %d\n",retCode);
			return -1;
		}
		retCode = sendAll(sock, pass ,len,0);
		if(retCode < 0){
			printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
			printf("Bytes involved: %d\n",retCode);
			return -1;
		}
		//printf("I SENT USER AND PASS!\n");
		retCode = recvAll(sock, &op, sizeof(int),0);
		if(retCode < 0){
			printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
			printf("Bytes involved: %d\n",retCode);
			return -1;
		}
		//printf("op got: %d\n", op);
		if(op == 1){
			break;
		}
		else{
			printf("Wrong details, please log in again!\n");
		}
	}
	retCode = recvAll(sock, &len, sizeof(int),0);
	if(retCode < 0){
		printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
		printf("Bytes involved: %d\n",retCode);
		return -1;
	}

	printf("Hi %s, you have %d files stored.\n",user,len);

	/*
	 *
	 * start asking for functions
	 */
	while(1){
		len = sizeof(funcname);
		//printf("*********NEW LOOP ******** \n");
		//printf("I am NOT past get line\n");
		//while ((c = getchar()) != '\n' && c != EOF) { }
		fgets(funcname,len,stdin);
		funcname[strlen(funcname) - 1] = 0;
		//printf("i got the line: %s\n", funcname);
		/*
		retCode = sendAll(sock, pass ,len,0);
		if(retCode < 0){
			printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
			printf("Bytes involved: %d\n",retCode);
			return -1;
		}
		 */

		strcpy(func , strtok(funcname," "));
		//printf("func parameter has: %s\n", func);
		if(strcmp(func,"list_of_files") == 0){
			//printf("***ENTER OP0***\n");
			op = 0;
			retCode = sendAll(sock, &op ,sizeof(op),0);
			if(retCode < 0){
				printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
				printf("Bytes involved: %d\n",retCode);
				return -1;
			}
			retCode = recvAll(sock, &howManyFiles, sizeof(howManyFiles),0);
			if(retCode < 0){
				printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
				printf("Bytes involved: %d\n",retCode);
				return -1;
			}
			for(int i=0; i<howManyFiles; i++){
				retCode = recvAll(sock, &len, sizeof(len),0);
				if(retCode < 0){
					printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
					printf("Bytes involved: %d\n",retCode);
					return -1;
				}
				char tmp [MAXFILENAME];
				memset(tmp, 0, sizeof(tmp));
				retCode = recvAll(sock, tmp, len, 0);
				if(retCode < 0){
					printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
					printf("Bytes involved: %d\n",retCode);
					return -1;
				}
				printf("%s\n", tmp);
			}
			continue;
		}

		else if(strcmp(func,"delete_file")== 0){
			op = 1;
			retCode = sendAll(sock, &op ,sizeof(op),0);
			if(retCode < 0){
				printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
				printf("Bytes involved: %d\n",retCode);
				return -1;
			}
			strcpy(func , strtok(NULL," ")); //has now the file name
			//printf("func contains: %s******\n", func);
			len = strlen(func);
			retCode = sendAll(sock, &len ,sizeof(len),0);
			if(retCode < 0){
				printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
				printf("Bytes involved: %d\n",retCode);
				return -1;
			}
			retCode = sendAll(sock, func, len, 0);
			if(retCode < 0){
				printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
				printf("Bytes involved: %d\n",retCode);
				return -1;
			}

			retCode = recvAll(sock, &success, sizeof(success), 0); //-1 if problem else 1
			if(retCode < 0){
				printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
				printf("Bytes involved: %d\n",retCode);
				return -1;
			}
			if(success > 0){
				printf("File removed\n");
			}
			else{
				printf("No such file exists!\n");
			}
			continue;

		}
		else if(strcmp(func,"add_file")== 0){
			//printf("***ENTER OP2***\n");
			op = 2;
			retCode = sendAll(sock, &op ,sizeof(op),0);
			if(retCode < 0){
				printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
				printf("Bytes involved: %d\n",retCode);
				return -1;
			}
			strcpy(filepath, strtok(NULL," "));
			len = strlen(func);
			strcpy(filename ,strtok(NULL," "));
			//sprintf(filepath,"%s\/%s", filepath, filename);
			//printf("paramaters: file path %s ***** file name %s\n",filepath,filename);
			fd = open(filepath, O_RDONLY );
			if(fd < 0){
				printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
				return -1;
			}
			off_t filesize = lseek(fd, 0 , SEEK_END);
			lseek(fd, 0, SEEK_SET);
			howMany = 0;
			while(howMany < filesize){
				retCode = read(fd, fileContent + howMany, (filesize - howMany));
				if(retCode < 0){
					printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
					printf("Bytes involved: %d\n",retCode);
					return -1;
				}
				howMany += retCode;
			}
			len = strlen(filename);
			//printf("len of %s: %d\n", filename,len);
			retCode = sendAll(sock, &len ,sizeof(len),0);
			if(retCode < 0){
				printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
				printf("Bytes involved: %d\n",retCode);
				return -1;
			}
			retCode = sendAll(sock, filename, len, 0);
			if(retCode < 0){
				printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
				printf("Bytes involved: %d\n",retCode);
				return -1;
			}
			//printf("I SENT: %d %s\n", len, filename);
			len = filesize;
			retCode = sendAll(sock, &len ,sizeof(len),0);
			if(retCode < 0){
				printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
				printf("Bytes involved: %d\n",retCode);
				return -1;
			}
			retCode = sendAll(sock, fileContent, len, 0);
			if(retCode < 0){
				printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
				printf("Bytes involved: %d\n",retCode);
				return -1;
			}
			retCode = recvAll(sock, &success, sizeof(success), 0);
			if(retCode < 0){
				printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
				printf("Bytes involved: %d\n",retCode);
				return -1;
			}
			close(fd);
			if(success == 1){
				printf("File added\n");
			}
			else{
				printf("File already exists on the server, or the max has been reached.\n");
			}
			continue;
		}
		else if(strcmp(func,"get_file")== 0){
			op = 3;
			memset(fileContent ,0 ,sizeof(fileContent) );
			retCode = sendAll(sock, &op ,sizeof(op),0);
			if(retCode < 0){
				printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
				printf("Bytes involved: %d\n",retCode);
				return -1;
			}
			strcpy(filename , strtok(NULL," "));
			len = strlen(filename);
			strcpy(filepath , strtok(NULL," "));
			retCode = sendAll(sock, &len ,sizeof(len),0);
			if(retCode < 0){
				printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
				printf("Bytes involved: %d\n",retCode);
				return -1;
			}
			retCode = sendAll(sock, filename, len, 0);
			if(retCode < 0){
				printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
				printf("Bytes involved: %d\n",retCode);
				return -1;
			}

			retCode = recvAll(sock, &len, sizeof(len), 0);
			if(retCode < 0){
				printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
				printf("Bytes involved: %d\n",retCode);
				return -1;
			}
			if(len == -1){
				// the file does not exists
				printf("File does not exists on the server. Try again!\n");
				continue;
			}
			memset(fileContent, 0, sizeof(fileContent));
			retCode = recvAll(sock, fileContent, len, 0);
			if(retCode < 0){
				printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
				printf("Bytes involved: %d\n",retCode);
				return -1;
			}

			strcat(filepath, filename);
			fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC);
			if(fd < 0){
				printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
				return -1;
			}

			wrote = 0;
			while(wrote < len){
				retCode = write(fd, fileContent, len - wrote);
				if(retCode <= 0){
					printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
					printf("Bytes involved: %d\n",retCode);
					return -1;
				}
				wrote += retCode;
			}
			continue;
		}
		else if(strcmp(func,"quit")== 0){
			op = 4;
			retCode = sendAll(sock, &op ,sizeof(op),0);
			if(retCode < 0){
				printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
				printf("Bytes involved: %d\n",retCode);
				return -1;

			}
			close(sock);
			code = -2;
		}
		else{
			printf("This function is not supported. Try again!\n");
		}
		if(code == -2){
			freeaddrinfo(res);
			break;
		}
	}

}





