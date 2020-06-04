//Faisal ALnahhas
//UTA - Fall 2017
//Operating System - Project 3 
//  mandelseries.c
//  
//
//  Created by Faisal Alnahhas on 10/20/17.
//

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/time.h>

int main(int argc, char * argv[])
{
    if (argc != 2)
    {
        printf("you must enter only two arguments, mandelseries n\n");
        exit(-1);
    }
        
    struct timeval begin, end;
    
    char mandelstring [] = "./mandel -x 0.286932 -y 0.014287 -s 2 -m 1000 -o mandeli.bmp";
    int i, x , t, j;
    int count = 0;
    int status;

    //introcue array of strings to tokenize input later
    char *tokens[20];
    
    int n = atoi(argv[1]);
    
    int m = 50 - (50%n);
    
    float s = 2;
    
    
    //tokenize the first entry, i.e. ./mandel then tokenize the rest in the loop below
    tokens[0] = strtok(mandelstring, " ");
    printf("first token: %s\n", tokens[0]);
    for(t=1; tokens[t-1]!=NULL ; t++)
    {
        tokens[t] = strtok(NULL, " ");
        printf("token is: %s\n", tokens[t]);
        
    }

    gettimeofday(&begin, NULL);
    
    for (i=0; i<m; i= i+n)
    {
        printf("n is: %d\n", n);
        printf("i is: %d\n", i);
        for(j=0; j<n; j++)
        {
            pid_t child_pid = fork();
            if(child_pid == 0)
            {
               // sprintf(tokens[3], "%f", z);
                sprintf(tokens[6], "%f", s);
                //printf("z is %f\n",z);
                sprintf(tokens[10], "mandel%d.bmp", count);
                printf("token[10] is: %s", tokens[10]);
                printf("\n");
                printf("s is %f\n",s);
                execvp(tokens[0], tokens);
                exit(EXIT_SUCCESS);
            }
            else
            {
                waitpid(child_pid, &status, 0);
            }
            s = s /(1.276499356);
            count++;
        }
    }
    n = 50%n;
    for(x=0; x<=n-1; x++)
    {
        pid_t child_pid = fork();
        if(child_pid == 0)
        {
            sprintf(tokens[6], "%f", s);
            sprintf(tokens[10], "mandel%d.bmp", count);
            printf("\n");
            printf("s is %f\n",s);
            execvp(tokens[0], tokens);
            exit(EXIT_SUCCESS);
        }
        else
        {
            waitpid(child_pid, &status, 0);
        }
        s = s /(1.276499356);
        count++;
    
    }
    
    gettimeofday(&end, NULL);
    
    int time_duration = ((end.tv_sec - begin.tv_sec) * 1000000 + (end.tv_usec - begin.tv_usec));
    
    printf("Duration: %d\n", time_duration);
    
    
    return 0;
}
