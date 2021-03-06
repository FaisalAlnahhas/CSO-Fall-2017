////Faisal Alnahhas
//UT Arlington - Fall 2017
//OS - Project 2 Writing a Shell
//  msh.c
//
//
//  Created by Faisal Alnahhas on 10/8/17.
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

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
// so we need to define what delimits our tokens.
// In this case  white space
// will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

//Changing it to 10 per requirement of project
#define MAX_NUM_ARGUMENTS 10     // Mav shell only supports five arguments


//for any path passed to cd, insert / at beginning and concatenate it with the rest of the path
//obtained using getcwd, then use chdir to change directory to the full path
int cd(char *path)
{
    char cwd[255];
    getcwd(cwd, sizeof(cwd));
    strcat(cwd, "/");
    strcat(cwd, path);
    chdir(cwd);
    return 0;
}


//using https://stackoverflow.com/questions/12683466/after-suspending-child-process-with-sigtstp-shell-not-responding
//for ctrl-c
static void handle_signal(int sig)
{
    printf (" Caught signal %d\n", sig );
}

//same as above but for ctrl-z which also needs SIGCHILD handling since it forks, unlike ctrl-c which just stops
//a running process. using switch allows to handle a subcase, i.e. SIGCHILD. While waitpid allows us to wait for
//the child process to end before we can break.
static void handle_signalz(int sig)
{
    int status;
    printf (" Caught signal %d\n", sig );
    
    switch(sig){
        case SIGCHLD:
            waitpid(-1, &status, WNOHANG);
            printf (" Caught signal %d\n", sig );
            break;
    }
    
    
}

//I know we're not supposed to implement cd .. on our own, but I wrote this before I realized that.
/*int cddot()
{
    
    //for this function i am taking the cwd and checking the string from its end until i find a /, since there is no
    //back slash at the end of a path. When I see one I use n to make a copy of the old cwd to a new cwd that stops at n
    //without including the last portion of the path.
    char cwd[255];
    getcwd(cwd, sizeof(cwd));
    char reduced_path[255];
    
    long n = strlen(cwd);
    
    while(n!=0)
    {
        if (cwd[n] == '/')
        {
            break;
        }
        else
        {
            n--;
        }
    }
    strncpy(reduced_path, cwd, n);
    chdir(reduced_path);
    return 0;
} */



