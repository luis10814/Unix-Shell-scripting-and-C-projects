#include <stdio.h>
#include <dirent.h>

void list_files()
{
	DIR *d;
	struct dirent *dir;
	d = opendir(".");
	
	if (d)
	{
		while ((dir = readdir (d)) != NULL)
		{
			printf ("%s\n", dir->d_name);
		}
		closedir(d);
	}
}

int main(int argc, char **argv)
{
	
	list_files();
	
	return 0;
}

