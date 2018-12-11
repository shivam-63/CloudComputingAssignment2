//
//  forksum.c
//
//
//  Created by Gergely Bihary on 2018. 12. 09..
//

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


void createLowerUpperRanges(int begin, int end, int *returnArray){
    int middleValue = 0;
    int range = end - begin + 1;
    
    if(end == begin + 1){
        returnArray[0] = begin;
        returnArray[1] = begin;
        returnArray[2] = end;
        returnArray[3] = end;
        return;
    }
    
    
    if(range%2==0){
        middleValue = begin + (range / 2) - 1;
    }
    else {
        middleValue = begin + (range / 2);
    }
    
    returnArray[0] = begin;
    returnArray[1] = middleValue;
    returnArray[2] = middleValue + 1;
    returnArray[3] = end;
    return;
    
}

int main(int argc , char *argv[] )
{
    pid_t pid;
    
    if (argc != 3)
    {
        printf("Not enough arguments\n");
        exit(0);
    }
    int rangeStart = atoi(argv[1]);
    int rangeEnd = atoi(argv[2]);
    
    if(rangeStart == rangeEnd){
        printf("%d\n", rangeStart);
        return 0;
    }
    
    int leftResult = 0;
    int rightResult = 0;
    
    
    int range[4];
    createLowerUpperRanges(rangeStart, rangeEnd, range);
    int leftStart = range[0];
    int leftEnd = range[1];
    int rightStart= range[2];
    int rightEnd = range[3];
    
    // printf("Range: %d, %d, %d, %d \n", range[0], range[1], range[2], range[3]);
    while (leftResult == 0 || rightResult == 0){
        
        // Traverse left side first
        while (leftResult == 0) {
            
            if(leftStart == leftEnd){
                leftResult = leftEnd;
                //printf("Left finished: %d\n", leftResult);
                break;
            }
            
            // Initialize pipe for the process
            int filedes[2];
            if (pipe(filedes) == -1) {
                perror("pipe");
                exit(1);
            }
            
            // Fork the process
            pid=fork();
            
            
            if ( pid<0 )
            {
                printf("Failed to create child! \n");
                exit(0);
            }
            else if ( pid>0 )
            {
                int wpid;
                int leftAmount = 0;
                while ((wpid = wait(&leftAmount)) > 0);
               
                //printf("Wait over in left side. Known: \n");
                //
                //printf("Left: %d, Right: %d\n", leftResult, rightResult);
                //printf("Range: %d, %d, %d, %d \n", leftStart, leftEnd, rightStart, rightEnd);
                //printf("Range start: %d Range End %d\n", rangeStart, rangeEnd);
                
                char buffer[4096];
                while (1) {
                    ssize_t count = read(filedes[0], buffer, sizeof(buffer));
                    if (count == -1) {
                        if (errno == EINTR) {
                            continue;
                        } else {
                            perror("read");
                            exit(1);
                        }
                    } else if (count == 0) {
                        break;
                    } else {
                        leftResult = atoi(buffer);
                        break;
                        
                    }
                }
                
            }
            
            else {
                
                while ((dup2(filedes[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
                close(filedes[1]);
                close(filedes[0]);
                
                leftResult = 0;
                rightResult = 0;
                
                if(leftStart == leftEnd){
                    leftResult = leftEnd;
                    //printf("Left child finished: %d\n", leftResult);
                    break;
                }
                
                int leftNextRanges[4];
                createLowerUpperRanges(leftStart, leftEnd, leftNextRanges);
                leftStart = leftNextRanges[0];
                leftEnd = leftNextRanges[1];
                rightStart = leftNextRanges[2];
                rightEnd = leftNextRanges[3];
                //printf("Left side next ranges %d - %d, %d-%d\n", leftStart, leftEnd, leftNextRanges[2], leftNextRanges[3]);
                
                
            }
            
        }
        // Traverse right childs of node
        while (rightResult == 0) {
            
            // Exit if the same values are received
            if(rightStart == rightEnd){
                rightResult = rightEnd;
                //printf("Right parent finished: %d\n", rightResult);
                break;
            }
            
            // Initialize pipe for right side
            int filedes[2];
            if (pipe(filedes) == -1) {
                perror("pipe");
                exit(1);
            }
            
            // Fork for the second time
            pid=fork();
            
            if ( pid<0 )
            {
                printf("Failed to create child! \n");
                exit(0);
            }
            else if ( pid>0 )
            {
                int wpid;
                int rightAmount = 0;
                while ((wpid = wait(&rightAmount)) > 0);
                
                //printf("Wait over in right side. \n");
                //printf("Left: %d, Right: %d\n", leftResult, rightResult);
                //printf("Range: %d, %d, %d, %d \n", leftStart, leftEnd, rightStart, rightEnd);
                //printf("Range start: %d Range End %d\n", rangeStart, rangeEnd);
                
                char buffer[4096];
                while (1) {
                    ssize_t count = read(filedes[0], buffer, sizeof(buffer));
                    if (count == -1) {
                        if (errno == EINTR) {
                            continue;
                        } else {
                            perror("read");
                            exit(1);
                        }
                    } else if (count == 0) {
                        break;
                    } else {
                        rightResult = atoi(buffer);
                        break;
                        
                    }
                }
                
            }
            
            else {
                while ((dup2(filedes[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
                close(filedes[1]);
                close(filedes[0]);
                
                leftResult = 0;
                rightResult = 0;
                if(rightStart == rightEnd){
                    rightResult = rightEnd;
                    //printf("Right child process finished: %d\n", rightResult);
                    break;
                }
                int rightNextRanges[4];
                createLowerUpperRanges(rightStart, rightEnd, rightNextRanges);
                
                leftStart = rightNextRanges[0];
                leftEnd = rightNextRanges[1];
                rightStart = rightNextRanges[2];
                rightEnd = rightNextRanges[3];
                //printf("Right side next ranges %d-%d,  %d-%d \n", leftStart, leftEnd, rightStart, rightEnd);
                
                
            }
            
        }
    }
    
    //printf("Finished where start: %d - end: %d and PID: %d \n", rangeStart, rangeEnd, pid);
    //printf("%d \n", leftResult + rightResult);
    int result = leftResult + rightResult;
    
    printf("%d\n", result);
    return 0;
    
}
