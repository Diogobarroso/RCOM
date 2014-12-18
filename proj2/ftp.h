#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include "strutils.h"


#define FTP_PORT 21


//Global variables
extern int socket_id;
extern int socket_id_download;


void print_error_message_write(char *command);
void print_error_message_read(char *command);



/**
 * This function creates a connection by sockets with a given ip_address
 * 
 * returns the socket id on success, -1 otherwise
 */
int create_socket_connection(char *ip_address, int port);



/**
 * This function tries to log in a user
 */
int login_ftp(struct FTP_URL *ftp);



int quit_ftp();




/**
 * This function enters in passive mode 
 * 
 * For more information about the passive/active modes please refer to http://slacksite.com/other/ftp.html#pasvexample
 */
int enter_pasv_mode();



/**
 * Executes the RECV command with a given filename
 * 
 * @param filename The name of the file we want to download
 */
char* retrieve_file(const char *filename);




/**
 * Downloads a file from the FTP server
 * 
 * @param filename The name of the file
 */
int download_file(const char *filename);





/**
 * Connect our application to a FTP server with a given ip address 
 * 
 * 
 * @param ip_address The IP address of the FTP server
 */
int connect_ftp_to_server(char *ip_address, int port);




/**
 * Writes a series of bytes to the server
 * 
 * We also make sure that the correct number of bytes were sent 
 */
int send_ftp_command(char *command);




/**
 * Reads a command from the FTP server with a given size 
 */
int read_from_ftp(char *command, size_t size);