/*
 * file_server.c
 *
 *  Created on: Nov 12, 2017
 *      Author: root
 */

#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdlib.h>
#include <errno.h>

/* ~~~~Application protocol~~~~
 *
 *Before every message sent by both sides, they send the length
 *of it so the other side knows how many bytes to read into his buffer.
 *
 * The functions are labeled 0 to 4:
 * 0. list_of_files
 * 1. delete_file
 * 2. add_file
 * 3. get_file
 * 4. quit
 *
 * The client reads the required function and send a code to the
 * server represeting the function.
 *
 * The server sends 1 for success and -1 for failure
 * if it is required.
 *
 *
 */
/*
#define MAXAMOUNT 15 //How many files can be stored
#define MAXUSERS 15 //How many users can be in the users file
#define MAXFILENAME 100 //Maximum length of the file name
#define INPUTLENGTH 25 //Maximum length of pass,ID
#define MAXFILESIZE 512 //Maximum file size in BYTES
#define DEFPORT 1337 // The default port
#define MAXPATH 500 // The maximum length of a path to file
*/
#include "constants.h"


typedef struct UserFile UserFile;
typedef struct User User;

struct UserFile{
	char	fileName [100];
	int		alive; //1 alive -1 dead
} ;

struct User {
	char  name[INPUTLENGTH];
	char password[INPUTLENGTH];
	UserFile files [MAXAMOUNT];
	int   fileNum;
} ;

int recvAll(int client_fd, void * store , size_t  strlen, int flag){
	int sumGot = 0, sumNeeded = strlen, retCode = 0,stillNeed = sumNeeded;
	while(sumGot < sumNeeded){
		retCode = recv(client_fd, store + sumGot, stillNeed, flag);
		if(retCode < 0){
			printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
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
			return -1;
		}
		sumGot += retCode;
		stillNeed -= retCode;
	}
	return 1;
}
int isLegit(char* userChecked, char* passChecked, User* allUsers){
	char user [INPUTLENGTH] , pass [INPUTLENGTH];
	for(int i =0 ; i< MAXUSERS; i++){
		strcpy(user,allUsers[i].name);
		strcpy(pass,allUsers[i].password);
		if(strcmp(user,userChecked) == 0 && strcmp(pass,passChecked) ==0){
			return i;
		}
	}
	return -1;
}

char* getUser(int client_fd, int* strlen) {
	// wait for id and pass...
	int retCode = -1;
	retCode = recvAll(client_fd, strlen, sizeof(*strlen), 0); //TODO Check return
	if(retCode < 0){
		printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
		printf("Bytes involved: %d\n",retCode);
		return NULL;
	}
	char* userName = calloc(sizeof(char),*strlen + 1); //+1 for strcpy
	retCode = recvAll(client_fd, userName, *strlen,0); //TODO Check return
	if(retCode < 0){
		printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
		printf("Bytes involved: %d\n",retCode);
		return NULL;
	}
	return userName;
}

char* getPassword(int client_fd, int* strlen) {
	int retCode = -1;
	retCode = recvAll(client_fd, strlen, sizeof(*strlen), 0); //TODO Check return
	if(retCode < 0){
		printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
		printf("Bytes involved: %d\n",retCode);
		return  NULL;
	}
	char* password = calloc(sizeof(char), *strlen + 1); //+1 for strcpy
	retCode = recvAll(client_fd, password, *strlen,0); //TODO Check return
	if(retCode < 0){
		printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
		printf("Bytes involved: %d\n",retCode);
		return NULL;
	}
	return password;
}

