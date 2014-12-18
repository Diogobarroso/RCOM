#include "ftp.h"


int socket_id = -1;
int socket_id_download = -1;


void print_error_message_write(char *command)
{
  char buff[1024];
  snprintf(buff, sizeof(buff), "[!] Something is wrong! We could not send the message %s to the server!\n", command);
  fflush(stdout);
}



void print_error_message_read(char *command)
{
  char buff[1024];
  snprintf(buff, sizeof(buff), "[!] Something is wrong! We could not read the message from the server after the %s command\n", command);
  fflush(stdout);
}




int download_file(const char *filename)
{
  FILE *file;
  char buff[256];
  int tmp_bytes;
  
  
  file = fopen(filename, "w");
  if(!file)
    return -1;
  
  
  while((tmp_bytes = read(socket_id_download, buff, sizeof(buff))) > 0)
  {
    if(!tmp_bytes)
      return -2;
    
    if(fwrite(buff, tmp_bytes, 1, file) < 0)
      return -3;
  }
  
  
  //close the file, never forget
  fclose(file);
  return 0;
}




int retrieve_file(const char *filename)
{
  int err;
  
  err = send_ftp_command("RETR welcome.msg\n");
  if(err == -1)
  {
    print_error_message_write("RETR");
    return err;
  }
  
  
  char str[1024] = "";
  err = read_from_ftp(str, sizeof(str));
  if(err == -1)
    return err;
  
  
  return 0;
}




int enter_pasv_mode()
{
  int err;
  char buff[1024] = "";
  int ip[4];
  int port[2];
  char pasv_ip[16];
  int pasv_port, new_socket_id;
  
  
  
  printf("[*] Sending PASV command!\n");
  fflush(stdout);
  
  
  //Send the PASV command to the server 
  err = send_ftp_command("PASV\n");
  if(err == -1)
  {
    print_error_message_write("PASV");
    return err;
  }
  
  err = read_from_ftp(buff, sizeof(buff));
  if(err == -1)
  {
    print_error_message_read("PASV");
    return err;
  }
  write_red_text(buff);
  
  //We should be in passive mode at this moment
  err = sscanf(buff, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &ip[0], &ip[1], &ip[2], &ip[3], &port[0], &port[1]);
  if(err < 0)
  {
    print_normal_text("[!] An error occurred while trying to read the data from the PASV command!");
    return err;
  }
  
  memset(&pasv_ip, 0, sizeof(pasv_ip));
  err = sprintf(pasv_ip, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  
  printf("[*] The new IP address is: %s\n", pasv_ip);
  fflush(stdout);
  pasv_port = 256 * port[0] + port[1];
  printf("[*] The new port is: %d\n", pasv_port);
  fflush(stdout);
  
  
  new_socket_id = create_socket_connection(pasv_ip, pasv_port);
  if(new_socket_id < 0)
  {
    perror("An error occurred while connecting to the NEW server!\n");
    return -1;
  }
 
  socket_id_download = new_socket_id;
  return 0;
}



int login_ftp(struct FTP_URL *ftp)
{
  int err;
  
  print_normal_text("[*] Sending USER command!");
  char buff[1024];
  snprintf(buff, sizeof(buff), "USER %s\n", ftp->username);
  //Send the username to the server
  err = send_ftp_command(buff);
  if(err == -1)
  {
    print_error_message_write("USER");
    return err;
  }
  
  //Read the message from the server
  memset(&buff, 0, sizeof(buff));
  err = read_from_ftp(buff, sizeof(buff));
  if(err == -1)
  {
    print_error_message_read("USER");
    return err;
  }
  //Show the message from the server to the user
  write_red_text(buff);
  
  
  memset(&buff, 0, sizeof(buff));
  print_normal_text("[*] Sending PASS command");
  snprintf(buff, sizeof(buff), "PASS %s\n", ftp->password);
  err = send_ftp_command(buff);
  if(err == -1)
  {
    print_error_message_write("PASS");
    return err;
  }
  
  
  memset(&buff, 0, sizeof(buff));
  err = read_from_ftp(buff, sizeof(buff));
  if(err == -1)
  {
    print_error_message_read("PASS");
    return err;
  }
  write_red_text(buff);
  
  if(buff[0] == '4' || buff[0] == '5')
    return -1;
  
  //Success
  return 0;
}



int read_from_ftp(char *command, size_t size)
{
  if(socket_id < 0)
  {
    perror("[!] Something is wrong! It looks like no socked connection exists! Exiting the program now...\n");
    exit(EXIT_FAILURE);
  }
  
  
  int offset = 0;
  ssize_t tmp_bytes;
  size_t size_left;
  size_left = size;
  
  
  /**while(size_left > 0)
  {
    tmp_bytes = read(socket_id, command, size_left);
    if(tmp_bytes < 0)
      return -1;
    if(tmp_bytes == 0)
      break; 
    
    size_left -= tmp_bytes;
    command += tmp_bytes;
    if(size_left <= 0)
      return 0;
  }*/
  tmp_bytes = read(socket_id, command, size);
  if(!tmp_bytes)
    return -1;
  
//  printf("Command is: %s\n", command);
  
  return 0;
}



int send_ftp_command(char *command)
{
  //Error, no connection is opened. Should never happen
  if(socket_id < 0)
  {
    perror("[!] Something is wrong! It looks like no socket connection exists! Exiting the program now...\n");
    exit(EXIT_FAILURE);
  }
  
  
  int size_left, size_copy, tmp_bytes;
  size_left = size_copy = strlen(command);
  
  while(size_left > 0)
  {
    tmp_bytes = write(socket_id, command, size_left);
    size_left -= tmp_bytes;
    if(size_left <= 0)
      return 0;
    command += tmp_bytes;
  }
  
  return -1;
}




int connect_ftp_to_server(char *ip_address, int port)
{
  if(socket_id < 0)
    socket_id = create_socket_connection(ip_address, port);
  
  
  /**
   * Error, should never happen
   * 
   * The error message is already displayed before in the create_socket_connection function
   */
  if(socket_id < 0)
    exit(EXIT_FAILURE);
  
  
  /**
   * Success, looks like we're connected to the server
   * 
   * Now we need to inform the user and login
   */
  printf("[*] You're connected to the server(%s)! We're now logging you in...\n", ip_address);
  fflush(stdout);
  
  
  //Show the "welcome" message from the FTP server
  char welcome_message[1024] = "";
  memset(&welcome_message, 0, sizeof(welcome_message));
  read_from_ftp(welcome_message, sizeof(welcome_message));
  write_red_text(welcome_message);
  
  
  return 0;
}




int create_socket_connection(char *ip_address, int port)
{  
  int sock_id, connect_id;
  struct sockaddr_in sock;
  
  
  //Sets all values in buffer to zero
  bzero((char *)&sock, sizeof(sock));
  
  //Set the basic socket data 
  sock.sin_family = AF_INET;
  sock.sin_addr.s_addr = inet_addr(ip_address);
  sock.sin_port = htons(port);
  
  
  
  sock_id = socket(sock.sin_family, SOCK_STREAM, 0);
  if(sock_id < 0)
  {
    perror("[!] An error occurred while creating the socket!");
    return -1; 
  }
  
  
  
  connect_id = connect(sock_id, (struct sockaddr *) &sock, sizeof(sock));
  if(connect_id < 0)
  {
    perror("[!] An error occurred while connecting to the server!");
    return -1;
  }
  
  
  return sock_id;
}