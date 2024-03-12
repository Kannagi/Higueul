#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#define MAX_ENTRIES 4000

void listFilesRecursive(const char *path, char *entries[], int *entryCount)
{
    DIR *dir;
    struct dirent *entry;

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
        //printf("%s\n", fullPath);
        entries[*entryCount] = strdup(fullPath);
        (*entryCount)++;
/*
        if (entry->d_type == DT_DIR)
            listFilesRecursive(fullPath, entries, entryCount);
        else
        {
            int l = strlen(fullPath);
            if( (fullPath[l-4] == '.') && (fullPath[l-3] == 'e') && (fullPath[l-2] == 'g') && (fullPath[l-1] == 'l') )
            {
                fullPath[l-4] = 0;
                //rmakeCommand(_rmake_file,_rmake_rmake,_rmake_rmake->cmd_compiler,fullPath,1);

            }else
                printf("unkonw format %s\n",fullPath);

        }
        */
    }

    closedir(dir);
}

void listFiles(FILE *file,const char *path)
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
/*
void *rmake_keyword(RMAKE *rmake,char *keywords)
{
    if(strcmp(keywords,"exe_name") == 0)
        return rmake->exe_name;

    if(strcmp(keywords,"compiler") == 0)
        return rmake->compiler;

    if(strcmp(keywords,"options") == 0)
        return rmake->options;

    if(strcmp(keywords,"target") == 0)
        return rmake->target;

    if(strcmp(keywords,"cmd_precompiler") == 0)
        return rmake->cmd_precompiler;

    if(strcmp(keywords,"cmd_postcompiler") == 0)
        return rmake->cmd_postcompiler;

    if(strcmp(keywords,"cmd_compiler") == 0)
        return rmake->cmd_compiler;

    if(strcmp(keywords,"add_folder") == 0)
        return rmake->folder;

    if(strcmp(keywords,"add_file") == 0)
        return rmake->file;

    return NULL;
}*/

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
                    //printf("cmd:%s\n",buf);

                    if(l > 0)
                    {
						if(kcmd == 1) //cmd_execute
						{
							system(buf);
						}else
						if(kcmd == 2) //compiler_run
						{

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
									k++;
								}
								filename[k] = 0;

								if(kcmd == 0) // option
								{

								}

								if(kcmd == 3) //add_folder
								{

								}

								if(kcmd == 4) //add_folder
								{

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

/*
void rmakeCommand(FILE *file,RMAKE *rmake,char *buffer,char *filename,int compiler_type)
{
    char keywords[32];
    int l;

    int n = strlen(buffer);

    for(int i = 0;i < n;i++)
    {
        if(buffer[i] == '$')
        {
            l = 0;
            i++;
            while((buffer[i] != ' ') && (i < n))
            {
                keywords[l] = buffer[i];
                i++;
                l++;
            }

            keywords[l] = 0;

            if(strcmp(keywords,"compiler") == 0)
            {
				fputs(rmake->compiler,file);
            }


            if(strcmp(keywords,"options") == 0)
                fputs(rmake->options,file);



            if(strcmp(keywords,"file") == 0)
            {
                fputs(filename,file);

				fputs(".egl",file);


                char crt0[6];

                int n = strlen(filename);

                crt0[0] = filename[n-4];
                crt0[1] = filename[n-3];
                crt0[2] = filename[n-2];
                crt0[3] = filename[n-1];
                crt0[4] = filename[n-0];
                crt0[5] = 0;

                if(strcmp(crt0,"crt0") == 0)
                {
                    sprintf(rmake->crt0,"obj/%s.o ",filename);
                }else
                {
                    strcat(rmake->obj,"obj/");
                    strcat(rmake->obj,filename);
                    strcat(rmake->obj,".o ");
                }

                //folderRCreate(filename,"obj/");
            }

            if(strcmp(keywords,"object") == 0)
            {
                if(compiler_type < 16)
                {
                    fputs("obj/",file);
                    fputs(filename,file);
                    fputs(".o",file);
                }
                else
                {
                    fputs(rmake->crt0,file);
                    fputs(rmake->obj,file);
                }

            }

            if(strcmp(keywords,"exe_name") == 0)
                fputs(rmake->exe_name,file);
        }

        if(i < n)
            fputc(buffer[i],file);

    }
}




void rmakeCompileFile(FILE *file,RMAKE *rmake,char *buffer)
{
    char filename[256];
    int n = strlen(buffer);
    for(int i = 0;i < n;i++)
    {
        int l = 0;
        while((buffer[i] != ';') && (i < n))
        {
            filename[l] = buffer[i];
            i++;
            l++;
        }
        filename[l] = 0;

        if(l > 0)
        {
            filename[l] = 0;

            if( (filename[l-2] == '.') && (filename[l-1] == 'c') )
            {
                filename[l-2] = 0;
                rmakeCommand(file,rmake,rmake->cmd_compiler,filename,0);
                fputs("\n",file);
            }else
            if( (filename[l-4] == '.') && (filename[l-3] == 'c') && (filename[l-2] == 'p') && (filename[l-1] == 'p') )
            {
                filename[l-4] = 0;
                rmakeCommand(file,rmake,rmake->cmd_compiler,filename,1);
                fputs("\n",file);
            }else
                printf("unkonw format %s\n",filename);
        }
    }
}


void rmakeCreate(RMAKE *rmake)
{
    FILE *file = fopen(".vscode/compile.sh","w");

    if(file == NULL)
    {
        printf("Error write file compile.sh\n");
        return;
    }


    //fputs(bin_sh,file);

    //pre compiler
    fputs(rmake->cmd_precompiler,file);
    fputs("\n",file);

    //compiler file
    rmakeCompileFile(file,rmake,rmake->file);

    char filename[256];
    int n = strlen(rmake->folder);
    for(int i = 0;i < n;i++)
    {
        int l = 0;
        while((rmake->folder[i] != ';') && (i < n))
        {
            filename[l] = rmake->folder[i];
            i++;
            l++;
        }
        filename[l] = 0;

        if(l > 0)
        {
            filename[l] = 0;

            listFiles(rmake,file,filename);
        }
    }

    fputs("\n",file);


    //linker
    //rmakeCommand(file,rmake,rmake->cmd_linker,"",16);
    fputs("\n\n",file);

    //post compiler
    fputs(rmake->cmd_postcompiler,file);
    fputs("\n",file);

    fclose(file);

    //---------------
    file = fopen(".vscode/execute.sh","w");

    if(file == NULL)
    {
        printf("Error write file execute.sh\n");
        return;
    }

    //fputs(bin_sh,file);
    //fputs(rmake->cmd_execute,file);
    fputs("\n",file);


    fclose(file);
}

*/


int rmakeLoad(char *filename)
{
    char *buffer = NULL;
    int fsize;

    buffer = (char*)loadFile(filename,&fsize);
    if (buffer == NULL)
        return 0;

    rmakeInit(buffer,fsize);

    //rmakeCreate(&rmake);

    return 0;
}


