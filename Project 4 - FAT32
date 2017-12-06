////Faisal Alnahhas
//UT Arlington - Fall 2017
//OS - Project 4 FAT32
//  mfs.c
// Faisal Alnahhas and Shivanshi Pandya
//
//  Created by Faisal Alnahhas on 12/5/17.
//

// The MIT License (MIT)
//
// Copyright (c) 2016, 2017 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

//#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdint.h>
#include <ctype.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
// so we need to define what delimits our tokens.
// In this case  white space
// will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

//Changing it to 10 per requirement of project
#define MAX_NUM_ARGUMENTS 10     // Mav shell only supports five arguments

//define these values globally as we will be using them for all functions and calls
short BPB_BytesPerSec;
unsigned char BPB_SecPerClus;
int BPB_FATSz32;
unsigned char BPB_NumFATS;
short BPB_RsvdSecCnt;
char BS_VolLab[11];

struct __attribute__((__packed__)) DirectoryEntry {
    char        DIR_Name[11];
    uint8_t     DIR_Attr;
    uint8_t     Uunsed1[8];
    uint16_t    DIR_FirstClusterHigh;
    uint8_t     Uunsed2[4];
    uint16_t    DIR_FirstClusterLow;
    uint32_t    DIR_FileSize;
};
struct DirectoryEntry dir[16];

int LBAToOffset(int32_t sector)
{
    return ((sector - 2) * BPB_BytesPerSec) + (BPB_BytesPerSec * BPB_RsvdSecCnt) + (BPB_NumFATS * BPB_FATSz32 * BPB_BytesPerSec);
}

//function to print cwd
int current_working(char *path)
{
    char cwd[255];
    getcwd(cwd, sizeof(cwd));
    strcat(cwd, "/");
    strcat(cwd, path);
    printf("cwd: %s\n", cwd);
    return 0;
}

int NextLB(uint32_t sector)
{
    FILE *fp;
    uint32_t FATAddress = (BPB_BytesPerSec * BPB_RsvdSecCnt) + (sector * 4);
    int16_t val;
    fseek(fp, FATAddress, SEEK_SET);
    fread(&val, 2, 1, fp);
    return val;
}

//when the user specifies the position and number of bytes they want to read it's handled by this function
void read_file(FILE *fp, int pos, int num_bytes, int file_num)
{
    int block = dir[file_num].DIR_FirstClusterLow; //storing the first cluster of the file user wants to read
    int file_size = dir[file_num].DIR_FileSize;
    uint8_t val[file_size]; //array to store the values user wants to read
    int file_offset = LBAToOffset(block);
    
    //if pos > bytes_per_sec then we iterate through the blocks using NextLB function
    while(pos > 512)
    {
        block = NextLB(block);
        //printf("block now is: %d\n", block);
        pos -= 512;
    }
    
    //if the results coming from the loop of pos + num_bytes >512 then we fseek
    //the file offset and read from there, after we correct the pointer of pos
    if(pos + num_bytes > 512)
    {
        pos = 512-pos;
        fseek(fp, file_offset, SEEK_SET);
        fread(&val, pos, 1, fp);
        printf("val: %s\n", val);
        
    }
    
    else
    {
        //anything else we move the pointer to be at offset+pos
        //this way we're handling all the possible options
        fseek(fp, file_offset + pos, SEEK_SET);
        fread(&val, num_bytes, 1, fp);
        printf("val: %s\n", val);
    }
}


FILE *file_open(char *f)
{
    FILE *fo = fopen(f, "r");
    if (fo == NULL)
    {
        printf("Error: File system image not found\n");
        return fo;
    }
    fseek(fo, 11, SEEK_SET);
    
    
    /*
    FILE *fo = fopen(f, "r");
    
    if (fo == NULL) printf("Error: File system image not found\n");
    
    else
    {
        while (!feof(fo))
        {
            char chars = fgetc(fo);
            if (feof(fo)) break;
            printf("%c", chars);
        }
        fclose(fo);
    } */
    return fo;
    
}

void file_close()
{
    FILE *fc;
    fclose(fc);
}


/*void stats(FILE *fc, char *passed_file)
{
    int i;
    char *name = passed_file;
    long root_dir = (BPB_NumFATS * BPB_FATSz32 * BPB_BytesPerSec) + (BPB_RsvdSecCnt * BPB_BytesPerSec);
    
    //stats gives statistics of the struct of the fat32.img
    //therefore we must fseek to the root directory to read the information from there
    fseek(fc, root_dir, SEEK_SET);
    for(i=0; i<16; i++)
    {
        memset(&dir[i].DIR_Name, 0, 11);
        fread(&dir[i], 32, 1 , fc);
    }
    
    for(i=0;i<16;i++)
    {
        if(dir[i].DIR_Attr == 0x10 || dir[i].DIR_Attr == 0x20) printf("Filename: %s\n", dir[i].DIR_Name);
    }

    
} */

