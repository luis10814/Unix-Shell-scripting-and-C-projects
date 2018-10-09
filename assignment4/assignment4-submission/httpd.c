#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>
#include <ctype.h>
#include <assert.h>
#include "mimetypes.h"
#include "request.h"
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>

int main (int argc, char **argv)
{
	
	int buf = 1024;
	char dest[buf];
	char path[1024];
	char str[1024];
	char *dirName = "test-site";
	char *fullPath;
	FILE *file;
	//DIR *d;
	//struct dirent *dir;
	struct stat sb;
	DIR *dir;
	
	
	
	if (argc > 2)
	{
		fprintf (stderr, "Too many arguments\n");
        return 2;
	}

	
	
	req_t *req = req_create();
	//char *req_path(req);
	
	
	while (req_is_complete(req) == false)
	{		
		fgets(dest, buf, stdin);
		size_t strSize = strlen(dest);
		req_parse(req, dest, strSize);
	}
	
	if (req_is_valid(req) == true)
	{
		
		enum http_method method = req_method(req);
		if (method == HTTP_METHOD_POST)
		{
			fprintf (stderr, "405 Method Not Allowed\n");
			return -1;
		}
		
		
		strcpy(path, req_path(req));
		
		
		if (strstr(path, ".."))
		{
			fprintf (stderr, "400 Bad Request\n");
			return -1;
		}
		
			
		
		guess_content_type(path);
		
		
		
		fullPath = malloc(strlen(path)+1+9);
		strcpy(fullPath, dirName); 
		strcat(fullPath, path);
		
		
		
		
		
		file = fopen(fullPath, "r");
		if(errno == ENOENT)
		{
			fprintf (stderr, "404 Not Found\n");
			return -1;
		}
		if(errno == EACCES)
		{
			fprintf (stderr, "403 Forbidden\n");
			return -1;
		}
				
		else if (file) 
		{
			stat(fullPath, &sb);
			printf("HTTP/1.0 200 OK\r\n");
			printf("Content-Type: ");
			if(guess_content_type(path) != NULL)
			{
				printf("%s", guess_content_type(path));
				printf(" ");
				printf("%lld ", sb.st_size);
				printf(" bytes");
				printf(" ");
				printf("Last Modified - ");
				struct tm *tm;
				char buf[200];
				tm = localtime(&sb);
				strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", tm);
				printf("%s\n", buf);
				printf("\r\n");
			}
			else
			{
				printf("%lld", sb.st_size);
				printf(" ");
				printf("%s", sb.st_mtime);
				printf("\r\n");
				
			}
			
			
			printf("\r\n");
			while (fgets(str, 1024, file))
			{
			printf("%s",str);
			}
			fclose(file);
			printf("\r\n");
		}
		else
		{
			fprintf(stderr, "500 Internal server Error\n");
			return -1;
		}
		
		
	}
	else
	{
		fprintf (stderr, "400 Bad Request\n");
		return -1;
	}
		
	
	
	return 0;
}

