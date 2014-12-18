#include "strutils.h"



void write_red_text(char *s)
{
  printf("\033[1;31m%s\033[0m\n", s);
  fflush(stdout);
}


void print_normal_text(char *s)
{
  printf("%s\n", s);
  fflush(stdout);
}



char* host_to_ip(const char *hostname)
{
  struct addrinfo tmp;
  struct addrinfo *res;
  int err;
  char *addr;
  void *ptr;
  
  
  addr = malloc(100 * sizeof(char));
  
  
  memset(&tmp, 0, sizeof(tmp));
  tmp.ai_family = AF_INET;
  tmp.ai_socktype = SOCK_STREAM;
  tmp.ai_flags |= AI_CANONNAME;
  
  
  err = getaddrinfo(hostname, NULL, &tmp, &res);
  if(err != 0)
  {
    printf("[!] Something went wrong while converting the hostname %s to a valid IP address!\nThe program will now exit...\n\n", hostname);
    exit(EXIT_FAILURE);
  }
  
  
  while(res)
  {
    inet_ntop(res->ai_family, res->ai_addr->sa_data, addr, 100);
    switch(res->ai_family)
    {
      case AF_INET:
	ptr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
	break;
	

      case AF_INET6:
	ptr = &((struct sockaddr_in6 *) res->ai_addr)->sin6_addr;
	break;
    }
    
    inet_ntop(res->ai_family, ptr, addr, 100);
    res = res->ai_next;
  }
  
  return addr;
}



int is_valid_ip_address(const char *ip_address)
{
  struct sockaddr_in sock;
  return inet_pton(AF_INET, ip_address, &(sock.sin_addr)) != 0;
}




void set_ftp_struct_field(struct FTP_URL *ftp, int index, char *val)
{
  if(index == 2)
    ftp->username = val;
  else if(index == 3)
    ftp->password = val;
  else if(index == 4)
  {
    int valid_ip = is_valid_ip_address(val);
    if(valid_ip == 0)
    {
      //Convert to IP address 
      char *ip;
      ip = host_to_ip(val);
      ftp->host = ip; 
    } else 
      ftp->host = val;
  }
  else if(index == 5)
    ftp->url_path = val;
}




void ftp_url_destroy(struct FTP_URL *what)
{
  free(what);
}




char* extract_string_part(char *string, int start, int end)
{
  int size;
  size = strlen(string);
  
  //Check for invalid indexes
  if(start < 0 || start > size - 1 || end < 0 || end > size || end < start)
    return NULL;
  
  
  char *tmp_string = NULL;
  size = end - start;
  tmp_string = (char *)malloc(size + 1);
  strncpy(tmp_string, string + start, size);
  tmp_string[size] = '\0';
  
  return tmp_string;
}



int extract_ftp_details(regmatch_t *array, char *string, struct FTP_URL *ftp)
{
  int start, end, current_index;
  
  if((int) array[SIZE_MATCHES - 1].rm_so == -1 || (int) array[SIZE_MATCHES - 1].rm_eo == -1)
    return -1;
  
 
  
  /**
   * At this point we know that the array has 5 matches, so we can 
   * start to extract them
   */
  for(current_index = 1; current_index < SIZE_MATCHES; current_index++)
  {
    start = (int) array[current_index].rm_so;
    end = (int) array[current_index].rm_eo;
    
    //printf("Index is %d. start index is %d and end index is %d\n", current_index, start, end);
    //fflush(stdout);
    
    char *tmp_string = extract_string_part(string, start, end);
    if(tmp_string != NULL)
      set_ftp_struct_field(ftp, current_index, tmp_string);
  }
  
  
  //Check if the credentials are empty
  if(ftp->url_path != NULL && ftp->host != NULL)
  {
    if(ftp->username == NULL && ftp->password == NULL)
    {
      ftp->username = "anonymous";
      ftp->password = "this@me.com";
    }
  }
  
  return 0;
}



regmatch_t* matches_regex(char *string, regex_t *expression)
{
  regmatch_t *total_matches;
  total_matches = malloc(SIZE_MATCHES * sizeof(regmatch_t));
  
  
  /**
   * regexec returns 0 on success, REG_NOMATCH otherwise
   * 
   * Please refer to http://linux.die.net/man/3/regexec for more information
   */
  if(regexec(expression, string, SIZE_MATCHES, total_matches, 0) != 0)
    return NULL;
  
  return total_matches;
}



struct FTP_URL* ftp_exp_valid_and_struct(char *ftp_url) 
{
  const char *FTP_REGEX = "ftp://(([^:].+):([^@].+)@)?(.*?)/(.*)";
  int res, t;
  char tmp_user[40];
  char tmp_pass[40];
  char tmp_host[40];
  char tmp_path[2048];
  
  
  //Build the regular expression(compiled)
  regex_t ftp_exp;
  t = regcomp(&ftp_exp, FTP_REGEX, REG_EXTENDED);
  if(t != 0)
  {
    perror("[!] Error! The regex could not be compiled!");
    return NULL; 
  }

  
  struct FTP_URL *ftp = malloc(sizeof(struct FTP_URL));
  ftp->username = ftp->password = ftp->host = ftp->url_path = NULL;
  
  
  ///regmatch_t *reg;
  res = sscanf(ftp_url, "ftp://%[^:]:%[^@]@%[^/]/%s\n", tmp_user, tmp_pass, tmp_host, tmp_path);
  if(res == 4)
  {
    ftp->username = tmp_user;
    ftp->password = tmp_pass;
    ftp->host = host_to_ip(tmp_host);
    ftp->url_path = tmp_path;
    return ftp; 
  }

  
  res = sscanf(ftp_url, "ftp://%[^/]/%s\n", tmp_host, tmp_path);
  if(res == 2)
  {
    ftp->username = "anonymous";
    ftp->password = "this@me.com";
    ftp->host = host_to_ip(tmp_host);
    ftp->url_path = tmp_path;
    
    return ftp;
  }
  
  /**reg = matches_regex(ftp_url, &ftp_exp);
  if(reg != NULL)
  {
    /**
     * At this point we know that the regex is valid 
     * Now we need to extract the relevant details of the ftp address
     */
    /**int set_vals;
    set_vals = extract_ftp_details(reg, ftp_url, ftp);
    if(set_vals != 0)
    {
      regfree(&ftp_exp);
      ftp_url_destroy(ftp);
      perror("[!] Something went wrong while extracting the data from the FTP URL!");
      return NULL;
    } else 
    {
      //Delete the compiled expression
      regfree(&ftp_exp);
      return ftp;
    }
  }
  
  regfree(&ftp_exp);*/
  return NULL;
}
