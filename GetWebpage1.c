//gcc -o test test.c -lssl -lcrypto
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <stdarg.h>
#include "GetWebpage1.h"
#include <ctype.h>
#include <assert.h>

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>




char* countries[196];


void loadCountries(char* path){
			
	FILE *fp = fopen(path, "r");
	

	int i =0;
	if (fp != NULL){
		char line[128];
		while (fgets(line, sizeof line, fp) != NULL){
			countries[i] = strdup(line);  
			i++;     	
	    }
	}
}



char* getWebpage(char* bandName){

	int sd;
	struct hostent *host;
	struct sockaddr_in addr;
	BIO *outbio = NULL;
	SSL_METHOD *method;
	SSL_CTX *ctx;
	SSL *ssl;
	char *req;
	int req_len;
	char *hostname = "www.metal-archives.com";
	char *certs = "/etc/ssl/certs/ca-certificates.crt";
	int port = 443;
	int bytes;
	char buf[128];
	
	loadCountries("./listOfCountries.txt");
	
	
    /* Load all digest and cipher algorithms */
	OpenSSL_add_all_algorithms();
	ERR_load_BIO_strings();
	
	/* Load the human readable error strings for libcrypto */
	ERR_load_crypto_strings();

	SSL_load_error_strings();
	
	//return a new BIO with BIO_S_file method
	outbio = BIO_new(BIO_s_file());
	//                  filestream, int flags
	outbio = BIO_new_fp(stdout, BIO_NOCLOSE);
	
	if (SSL_library_init() < 0){
		BIO_printf(outbio, "Couldn't initialize OpenSSL library \n");
	}
	
	
	method = SSLv23_client_method();
	ctx = SSL_CTX_new(method); //initialize ctx with SSLv23 client method
	SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2); //dont use SSLv2
	
	host = gethostbyname(hostname); //gethostbyname function outdated?
	sd = socket(AF_INET, SOCK_STREAM, 0); //address family, TCP socket
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = *(long*)(host->h_addr);
	
	if (connect(sd, (struct sockaddr*)&addr, sizeof(addr)) == -1){
		BIO_printf(outbio, "Cannot connect to host %s [%s] on port %d.\n", hostname, inet_ntoa(addr.sin_addr), port);
	}
	
	ssl = SSL_new(ctx);
	SSL_set_fd(ssl, sd);
	SSL_connect(ssl);
	
	char request_buffer[1000];
	request_buffer[0] = '\0';
	
	strcat(request_buffer, "GET /bands/");
	strcat(request_buffer, bandName);
	strcat(request_buffer, " HTTP/1.0\r\n");
	
	strcat(request_buffer, "Host: ");
	strcat(request_buffer, hostname);
	strcat(request_buffer, "\r\n");
	
	strcat(request_buffer, "\r\n"); //Must end HTTP with a blank line
	
	req_len  = strlen(request_buffer);
	SSL_write(ssl, request_buffer, req_len); //(ssl, buf, numBytes) write numBytes from buf into ssl
	
	memset(buf, '\0', sizeof(buf));
	bytes = SSL_read(ssl, buf, sizeof(buf));

	

	char *countryOfOrigin = malloc(128);
	char * country;
	
	FILE *fp;
	fp = fopen("./test.txt", "w+");
	
	while(bytes > 0){
	
		/*
		if (strstr(buf, "https://www.metal-archives.com/lists/")){
			sscanf(buf, "%*[^>]>%127[^<]", countryOfOrigin);
			printf("\n------------%s----------\n", countryOfOrigin);
			break;
		}*/
		
		//write(STDOUT_FILENO, buf, bytes);
		fprintf(fp, "%s", buf);	
		memset(buf, '\0', sizeof(buf));
		bytes = SSL_read(ssl, buf, sizeof(buf));
	}

	fclose(fp);
	
	country = findCountry("./test.txt");
	printf("%s\n", country);
	
	
	SSL_free(ssl);
	close(sd);
	SSL_CTX_free(ctx);
	
	return (countryOfOrigin);
	//>[^<>]+<
	//%*[^>]>%127[^<]
}



char* findCountry(char* filename){
	
	char * line = NULL;
	char* trimmedLine = NULL;
	size_t len = 0;
	ssize_t read;
	FILE * fp;
	char* origin = malloc(128);
	char* country;
	
	fp = fopen(filename, "r");
	if (fp == NULL){
		exit(EXIT_FAILURE);
	}
	
    while ((read = getline(&line, &len, fp)) != -1) {
    
		if (strstr(line, "https://www.metal-archives.com/lists/")){
			printf("%s\n", line);
			int a;
			for (a = 0; a < 196; a++){
				country = countries[a];
				
				len = strlen(country);
				if (country[len-1] == '\n')
					country[len-1] = 0;
				
				if (strstr(line, country)){
					return countries[a];
				}
			}
			break;
		}
	}

	/*	
	char** res = str_split(line, '<');
    if (res)
    {
        int i;
        for (i = 0; *(res + i); i++)
        {
            printf("band=[%s]\n", *(res + i));
            free(*(res + i));
        }
        printf("\n");
        free(res);
    }*/
	
	fclose(fp);
	return origin;
	
	
}


char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}




char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            //assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        //assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}



































