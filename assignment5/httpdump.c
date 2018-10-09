#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

ssize_t lookup_address(const char *host, const char *port, struct sockaddr **addr)
{
	struct addrinfo hints =	{
		0, 
		AF_INET, 
		SOCK_DGRAM, 
		0
	};
	struct addrinfo *ai;
	ssize_t len;
	int code;
	code = getaddrinfo(host, port, &hints, &ai);
	if(code < 0)
	{
		return -1;
	}
	len = ai->ai_addrlen;
	*addr = malloc(len);
	memcpy(*addr, ai->ai_addr, len);
	freeaddrinfo(ai);
	return len;
}

int send_request(FILE* sock, char *host, char *port, char *path)
{
	
	size_t req_len = strlen(host) + strlen(port) + strlen(path) + 62;
	
	char *request = malloc(req_len);
	
	strcpy(request, "GET /");
	strcat(request, path);
	strcat(request, " HTTP/1.0\r\n");
	strcat(request, "Host: ");
	strcat(request, host);
	strcat(request, "\r\n");
	strcat(request, "User-Agent: TXState CS4350 httpdump\r\n\r\n");
				
	
	fprintf(stderr, "> GET /");
	fprintf(stderr, "%s HTTP/1.0\n", path);
	fprintf(stderr, "> Host: %s", host);
	fprintf(stderr, ":%s\n", port);
	fprintf(stderr, "> User-Agent: TXState CS4350 httpdump\n\n");
	
	
	if (fprintf(sock, "%s\r\n", request) < 0)
	{
		return -1;
	}
	
	if (fflush(sock))
	{
		return -1;
	}
	return 0;
}

int fetch_reply(FILE* sock)
{
	char response[1024];
	char *status2;
	char *status;
	int statusReturn;
	status = fgets(response, 1024, sock);
	status2 = status + 9;
	status2 = strtok(status2, " ");
	
	sscanf(status2, "%d", &statusReturn);
	
	
	fprintf (stderr, "< %s\r\n", status);
	while (strstr(fgets(response, 1024, sock), "\r\n")) 
	{
		fprintf(stderr, "< ");
		fputs(response, stderr);
		
	}
	
	
	while (fgets(response, 1024, sock) != NULL)
	{
		
		fputs(response, stdout);
	}
	if (!feof(sock))
	{
		return -1;
	}
	
	if (statusReturn >= 400)
		{
			
			exit (2);
		}
		if (statusReturn > 300 && statusReturn < 399)
		{
			
			exit (1);
		}
	
	return 0;
}


int main(int argc, char **argv)
{
	
	char *host = "localhost";
	char result[8];
	char *test;
	char *path = "/";
	char *port = "80";
	
	char *data = argv[1];
	struct sockaddr *addr;
	ssize_t addr_len;
	
	if (argc < 2)
	{
		fprintf (stderr, "No URL Provided\n");
        exit(EX_USAGE);
	}
	
	
	strncpy(result, data, 7);
	
	
	if (strstr(result, "http://") != 0)
	{
		
				
		test = strtok(data, "://");
		test = strtok(NULL, "\0");
		
		if(strstr(test, ":"))
		{		
			test = strtok(test, ":");
			host = test + 2;
			
			if(strstr(test, "/"))
			{
				test = strtok(NULL, "/");
				port = test;
				
				test = strtok(NULL, "\0");
				if(test == NULL)
				{
					path = "/";
				}
				else
				{
					path = test;
				}
			}
		
			else
			{
				
				test = strtok(NULL, "\0");
				port = test;
				path = "/";
			}
	
		}
		
		else if(strstr(test + 2, "/"))
		{
			
			test = strtok(test, "/");
			host = test;
			port = "80";
			test = strtok(NULL, "\0");
			if(test == NULL)
			{
				path = "/";
			}
			else
			{
				path = test;
			}
			
		}
		else
		{
			
			test = strtok(test, "\0");
				if(test == NULL)
				{
					host = "localhost";
				}
				else
				{
					host = test + 2;
				}
			port = "80";
			path = "/";
		}
		
		
		
		
		
		addr_len = lookup_address(host, port, &addr);
		if (addr_len < 0)
		{
			perror(host);
			exit(EXIT_FAILURE);
		}
		
		int sock = socket(AF_INET, SOCK_STREAM, 0);
		if(sock < 0)
		{
			perror("Socket Error");
			exit(EXIT_FAILURE);
		}
		
		int code = connect(sock, addr, addr_len);
		if (code < 0)
		{
			perror("Connection Error");
			exit(EXIT_FAILURE);
		}
		
		
		FILE *stream = fdopen(sock, "r+");
		
		if (stream == NULL)
		{
			perror("fdopen Error");
			exit(EX_IOERR);
		}
		
		if (send_request(stream, host, port, path) < 0)
		{
			perror("Send Request Error");
			exit(EX_IOERR);
		}
		
		if (fetch_reply(stream) < 0)
		{
			perror("Fetch Reply Error");
			exit(EX_IOERR);
		}
		
		
		
		fclose(stream);
		free(addr);
		
		
	}
	else
	{
		fprintf (stderr, "Does Not Start With http://\n");
        exit(EX_USAGE);
	}
	
	
	return 0;
}

