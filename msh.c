 // The MIT License (MIT)
// 
// Copyright (c) 2024 Trevor Bakker 
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
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 32     

void whiteSpace(char *token[], int *count) {
  //moving non white space characters to left
    int left = 0;
    for (int i = 0; i < *count; i++) {
        if (token[i] && *token[i] != '\0') {
            token[left] = token[i];
            left++;
        }
    }
    *count = left; 
}


int main(int argc, char *argv[])
{
  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );

  FILE *input_stream = stdin;
    if (argc == 2) {
        input_stream = fopen(argv[1], "r");
        if (input_stream == NULL) {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            return EXIT_FAILURE;
        }
    } else if(argc > 2) {
      char error_message[30] = "An error has occurred\n";
      write(STDERR_FILENO, error_message, strlen(error_message));
      return EXIT_FAILURE;
    }

  //printf("TESTTTT: %d", argc);
  //printf("%d", input_stream);
  while( 1 )
  {
    // Print out the msh prompt
    int booleanValue = 1;


    // Read the command from the commandi line.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something.

    if (argc == 1)
        {
            printf("msh> ");
        }

        // Read command from input stream
        if (fgets(command_string, MAX_COMMAND_SIZE, input_stream) == NULL)
        {
            // If end of file is reached, break from the loop
            if (feof(input_stream))
            {
                break;
            }
            // Print error message and exit if reading fails
            perror("Error reading input");
            return EXIT_FAILURE;
        }
    
    // else
    //  read from a line from the file

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_pointer;                                         
                                                           
    char *working_string  = strdup( command_string );                

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    
    char *head_ptr = working_string;
    
    // Tokenize the input with whitespace used as the delimiter
    while ( ( (argument_pointer = strsep(&working_string, WHITESPACE ) ) != NULL) &&
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_pointer, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }
    whiteSpace(token, &token_count);
    

    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your shell functionality
    int size = 0;
    while (token[size] != NULL) {
      size++;
    }
    //printf("token count: %d\n", token_count);
    if (size == 0){
      //printf("test");
      continue;
    }
    else if (strcmp(token[0], "exit") == 0) {
      int size = 0;
      while (token[size] != NULL) {
        size++;
      }
      if (size != 1) {
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
      } else {
        exit(0);
      }
    }
    else if (strcmp(token[0], "cd") == 0) {
      
      int size = 0;
      while (token[size] != NULL) {
        size++;
      }
      
      if ((token[1] == NULL) || (size > 2)) {
      
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        //exit(EXIT_FAILURE);
      }
      else {
        char current_directory[1024];
        

        if(strcmp(token[1], "..") == 0) {
          strcpy(current_directory, "..");
        } else {

          if (getcwd(current_directory, sizeof(current_directory)) == NULL) {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
          }
          char destination[1024] = "/";
          strcat(destination, token[1]);
          strcat(current_directory, destination);
        }
        

        if (chdir(current_directory) == -1) {
          char error_message[30] = "An error has occurred\n";
          write(STDERR_FILENO, error_message, strlen(error_message));
        }
      }
    }
    //automatically have the null at the end of our array
    else {
      //fork and pass array into exex (just use execv)
      //PASS ARRAY INTO EXEC VP
      pid_t child_pid = fork();
      int status;
      char path[50]; 

      strcpy(path, "/bin/");
      strcat(path, token[0]);
      int size = 0;
      while (token[size] != NULL) {
        size++;
      }

      //child process is 0
      if( child_pid == 0 )
      {
        //execl("/bin/ls", "ls", NULL );
        int i;
        for( i=1; i<size; i++ )
        {
          if( strcmp( token[i], ">" ) == 0 )
          {
              if (token[i + 1] == NULL || token[i + 2] != NULL) {
                  //char error_message[30] = "An error has occurred\n";
                  //write(STDERR_FILENO, error_message, strlen(error_message)); 
                  //printf("Tests");
                  booleanValue = 0;
              } else {
              int fd = open(token[i+1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );
              if( fd < 0 )
              {
                char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message));                 
              }
              dup2( fd, 1 );
              close( fd );
              
              // Trim off the > output part of the command
              token[i] = NULL;
              }
          }
        }
        //execvp( token[1], &token[1] );

        //printf("Boolean: %d", booleanValue);
        if((booleanValue == 0) || (execv(path, token) == -1 )) {
          //printf("\nCHECKING EERRIOR\n");
          char error_message[30] = "An error has occurred\n";
          write(STDERR_FILENO, error_message, strlen(error_message));
        } else{
          exit( EXIT_SUCCESS );
        }
        
      }

      waitpid( child_pid, &status, 0 );

    }
    
    //PASS ARRAY INTO EXEC Vp
    free( head_ptr );

  }

  
  if (input_stream != stdin) {
    fclose(input_stream);
  }
  free(command_string);
  return 0;
  
}