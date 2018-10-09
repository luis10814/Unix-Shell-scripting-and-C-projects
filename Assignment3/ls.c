#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>

void listdir(const char *name, int level)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;
    if (!(entry = readdir(dir)))
        return;

    do {
        if (entry->d_type == DT_DIR) {
            char path[1024];
            int len = snprintf(path, sizeof(path)-1, "%s/%s", name, entry->d_name);
            path[len] = 0;
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            printf("%*s[%s]\n", level*2, "", entry->d_name);
            listdir(path, level + 1);
        }
        else
            printf("%*s- %s\n", level*2, "", entry->d_name);
    } while ((entry = readdir(dir)));
    closedir(dir);
}


void lsStat_file(char *filename)
{
	struct passwd *pwd;
	struct group *grp;
    struct stat sb;
    char dest[1024];
    if (lstat(filename, &sb) < 0) {
        perror(filename);
        return;
    }

    if (S_ISREG(sb.st_mode)) {
		printf ("-");
	}
	if (S_ISDIR(sb.st_mode)){
		printf ("d");
	}
	printf( (sb.st_mode & S_IRUSR) ? "r" : "-");
    printf( (sb.st_mode & S_IWUSR) ? "w" : "-");
    printf( (sb.st_mode & S_IXUSR) ? "x" : "-");
    printf( (sb.st_mode & S_IRGRP) ? "r" : "-");
    printf( (sb.st_mode & S_IWGRP) ? "w" : "-");
    printf( (sb.st_mode & S_IXGRP) ? "x" : "-");
    printf( (sb.st_mode & S_IROTH) ? "r" : "-");
    printf( (sb.st_mode & S_IWOTH) ? "w" : "-");
    printf( (sb.st_mode & S_IXOTH) ? "x " : "- ");
	
	
	
	printf("%d ", sb.st_nlink);
	pwd = getpwuid(sb.st_uid);
	printf("%s ", pwd->pw_name);
	grp = getgrgid(sb.st_gid);
	printf("%s ", grp->gr_name);
    //printf("%d %d ", sb.st_uid, sb.st_gid);
    printf("%d ", sb.st_size);
    printf("%s", filename);
    
    if (S_ISLNK(sb.st_mode))
    {
		printf("%s", dest);
	}
	printf("\n");
    
}

void longForm(char *gdir)
{
	DIR *d;
	struct dirent *dir;
	d = opendir(gdir);
	
	if (d)
	{
		while ((dir = readdir (d)) != NULL)
		{
			
			lsStat_file(dir->d_name);
			
		}
		closedir(d);
	}
}


void stat_file(char *filename)
{
    struct stat sb;
    //char dest[1024];
    if (lstat(filename, &sb) < 0) {
        perror(filename);
        return;
    }

    switch (sb.st_mode & S_IFMT) {
        case S_IFDIR:
            printf("%s\n", filename);
            break;
		}
	}

void list_files(char *gdir)
{
	DIR *d;
	struct dirent *dir;
	d = opendir(gdir);
	
	if (d)
	{
		while ((dir = readdir (d)) != NULL)
		{
			printf ("%s\n", dir->d_name);
		}
		closedir(d);
	}
}


void list_dir_file(char *gdir)
{
	DIR *d;
	struct dirent *dir;
	d = opendir(gdir);
	
	if (d)
	{
		while ((dir = readdir (d)) != NULL)
		{
			
			stat_file(dir->d_name);
			
		}
		closedir(d);
	}
}




int main(int argc, char **argv)
{
int opt;


	while ((opt = getopt(argc, argv, "a:d:l:t:S:R")) != -1)
	{
	
		switch (opt)
		{
			case 'a':
				list_files(argv[argc - 1]);
				break;
			case 'd':
				list_dir_file(argv[argc - 1]);
				break;
			case 'l':
				longForm(argv[argc - 1]);
				break;
			case 't':
				printf("test");
				break;
			case 'S':
				printf("test");
				break;
			case 'R':
				listdir(argv[argc - 1], 0);
				break;
			}
			
	}

	
	return 0;
}

