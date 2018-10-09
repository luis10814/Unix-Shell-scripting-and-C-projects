#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int i;
	int b = 1020;
	char ch[b];
	char *result;
	FILE *ifp, *ofp;
	ifp = fopen(argv[3], "r");
	ofp = fopen("test.txt", "w");

	if(ifp == NULL)
	{
		ifp = stdin;
	}
	
	
	while (fgets(ch, 20, ifp) != NULL)
	{
	
		result = strtok(ch, " ");

		while (result != NULL)
		{
			if (strstr(result, argv[1]) != 0)
			{
				fputs(argv[2], ofp);
				printf("%s", argv[2]);
			
				int m = strlen(result);
				if(result[m - 1] == ',')
				{
					fputs(",", ofp);
					printf(",");
				}
			
						
				for(i=0; i <= strlen(result); i++)
				{
					if(result[i] == '\n')
					{
						fputs("\n", ofp);
						printf("\n");
					}
				
			
				}
		

			}
			else
			{
				fputs(result, ofp);
				printf("%s", result);
				int m = strlen(result);
				if(result[m - 1] != '\n')
				{
					fputs(" ", ofp);
					printf(" ");
				}
			}
			result = strtok(NULL, " ");
	
		}
	}

	fclose(ifp);
    fclose(ofp);

	return 0;
}
