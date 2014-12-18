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
  //ftp = ftp_exp_valid_and_struct("ftp://ftp.rediris.es/anella");
  
  if(argc != 2)
    usage(argv[0]);
  
  
  ftp = ftp_exp_valid_and_struct(argv[1]);
  if(ftp == NULL)
  {
    write_red_text("[!] No matches found!");
    exit(EXIT_FAILURE);
  }
  connect_ftp_to_server(ftp->host, 21);
  if(login_ftp(ftp) != 0)
    exit(EXIT_FAILURE);
  enter_pasv_mode();
  if(retrieve_file("welcome.msg") != -1)
    download_file("welcome.msg");
  
  return 0;
}
