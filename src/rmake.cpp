#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#define MAX_ENTRIES 4000

void rmake_add_file(char *filename);
void rmake_option(char *filename);

void rmake_compile_run(char *target);

void listFilesRecursive(const char *path, char *entries[], int *entryCount)
{
	DIR *dir;
	struct dirent *entry;
	struct stat info;

	if ((dir = opendir(path)) == NULL)
	{
		perror("opendir");
		exit(EXIT_FAILURE);
	}

	while ((entry = readdir(dir)) != NULL)
	{
		// Ignore "." and ".."
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;

		char fullPath[1024];
		snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

		// Store the file path in entries array
		entries[*entryCount] = strdup(fullPath);
		(*entryCount)++;


		if (stat(fullPath, &info) != 0) {
			perror("stat");
			continue;
		}

		if (S_ISDIR(info.st_mode)) {
			// It's a directory
			listFilesRecursive(fullPath, entries, entryCount);
		} else {
			int l = strlen(fullPath);
			if( (fullPath[l-4] == '.') && (fullPath[l-3] == 'e') && (fullPath[l-2] == 'g') && (fullPath[l-1] == 'l') )
			{
				//printf("konw format %s\n",fullPath);
				rmake_add_file(fullPath);

			}//else
				//printf("unkonw format %s\n",fullPath);
		}

	}

	closedir(dir);
}

void listFiles(const char *path)
{
	char *entries[MAX_ENTRIES];
	int entryCount = 0;

	listFilesRecursive(path, entries, &entryCount);

	for (int i = 0; i < entryCount; i++)
		free(entries[i]);
}

void* loadFile(const char* filename,int *sz)
{
	int fileSize;
	FILE* file = fopen(filename, "r");
	if (file == NULL)
	{
		fprintf(stderr, "Impossible to open the file : %s\n", filename);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (fileSize <= 0)
	{
		fprintf(stderr, "Empty file: %s\n", filename);
		fclose(file);
		return NULL;
	}


	char* buffer = (char*)malloc(fileSize+2);
	if (buffer == NULL)
	{
		fprintf(stderr, "Memory allocation error :%s /%d\n",filename,fileSize);
		fclose(file);
		return NULL;
	}

	int bytesRead = fread(buffer, 1, fileSize, file);
	if(bytesRead != fileSize)
	{
		fprintf(stderr, "Error reading file : %s\n", filename);
		fclose(file);
		free(buffer);
		return NULL;
	}

	fclose(file);

	*sz = fileSize;

	buffer[fileSize] = 0;

	return buffer;
}


int rmake_keyword(char *keywords)
{
	if(strcmp(keywords,"options") == 0)
		return 0;

	if(strcmp(keywords,"cmd_execute") == 0)
		return 1;

	if(strcmp(keywords,"compiler_run") == 0)
		return 2;

	if(strcmp(keywords,"add_folder") == 0)
		return 3;

	if(strcmp(keywords,"add_file") == 0)
		return 4;

	return -1;
}


void rmakeInit(char *buffer,int n)
{
	int l = 0;
	char keywords[32];
	char buf[2048];
	char filename[256];
	int lec = 0;
	int kcmd = -1;

	for(int i = 0;i < n ;i++)
	{
		if(buffer[i] == '#')
		{
			while((buffer[i] != '\n') && (i < n)) i++;
		}

		if(buffer[i] >= ' ')
		{
			if(lec == 0)
			{
				l = 0;
				while((buffer[i] != ':') && (i < n))
				{
					keywords[l] = buffer[i];
					i++;
					l++;
				}

				if(l > 0)
				{
					keywords[l] = 0;
					l = 0;
					//printf("key:%s\n",keywords);

					kcmd = rmake_keyword(keywords);

					lec = 1;
				}

				if(buffer[i+1] == '\n')
					lec = 0;
			}
			else
			{
				if(kcmd != -1)
				{
					l = 0;
					while((buffer[i] != '\n') && (i < n))
					{
						buf[l] = buffer[i];
						i++;
						l++;
					}
					buf[l] = 0;
					//printf("cmd:%s \n",buf);

					if(l > 0)
					{
						if(kcmd == 1) //cmd_execute
						{
							system(buf);
						}else
						if(kcmd == 2) //compiler_run
						{
							rmake_compile_run(buf);
						}else
						{
							int bn = strlen(buf);
							int j = 0;
							while(j < bn)
							{

								int k = 0;
								while((buf[j] != ';') && (j < bn))
								{
									filename[k] = buf[j];
									k++;
									j++;
								}
								j++;
								filename[k] = 0;

								if(kcmd == 0) // option
								{
									rmake_option(filename);
								}

								if(kcmd == 3) //add_folder
								{
									listFiles(filename);
								}

								if(kcmd == 4) //add_file
								{
									rmake_add_file(filename);
								}

							}

						}


						l = 0;
					}
				}else
				{
					while((buffer[i] != '\n') && (i < n)) i++;
				}

				lec = 0;
			}


		}
	}
}



int rmakeLoad(const char *filename)
{
	char *buffer = NULL;
	int fsize;

	buffer = (char*)loadFile(filename,&fsize);
	if (buffer == NULL)
	{
		printf("Error: Unable to open higueul_make.txt file\n");
		return 0;
	}


	rmakeInit(buffer,fsize);

	//rmakeCreate(&rmake);

	return 0;
}