void get_file(char *f, FILE *fp, int file_size)
{
    //val stores the information we read from existing file pointer
    //then write that information from val to a new file poitner we introduced *fw
    int i, firstClus;
    FILE *fw = fopen(f, "w");
    uint8_t val[file_size];
    fread(&val, file_size, 1, fp);
    fwrite(&val, sizeof(val), 1, fw);
    fclose(fw);
}


char *correct_name(char input[11])
{
    int i, i2;
    int l = strlen(input);
    static char input_copy[12];
    int index = 0;
    
    //examine the input name if it has a letter, using isalpha then we capitalize it
    for(i = 0; i<l || index<11; i++)
    {
        if (isalpha(input[i]))
        {
            input_copy[index] = toupper(input[i]);
            index++;
        }
        //conver the . in file names to a space
        else if(input[i]=='.')
        {
            //since we know that all file have extension length of 3
            //we run them through a loop that calculates the correct
            //cutoff bt. file name and extension to pad it with spaces
            for(i2=0; i2<(11-3-i); i2++)
            {
                input_copy[index] = ' ';
                index++;
            }

        }
        //when dealing with folders (no .) we just pad with spaces
        else
        {
            input_copy[index]= ' ';
            index++;
        }
    }
    input_copy[index] = '\0';
    return input_copy;
   
}

