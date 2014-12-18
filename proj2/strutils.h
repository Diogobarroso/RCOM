#ifndef STRUTILS_H_
#define STRUTILS_H_
#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>


#define SIZE_MATCHES 6



/**
 * Simple struct that represents the different parts that form a 
 * FTP url 
 * 
 * TODO: url_path can have more than one path
 */
struct FTP_URL {
  char *username;
  char *password;
  
  char *host;
  char *url_path;
};



//Outputs red text to the console
void write_red_text(char *s);



void print_normal_text(char *s);




/**
 * This function checks whether a given IP address 
 * is valid or not 
 * 
 * Note that this function only tests for IPv4 addresses 
 * Function returns 0 if the ip_address argument is not valid
 * 
 * Based on http://www.logix.cz/michal/devel/various/getaddrinfo.c.xp
 */
int is_valid_ip_address(const char *ip_address);



/**
 * Converts a hostname to the respective IP address
 */
char* host_to_ip(const char *hostname);




/**
 * This function destroys a FTP_URL struct 
 * 
 * @arg what The struct to be destroyied
 */
void ftp_url_destroy(struct FTP_URL *what);




/**
 * Matches a given char array to a given regular expression
 */
regmatch_t* matches_regex(char *string, regex_t *expression);




/**
 * Checks whether a given ftp URL is valid according to the following norm:
 * 
 * ftp://[<user>:<password>@]<host>/<url-path>
 * 
 * If it is then we return a FTP_URL struct
 */
struct FTP_URL* ftp_exp_valid_and_struct(char *ftp_url);




/**
 * This function is used to extract the relevant details from a given FTP url 
 * We extract the username/password and the host/relative path
 * 
 * @arg array The array of regex matches 
 * @arg string The string where we'll be extracting the data from
 */
int extract_ftp_details(regmatch_t *array, char *string, struct FTP_URL *ftp);




/**
 * Extracts a part of a string, between start and end
 * 
 * @arg The start index of the substring
 * @arg The end index of the substring
 */
char *extract_string_part(char *string, int start, int end);





/**
 * This function sets a field from the FTP_URL struct
 * Note that the field is determined by the index that is passed in the second argument
 */
void set_ftp_struct_field(struct FTP_URL *ftp, int index, char *val);



#endif
