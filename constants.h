/*
 * constants.h
 *
 *  Created on: Nov 19, 2017
 *      Author: root
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#define MAXAMOUNT 15 //How many files can be stored
#define MAXUSERS 15 //How many users can be in the users file
#define MAXFILENAME 100 //Maximum length of the file name
#define INPUTLENGTH 25 //Maximum length of pass,ID
#define MAXFILESIZE 512 //Maximum file size in BYTES
#define DEFPORT 1337 // The default port
#define MAXPATH 500 // The maximum length of a path to file
#define MAXPORT 20 //The maximum amount of chars a port contains
#define MAXLINE 500 //The maximum amount of chars a request can have including arguments
#define MAXFUNCLEN 40 // The maximum amount of chars a request can have not including arguments
#define MAXFILENAME 100 //The maximum amount of chars a file name can contain
#define CONSTINPUT 15 // The maximum amount of chars a request can have not including arguments
#define USERPASSLINE (CONSTINPUT + INPUTLENGTH)


#endif /* CONSTANTS_H_ */
