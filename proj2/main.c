#include <stdio.h>
#include <stdlib.h>
#include "strutils.h"
#include "ftp.h"


void usage(char *name)
{
  printf("Usage: %s ftp://<user>:<password>@<host>/<url-path>\n", name);
  //No need to continue the program, just exit
  exit(EXIT_FAILURE);
}


int main(int argc, char **argv) 
{
  struct FTP_URL *ftp;
  //ftp = ftp_exp_valid_and_struct("ftp://ftp.datafellows.fi/misc/unixutil/skeysrcs.zip");
  char *filename;
  filename = malloc(256 * sizeof(char));
  
  if(argc != 2)
    usage(argv[0]);
  
  ftp = ftp_exp_valid_and_struct(argv[1]);
  if(ftp == NULL)
  {
    write_red_text("[!] No matches found!");
    exit(EXIT_FAILURE);
  }
  char *s = ftp->url_path;
  connect_ftp_to_server(ftp->host, 21);
  if(login_ftp(ftp) != 0)
    exit(EXIT_FAILURE);
  //printf("adosi: %s\n", ftp->url_path);
  //fflush(stdout);
  enter_pasv_mode();
  if((filename = retrieve_file(ftp->url_path)) != NULL)
  {
    printf("filename: %s\n", filename);
    fflush(stdout);
    if(download_file(filename) != 0)
    {
      printf("[!] Something is wrong! The file could not be downloaded!\n");
      exit(EXIT_FAILURE);
    }
  
    write_red_text("The file was downloaded with success!\n");
  } else {
    write_red_text("[!] Something is wrong! The file could not be retrieved!\n");
    exit(EXIT_FAILURE);
  }
  return 0;
}