int main()
{
    int command_index = 0;
    char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
    //dynamically create a new copy of cmd_str to avoid collision of memory for the pointers
    char * cmd_str_copy = (char*) malloc( MAX_COMMAND_SIZE );
    
    char *commands[15]; //array for storing the commands entered
    
    char *pids[255]; //array for storing the pids
    int *pid_index; //a pointer to the index so we can use it later to grab the value of the pid
    int pid_index_copy = 0; //to avoid memory collision use a copy of pid_index
    pid_index = &pid_index_copy;
    char pids_string[10];
    
    //handling signals for ctrl-c, ctrl-z and bg
    //using code from https://github.com/CSE3320/Code-Samples/blob/master/sigint.c
    
    struct sigaction act;
    //changing the code a bit to suit ctrl-z as well as ctrl-c
    struct sigaction sigstp;
    
    //set the sigaction struct to null
    memset(&act, '\0', sizeof(act));
    
    memset(&sigstp, '\0', sizeof(sigstp));
    
    //Set the handler to use the function handle_signal()
    act.sa_handler = &handle_signal;
    
    sigstp.sa_handler = &handle_signalz;
    
    //Install the handler and check the return value.
    if (sigaction(SIGINT , &act, NULL) < 0)
    {
        perror ("sigaction: ");
    }
    
    //Do the same but for SIGTSTP
    if (sigaction(SIGTSTP, &act, NULL) < 0)
    {
          perror ("sigstop: ");
        
    }
    

    
    while( 1 )
    {
        // Print out the msh prompt
    top:        printf ("msh> ");
        
        // Read the command from the commandline.  The
        // maximum command that will be read is MAX_COMMAND_SIZE
        // This while command will wait here until the user
        // inputs something since fgets returns NULL when there
        // is no input
        
        //no need to use while to wait for user input because we're okay with empty input
        fgets (cmd_str, MAX_COMMAND_SIZE, stdin);
        
        if (!strcmp(cmd_str, "\n"))
        {
            goto top; //if user just hits enter with typing anything go back to prompting msh>
        }
        
        /* Parse input */
        char *token[MAX_NUM_ARGUMENTS];
        
        int   token_count = 0;
        
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
        
        // Now print the tokenized input as a debug check
        // \TODO Remove this code and replace with your shell functionality
        
        //using strndup will allow the heap to create new usable memory for each entry in the array without overrwiting
        //space of each previously allocated pointer, which is useful in making, this way we can create add multiple
        //entries to the array
        cmd_str_copy = strndup(cmd_str, strlen(cmd_str));
        
        commands[command_index] = cmd_str_copy;

        command_index++;
        
        //we set the next index (after incrementing) to null to give the array an end point so that when we print
        //history it only prints up to the number of commands entered
        commands[command_index] = NULL;


        
        int token_index  = 0;
        int status;
       
        //calling exit(0) will cause the shell to stop the program
        if((!strcmp(token[token_index], "exit")) || (!strcmp(token[token_index], "quit")))
        {
            status = 0;
            exit(status);
        }
        
        //after storing the pid as a string we need to change it to an integer using atoi
        //then we call kill on the last pid stored in the array and use SIGCON to continue
        //the process after suspension
        else if(!strcmp(token[token_index], "bg"))
        {
            int last_pid = atoi(pids[(*pid_index)-1]);
           // printf("lastpid: %d \n", last_pid);
            kill (last_pid, SIGCONT);
        }
        

        //printing all the element in the array pids which stores the PIDs of the last (up to a set number)
        else if (!strcmp(token[token_index], "showpids"))
        {
            int i;
            for(i=0; pids[i] != NULL; i++)
            {
                printf("pid%d: %s \n", i , pids[i]);
                continue;
            }

        }
        //calling the cd function introduced above. cd is the 0th index in tokenized input and the
        //desired directory is the 1st element in the tokenized input so we switch to token_index+1
        else if (!strcmp(token[token_index], "cd"))
        {
            cd(token[token_index + 1]);
        }
        
        //this is a method to store the commands entered (up to 15) and listing them when needed using
        //history command
        else if(!strcmp(token[token_index], "history"))
        {
            int i;
            for(i=0; commands[i] != NULL; i++)
            {
                printf("%d: %s", i , commands[i]);
                continue;

            }
        }
        
        
        else
        {
            pid_t child_pid = fork();
            
            //since the pid changes when we fork, we create an array of pointers, as well as an array of string values
            //for the pids. When it's converted to a string and we take a copy of it and locate it at the desired pointer
            //address in the array of pointers, we avoid redefining the values.
            sprintf(pids_string,"%d",child_pid); // convert int values of pid to strings
            //printf("%s\n",pids_string);
            pids[(*pid_index)] = strdup(pids_string);
            
            //assure we're only printing up to 10 values
            if(*pid_index == 10)
                *pid_index = 0;
            (*pid_index)++;
            pids[(*pid_index)] = NULL;

            //to check for command not found we fork, and then check the child process
            //if it returns errno2 then we know the command is not found;
            if (child_pid == 0)
            {

                if(token[token_index][0] == '!')
                {
                    char *number;
                    number = strtok(token[token_index], "!");
                    int num_copy = atoi(number);
                    // printf("number: %d\n", num_copy);
                    //reassign the desired command to prompt line to re-execute it after using !n
                    //token[token_index] = strdup(commands[num_copy]);
                    //token[token_index][strlen(token[token_index])-1] = '\0';
                    //continue;
                    if(num_copy > command_index-1)
                    {
                        //when you meet the above condition sometimes the program
                        //takes a second to be able to "quit" or "exit" works successfully
                        //I don't really know why.
                        //If the command is not in history we want to reprompt msh>
                        printf("Command not in history.\n");
                        goto top;
                    }
                    else
                    {
                        printf("command: %s", commands[num_copy]);
                        token[token_index] = strdup(commands[num_copy]);
                        token[token_index][strlen(token[token_index])-1] = '\0';
                    }
                    
                }
                if(execvp(token[token_index], token) < 0)
                {
                    int error = errno;
                    if (error == 2)
                    {
                        printf("%s: Command Not Found\n", token[token_index]);
                    }
                    exit(1); //if we get here then we failed

                }
                exit(EXIT_SUCCESS); //if we get to here then we want to succesfully exit the process
            }
            else
            {
                waitpid(child_pid, &status, 0);
                
            }

        }
        

        free( working_root );
        
    }
  //  free(cmd_str_copy);
    return 0;
}