int main()
{
    //these two ints open_image and close_image will handle the error messages
    //for when the image file is opened and closed
    int open_image = 0;
    int close_image = 0;
    int command_index = 0;
    char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
    //dynamically create a new copy of cmd_str to avoid collision of memory for the pointers
    char *commands[15]; //array for storing the commands entered
    
    char *pids[255]; //array for storing the pids
    int *pid_index; //a pointer to the index so we can use it later to grab the value of the pid
    int pid_index_copy = 0; //to avoid memory collision use a copy of pid_index
    
    FILE *fopened;
    
    int root_dir;
    //printf("root dir before %d\n", root_dir);
    
    
    
    while( 1 )
    {
        // Print out the msh prompt
    top:        printf ("mfs> ");
        
        // Read the command from the commandline.  The
        // maximum command that will be read is MAX_COMMAND_SIZE
        // This while command will wait here until the user
        // inputs something since fgets returns NULL when there
        // is no input
        
        //no need to use while to wait for user input because we're okay with empty input
        fgets (cmd_str, MAX_COMMAND_SIZE, stdin);
        
        if (!strcmp(cmd_str, "\n"))
        {
            goto top; //if user just hits enter with typing anything go back to prompting mfs>
        }
        
        /* Parse input */
        char *token[MAX_NUM_ARGUMENTS];
        
        int   token_count = 0;
        int token_index = 0;
        int status;
        
        // Pointer to point to the token
        // parsed by strsep
        char *arg_ptr;
        
        char *working_str  = strdup( cmd_str );
        
        // we are going to move the working_str pointer to
        // keep track of its original value so we can deallocate
        // the correct amount at the end
        char *working_root = working_str;
        
        // Tokenize the input stringswith whitespace used as the delimiter
        while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) &&
               (token_count<MAX_NUM_ARGUMENTS))
        {
            token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
            if( strlen( token[token_count] ) == 0 )
            {
                token[token_count] = NULL;
            }
            token_count++;
        }
        
        if((!strcmp(token[token_index], "exit")) || (!strcmp(token[token_index], "quit")))
        {
            status = 0;
            exit(status);
        }
        
        
        else if((!strcmp(token[token_index], "open")))
        {
            if (open_image == 0)
            {
                int i;
                fopened = file_open(token[token_index + 1]);
                if(fopened != NULL)
                {
                    //read the information about the system right after we open it
                    //this way the values are stored when we open it
                    printf("Image file opened.\n");
                    open_image += 1;
                    close_image = 0;
                    fseek(fopened, 11, SEEK_SET);
                    fread(&BPB_BytesPerSec, 2, 1, fopened);
                    
                    fseek(fopened, 13, SEEK_SET);//SecPerClus
                    fread(&BPB_SecPerClus, 1, 1, fopened);
                    
                    
                    fseek(fopened, 36, SEEK_SET);//FATSz32
                    fread(&BPB_FATSz32, 4, 1, fopened);
                    
                    
                    fseek(fopened, 16, SEEK_SET);//NumFATs
                    fread(&BPB_NumFATS, 2, 1, fopened);
                    
                    
                    fseek(fopened, 14, SEEK_SET);//Rsvd
                    fread(&BPB_RsvdSecCnt, 2, 1, fopened);
                    
                    int root_dir = (BPB_NumFATS * BPB_FATSz32 * BPB_BytesPerSec) + (BPB_RsvdSecCnt * BPB_BytesPerSec);
                   
                    fseek(fopened, root_dir, SEEK_SET);
                    for(i=0; i<16; i++)
                    {
                        memset(&dir[i].DIR_Name, 0, 11);
                        fread(&dir[i], 32, 1 , fopened);
                    }
                }
            }

            
        }
        
        else if(open_image==0 && close_image==1)
        {
            printf("Error: File system image must be opened first.\n");
        }
        
        
        else if((open_image == 1) && (close_image == 0))
        {
            if (!strcmp(token[token_index], "cd"))
            {
                int root_dir = (BPB_NumFATS * BPB_FATSz32 * BPB_BytesPerSec) + (BPB_RsvdSecCnt * BPB_BytesPerSec);
                int i, firstClus;
                char *path = token[token_index+1];
                //handle .. first because we don't want it to get formated with correct_name function
                if (!strcmp(path, ".."))
                {
                    for(i=0; i<16; i++)
                    {
                        memset(path, 0 , 12);
                        strncpy(&path[0], dir[i].DIR_Name, 11);
                       // printf("path after copy: %s\n", path);
                        if(!strcmp(path, "..         "))//9 spaces to make sure it's length 11
                        {
                            firstClus = dir[i].DIR_FirstClusterLow;
                            break;
                        }
                        
                    }
                    //printf("first cluster: %d\n", firstClus);
                    //if the obtained value of first cluster is zero then
                    //fseek to root directory instead of new_offset
                    if (firstClus == 0)
                    {
                        fseek(fopened, root_dir, SEEK_SET);
                        for(i=0; i<16; i++)
                        {
                            memset(&dir[i].DIR_Name, 0, 11);
                            fread(&dir[i], 32, 1 , fopened);
                        }
                    }
                    else
                    {
                        int new_offset = LBAToOffset(firstClus);
                        fseek(fopened, new_offset, SEEK_SET);
                        for(i=0; i<16; i++)
                        {
                            memset(&dir[i].DIR_Name, 0, 11);
                            fread(&dir[i], 32, 1 , fopened);
                        }
                    }
                }
                else
                {
                    char *path_copy = path;
                    char new_path[strlen(correct_name(path))];
                    strcpy(new_path, correct_name(path));
                    for(i=0; i<16; i++)
                    {
                        memset(path, 0 , 12);
                        strncpy(&path[0], dir[i].DIR_Name, 11);
                        if(dir[i].DIR_Attr == 0x10)
                        {
                            if(!strcmp(path, new_path))
                            {
                                firstClus = dir[i].DIR_FirstClusterLow;
                            }
                        }
                    }
                    int new_offset = LBAToOffset(firstClus);
                    //fseek to new_offset sets the file points to the desired new location
                    //which we obtain using LBAToOffset
                    fseek(fopened, new_offset, SEEK_SET);
                    for(i=0; i<16; i++)
                    {
                        memset(&dir[i].DIR_Name, 0, 11);
                        fread(&dir[i], 32, 1 , fopened);
                        //printf("dir[i] is %s\n",dir[i].DIR_Name);
                    }
                }
                //current_working(path_copy);
            }
            
            else if (!strcmp(token[token_index], "ls"))
            {
                int i;
                for(i=0;i<16;i++)
                {
                    char name[12];
                    memset(name, 0, 12);
                    strncpy(&name[0], dir[i].DIR_Name, 11);
                    
                    if((dir[i].DIR_Attr == 0x10 || dir[i].DIR_Attr == 0x20))
                    {
                        //printf("dir[0] %c\n", dir[i].DIR_Name[0]);
                        if(name[0] != '?') printf("Filename: %s\n", name);
                    }
                }
            }
            
            else if (!strcmp(token[token_index], "read"))
            {
                //store the user entries int variables which we will use
                //to memset and calculate the correct number of bytes
                //to read
                char *file_name = token[token_index+1];
                int pos = atoi(token[token_index+2]);
                int num_bytes = atoi(token[token_index+3]);
                char new_file_name[strlen(correct_name(file_name))];
                strcpy(new_file_name, correct_name(file_name));
                int i, firstClus, file_num;
                for(i=0; i<16; i++)
                {
                    memset(file_name, 0, 12);
                    strncpy(&file_name[0], dir[i].DIR_Name, 11);
                    if(dir[i].DIR_Attr == 0x10 ||  dir[i].DIR_Attr == 0x20)
                    {
                        if(!strcmp(new_file_name, file_name))
                        {
                            //printf("Filename: %s\n", file_name);
                            firstClus = dir[i].DIR_FirstClusterLow;
                            file_num = i;
                            //printf("first cluster: %d\n", firstClus);
                        }
                        
                    }
                }
                read_file(fopened, pos, num_bytes, file_num);
            }
            
            else if (!strcmp(token[token_index], "open"))
            {
                printf("Error: File image already open\n");
            }
            else if (!strcmp(token[token_index], "info"))
            {
                printf("information: \n");
                printf("BPB_BytesPerSec: %d\n", BPB_BytesPerSec);
                printf("BPB_BytesPerSec: %x\n", BPB_BytesPerSec);
                printf("\n");
                
                printf("BPB_SecPerClus: %d\n", BPB_SecPerClus);
                printf("BPB_SecPerClus: %x\n", BPB_SecPerClus);
                printf("\n");
                
                printf("BPB_FATSz32: %d\n", BPB_FATSz32);
                printf("BPB_FATSz32: %x\n", BPB_FATSz32);
                printf("\n");
                
                printf("BPB_NumFATS: %d\n", BPB_NumFATS);
                printf("BPB_NumFATS: %x\n", BPB_NumFATS);
                printf("\n");
                
                printf("BPB_RsvdSecCnt: %d\n", BPB_RsvdSecCnt);
                printf("BPB_RsvdSecCnt: %x\n", BPB_RsvdSecCnt);
                printf("\n");
            }
            else if (!strcmp(token[token_index], "stat"))
            {
                
                int i, check = 0;
                char *name = token[token_index + 1];
                char stat_name[strlen(correct_name(name))];
                strcpy(stat_name, correct_name(name));
                for(i=0;i<16;i++)
                {
                    
                    if(dir[i].DIR_Attr == 0x10 || dir[i].DIR_Attr == 0x20)
                    {
                        
                        memset(name, 0 , 12);
                        strncpy(&name[0], dir[i].DIR_Name, 11);
                        if ((!strcmp(stat_name, name)))
                        {
                            printf("Filename: %s\n", name);
                            printf("First Cluster: %d\n", dir[i].DIR_FirstClusterLow);
                            printf("File Size: %d\n", dir[i].DIR_FileSize);
                            printf("Attribute %d\n", dir[i].DIR_Attr);
                            printf("\n");
                            check += 1;
                            break;
                        }
                    }
                    
                }
                if(check == 0)
                {
                    printf("Error: File not found.\n");
                }
            }
            
            else if (!strcmp(token[token_index], "volume"))
            {
                fseek(fopened, 71, SEEK_SET);
                fread(&BS_VolLab, 11, 1, fopened);
                if(!strcmp("NO NAME    ", BS_VolLab)) printf("Volume name not found.\n");
                printf("volume: %s\n", BS_VolLab);
            }
            
            else if (!strcmp(token[token_index], "get"))
            {
                char *name = token[token_index + 1];
                int i, firstClus, file_size, check = 0;
                //char *file_name = name;
                char get_name[strlen(correct_name(name))];
                strcpy(get_name, correct_name(name));
                for(i=0;i<16;i++)
                {
                    if(dir[i].DIR_Attr == 0x10 || dir[i].DIR_Attr == 0x20)
                    {
                        memset(name, 0 , 12);
                        strncpy(&name[0], dir[i].DIR_Name, 11);
                        if ((!strcmp(get_name, name)))
                        {
                            firstClus = dir[i].DIR_FirstClusterLow;
                            file_size = dir[i].DIR_FileSize;
                            //printf("file size: %d\n", dir[i].DIR_FileSize);
                            check += 1;
                        }
                    }
                    
                }
                if(check == 0)
                {
                    printf("Error: File not found.\n");
                    
                }
                else
                {
                    int new_offset = LBAToOffset(firstClus);
                    fseek(fopened, new_offset, SEEK_SET);
                    get_file(get_name, fopened, file_size);
                }
            }
            
            
            else if (!strcmp(token[token_index], "close"))
            {
                file_close();
                printf("Closed.\n");
                open_image -= 1;
                close_image += 1;
            }
            
        }
        else if (!strcmp(token[token_index], "close"))
        {
            printf("Error: File system not open.\n");
        }
        
        else
        {
            printf("Error: File system image must be opened first.\n");
        }
        free( working_root );
        
    }
    return 0;
}