int main(int argc, char* argv []){
	struct sockaddr_in myaddr , client_ad;
	int fd = 0, client_fd = 0; //TODO Check return
	int code = -1, codeDir = -1, port = DEFPORT, len = 0, progress = -1, userNum = 0, curUser = -1, codeop1 =-1, codeop2 = -1;
	int op = -1, retCode = -1, howManyFiles = 0, wrote = 0, howMany = 0;
	int filenum = 0;
	char filename[100], fileContent [MAXFILESIZE];// trouble?
	//printf("Past line: %d\n",__LINE__);
	User allUsers [MAXAMOUNT];
	memset(allUsers ,0 ,MAXAMOUNT * sizeof(*allUsers) );
	if(argc == 4){
		port = atoi(argv[3]); //TODO Check return
	}

	char user [INPUTLENGTH], pass [INPUTLENGTH];
	char* mode = "r";
	fd = open(argv[1], O_RDONLY);
	if(fd < 0){
		printf("Error opening the users file \n");
		return -1;
	}
	FILE* readUser = fdopen(fd, mode);//TODO Check return
	code = fscanf(readUser,"%s\t%s", user,pass ); //TODO Check return
	if(code <= 0){
		printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
		return -1;
	}
	while(code > 0){ //EOF
		char fullPath[MAXPATH];
		sprintf(fullPath,"%s%s", argv[2],user);
		//printf("Past line: %d\n", __LINE__);
		strcpy(allUsers[userNum].name, user);
		//printf("Got a name for you: %s\n",allUsers[userNum].name);
		strcpy(allUsers[userNum].password, pass);
		allUsers[userNum].fileNum = 0;
		for(int i =0; i< MAXAMOUNT; i++){
			allUsers[userNum].files[i].alive = -1;
		}
		codeDir = mkdir(fullPath, S_IRWXU); //mkdir not good yet
		if(codeDir < 0){
			printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
			return -1;
		}
		code = fscanf(readUser,"%s\t%s\n", user,pass ); //TODO Check return
		userNum++;
	}
	fclose(readUser);
	int sock = socket(AF_INET, SOCK_STREAM, 0); //TODO Check return
	memset(&myaddr,0,sizeof (myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons( port );
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//printf("BEFORE BIND \n");
	bind(sock, (const struct sockaddr *) &myaddr, sizeof(myaddr));//TODO Check return
	listen(sock, 5);//TODO Check return
	//printf("listening....\n");
	while(1){
		int adsize=sizeof(client_ad);
		client_fd = accept(sock,(struct sockaddr *) (&client_ad), (socklen_t *) (&adsize)); //TODO Check return
		//printf("SUCCESFULLY CONNECTED.\n");
		int welcomeMsg = 1;
		retCode = sendAll(client_fd, &welcomeMsg, sizeof(welcomeMsg),0); //TODO Check return
		if(retCode < 0){
			printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
			return -1;
		}
		// wait for id and pass...

		char* userName = getUser(client_fd, &len);
		if(userName == NULL){
			printf("Could not read username\n");
			return -1;

		}
		char* password = getPassword(client_fd, &len);
		if(password == NULL){
			printf("Could not read password\n");
			return -1;

		}
		/*
		off_t check = lseek(fd,0, SEEK_SET);
		if(check< 0){
			printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
			return -1;
		}
		*/
		progress = -1;
		//printf("INFO I GOT : USER: %s, PASS: %s\n", userName,password);
		curUser = isLegit(userName,password,allUsers);
		while (curUser  < 0){
			retCode = sendAll(client_fd, &progress, sizeof(progress),0); //TODO Check return //if -1 is sent then the user is not correct
			if(retCode < 0){
				printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
				return -1;
			}
			userName = getUser(client_fd, &len);
			password = getPassword(client_fd, &len);
			curUser = isLegit(userName,password,allUsers);
		}
		//printf("passed isLegit\n");
		progress = 1;
		retCode = sendAll(client_fd, &progress, sizeof(progress),0);//TODO check
		if(retCode < 0){
			printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
			return -1;
		}

		filenum = allUsers[curUser].fileNum; //how many files stored
		retCode = sendAll(client_fd, &filenum, sizeof(filenum),0);//TODO check
		if(retCode < 0){
			printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
			return -1;
		}

		/*
		 *
		 * Start taking requests
		 */
		while(1){
			code = 0;
			//printf("Got into the while loop\n");
			char fullPath[MAXPATH], pathToFile[MAXPATH];
			sprintf(fullPath,"%s%s", argv[2],allUsers[curUser].name);
			//printf("path is : %s\n", fullPath);
			retCode = recvAll(client_fd, &op, sizeof(op), 0);
			//printf("op code: %d\n", op);
			if(retCode < 0){
				printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
				return -1;
			}
			switch(op){
			case 0: //list_of_files
				howManyFiles = allUsers[curUser].fileNum;
				//printf("filenum is : %d\n", allUsers[curUser].fileNum);
				retCode = sendAll(client_fd, &howManyFiles, sizeof(howManyFiles),0);//TODO check
				if(retCode < 0){
					printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
					return -1;
				}
				for(int i=0; i<MAXAMOUNT; i++){
					if(allUsers[curUser].files[i].alive == 1){
						len = strlen(allUsers[curUser].files[i].fileName);
						retCode = sendAll(client_fd, &len, sizeof(len),0);//TODO check
						if(retCode < 0){
							printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
							return -1;
						}
						//printf("I send this : name %s length %d", allUsers[curUser].files[i].fileName,len);
						retCode = sendAll(client_fd, &(allUsers[curUser].files[i].fileName), len,0);
						if(retCode < 0){
							printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
							return -1;
						}
					}
				}
				break;
			case 1: //delete_file
				codeop1 = -1;
				retCode = recvAll(client_fd, &len, sizeof(len), 0);
				if(retCode < 0){
					printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
					return -1;
				}
				memset(filename, 0, sizeof(filename));
				retCode = recvAll(client_fd, filename, len, 0);
				if(retCode < 0){
					printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
					return -1;
				}
				//printf("I GOT : %d %s\n", len, filename);
				/*
				 * search if file exists
				 */
				for(int i =0; i<MAXAMOUNT; i++){
					if(allUsers[curUser].files[i].alive == 1 ){
						if(strcmp(allUsers[curUser].files[i].fileName,filename) ==0 ){
							//printf("I ADMIT DEFEAT \n");
							allUsers[curUser].fileNum--;
							allUsers[curUser].files[i].alive = -1;
							codeop1 = 1;
							sprintf(pathToFile,"%s//%s", fullPath, filename);
							retCode = remove(pathToFile);
							if(retCode != 0){
								printf("Failed to remove\n");
								return -1;
							}
							break;
						}
					}
				}
				retCode = sendAll(client_fd, &codeop1, sizeof(codeop1),0);
				if(retCode < 0){
					printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
					return -1;
				}
				break;

			case 2: //add_file
				codeop2 = 1;
				//printf("***ENTER OP 2***\n");
				retCode = recvAll(client_fd, &len, sizeof(len), 0);
				if(retCode < 0){
					printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
					return -1;
				}

				memset(filename, 0, sizeof(filename)); // TODO if works add in rest!
				retCode = recvAll(client_fd, filename, len, 0);
				if(retCode < 0){
					printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
					return -1;
				}
				//printf("len and filename: %s %d\n", filename,len);


				retCode = recvAll(client_fd, &len, sizeof(len), 0);
				if(retCode < 0){
					printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
					return -1;
				}
				memset(fileContent, 0, sizeof(fileContent));
				retCode = recvAll(client_fd, fileContent, len, 0);
				if(retCode < 0){
					printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
					return -1;
				}

				for(int i=0; i< MAXAMOUNT; i++){
					if(allUsers[curUser].fileNum == 15)
					{
						codeop2 = -1;
						retCode = sendAll(client_fd, &codeop2, sizeof(codeop2),0);
						if(retCode < 0){
							printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
							return -1;
						}
						break;
					}
					else if(allUsers[curUser].files[i].alive == 1){
						if(strcmp(allUsers[curUser].files[i].fileName, filename) == 0){
							codeop2 = -1;
							retCode = sendAll(client_fd, &codeop2, sizeof(codeop2),0);
							if(retCode < 0){
								printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
								return -1;
							}
							
						}
					}
				}
				if(codeop2 == -1){
					break;
				}
				memset(pathToFile, 0 , strlen(pathToFile));
				sprintf(pathToFile,"%s//%s", fullPath, filename);
				//printf("path to file is : %s \n", pathToFile);
				fd = open(pathToFile, O_WRONLY | O_CREAT);
				if(fd < 0){
					printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
					return -1;
				}
				allUsers[curUser].fileNum++;
				for(int i=0; i< MAXAMOUNT; i++){
					if(allUsers[curUser].files[i].alive == -1){
						allUsers[curUser].files[i].alive = 1;
						memset(allUsers[curUser].files[i].fileName, 0, MAXFILENAME);
						strcpy(allUsers[curUser].files[i].fileName, filename);
						//printf("FILENAME IS : %s\n",filename);
						break;
					}
				}

				wrote = 0;
				while(wrote < len){
					retCode = write(fd, fileContent, len - wrote);
					if(retCode <= 0){
						printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
						return -1;
					}
					wrote += retCode;
				}
				retCode = sendAll(client_fd, &codeop2, sizeof(codeop2),0);
				if(retCode < 0){
					printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
					return -1;
				}
				close(fd);
				break;
			case 3: //get_file
				//printf("***ENTER OP 3***\n");
				retCode = recvAll(client_fd, &len, sizeof(len), 0);
				if(retCode < 0){
					printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
					return -1;
				}

				memset(filename, 0, sizeof(filename)); // TODO if works add in rest!
				retCode = recvAll(client_fd, filename, len, 0);
				if(retCode < 0){
					printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
					return -1;
				}
				//printf("len and filename: %s %d\n", filename,len);

				for(int i =0; i<MAXAMOUNT; i++){
					if(allUsers[curUser].files[i].alive == 1 ){
						if(strcmp(allUsers[curUser].files[i].fileName,filename) ==0 ){
							//printf("I ADMIT DEFEAT \n");
							sprintf(pathToFile,"%s//%s", fullPath, filename);
							fd = open(pathToFile, O_RDONLY);
							if(fd < 0){
								printf("Failed\n");
								return -1;
							}
							off_t filesize = lseek(fd, 0 , SEEK_END);
							lseek(fd, 0, SEEK_SET);
							howMany = 0;
							while(howMany < filesize){
								retCode = read(fd, fileContent + howMany, (filesize - howMany));
								if(retCode < 0){
									printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
									return -1;
								}
								howMany += retCode;
							}
							len = filesize;
							retCode = sendAll(client_fd, &len ,sizeof(len),0);
							if(retCode < 0){
								printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
								return -1;
							}
							retCode = sendAll(client_fd, fileContent, len, 0);
							if(retCode < 0){
								printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
								return -1;
							}
							break;
						}
					}
				}
				//file not found
				len = -1;
				retCode = sendAll(client_fd, &len ,sizeof(len),0);
				if(retCode < 0){
					printf("A failure has occurred in line %d , it is: %s\n",__LINE__,strerror(errno));
					return -1;
				}
				break;
			case 4: //quit
				code = -2;
				break;

			}
			if(code == -2){
				close(client_fd);
				free(userName);
				free(password);
				break;
			}
		}


		/*
		 *
		 *
		 * handle client
		 *
		 */
	}




}
