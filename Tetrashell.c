#include"tetris.h"
#include<stdio.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>
#include<fcntl.h>
#include<pwd.h>
#include<curses.h>


//Stuff that is done: core functionality, improved prompts, undo last modify, list of functions, help, info, switch, quick-rank, pretty recover, pretty rank

struct TetrisGameState;

int main(int argc, char* argv[]){
	char file_to_edit[100];
	
	if(argc != 1){
		perror("Please start the program without any arguments using: './tetrashell'.");
	}
	
	printf("\033[93mHello and welcome to the ultimate version of...\033[0m\n ");
	printf(
 " \033[91m                                            \n"
 " \033[91m    _____     _           _____ _       _ _ \n"
 " \033[91m   |_   _|___| |_ ___ ___|   __| |_ ___| | |\n"
 " \033[91m     | | | -_|  _|  _| .'|__   |   | -_| | |\n"
 " \033[91m     |_| |___|_| |_| |__,|_____|_|_|___|_|_|\n"
 " \033[0m                                            \n"	
	);

	printf("\033[93mEnter the path of the quicksave you would like to edit:\033[0m ");
	
	fgets(file_to_edit, sizeof(file_to_edit), stdin);
	file_to_edit[strcspn(file_to_edit,"\n")] = 0;
	
	printf("\033[93m'%s' set as the quicksave to be modified.\n\033[0m", file_to_edit);
       	
	printf("\033[93mPlease enter a command below to edit your file, do 'help <command>' for basic information on how to use a command, or type 'functions' to get a list of all possible functions.\033[0m\n");
	
	//Gets username for improved prompt
	char input[100];
	uid_t uid = getuid();
        struct passwd *name = getpwuid(uid);
       	
	//Creates the shortened version of file name for prompt
	char fie_abreviated[8];
        int i; 
	for (i = 0; i < 4; i++) {
    		fie_abreviated[i] = file_to_edit[i];
	}	

	fie_abreviated[4] = '.';
	fie_abreviated[5] = '.';
	fie_abreviated[6] = '.';
	fie_abreviated[7] = 0; 
	
	//This second game variable is used to hold the previous version of a modified file
	TetrisGameState game2;
	
	while (1) {
		//Finds the score and lines of the game given to it, used in the improved prompt
		FILE *fp = fopen(file_to_edit, "rb");
         	TetrisGameState game;
        	if (fp == NULL) {
                	perror("Failed to open file");
                	return 1;
       	 	}	
        	size_t read_size = fread(&game, sizeof(TetrisGameState), 1, fp);
        	fclose(fp);
        	int current_game_score = game.score;
        	int current_game_lines = game.lines;
		
		//Prints a very colorful and fun improved prompt
		printf("\033[38;5;51m%s\033[0m", name->pw_name);
		printf("\033[91m@Tetrashell\033[0m");
		printf("\033[95m[%s]", fie_abreviated);
		printf("\033[92m[%d/%d]", current_game_score, current_game_lines);
		printf("\033[93m>\033[0m");

        	//Gets function inputted by the user and removes new lines from the array and replaces it with null terminator(s)
		fgets(input, sizeof(input), stdin);
        	input[strcspn(input, "\n")] = 0;
        	
		//Tokenizes and stores the inputs in a new array where each inputted argument is a single index in the array
		char *argv1[100];
        	int space_counter = 0;
        	char *space_locator = strtok(input, " ");
        	while (space_locator != NULL) {
            		argv1[space_counter++] = space_locator;
            		space_locator = strtok(NULL, " ");
        	}
        	argv1[space_counter++] = NULL;
		argv1[space_counter] = NULL;	
		//Terminates the program
        	if (strcmp(argv1[0], "exit") == 0) {
            		return 1;
        	}
		//Gives a list of functions that the user can input
		else if(strcmp(argv1[0], "functions") == 0){
			printf("\033[93m'help'\n"
			       "'undo'\n"
 			       "'rank'\n"
 			       "'check'\n"
 			       "'modify'\n"
 			       "'recover'\n"
			       "'switch'\n"
			       "'exit'\n"
			       "'info'\n"
			       "'visualize'\n"
			       "For more information on how to use each function type: 'help <function name>'.\033[0m\n"
        		);
		}

		//Gives the current savefile name, score, and lines
		else if (strcmp(argv1[0], "info") == 0){
			printf("\033[91mCurrent savefile: %s\033[0m\n", file_to_edit);
			printf("\033[91mScore: %d\033[0m\n", current_game_score);
			printf("\033[91mLines: %d\033[0m\n", current_game_lines);
		}

		//Explains how each function does and how to use it
		else if(strcmp(argv1[0], "help") == 0){
			if(argv1[1] == NULL){
                                printf("\033[93mPlease specify which function you would like help with: 'help <function name>'.\033[0m\n");
			}
			else if(strcmp(argv1[1], "undo") == 0){
                                printf("\033[93mThis command calls the 'undo' function which undoes the last modify you did on your file. You can run it by typing: 'undo', after you've modified a file.\033[0m\n");
			}
			else if(strcmp(argv1[1], "functions") == 0){
				printf("\033[93mThis command gives a list of functions that the user can input. It can be called by typing: 'functions'.\033[0m\n");
			}
			else if(strcmp(argv1[1], "rank") == 0){
                                printf("\033[93mThis commands calls the 'rank' program with the current file and compares your score to the top scores on the server and prints the number of games specified by the user from 1st place to last. If no arguements besides 'rank' are given, it will send your file to the leaderboard and print your filename and what it is ranked, as well as printing the names and ranks of files 3 below and 5 above your file. You can run rank by typing: 'rank <score/lines | optional> <number | optional>'.\033[0m\n");
			}
			else if(strcmp(argv1[1], "check") == 0){
				printf("\033[93mThis command calls the 'check' program and determines whether the current file is legitimate or illegitamate. You can run check by typing: 'check'.\033[0m\n");
			}
			else if(strcmp(argv1[1], "modify") == 0){
				printf("\033[93mThis command calls the 'modify' program where you can change your score or lines in the current file. You can call modify by typing: 'modify <score/lines> <number>'.\033[0m\n");
			}
			else if(strcmp(argv1[1], "recover") == 0){
				printf("\033[93mThis command calls the 'recover' program which, when given a path to a disk image, can recover deleted TetrisGameState save states as well as their score and lines. It will then ask you if you would like to switch to one of the recovered files. You can call recover by typing: 'recover <path to disk image>'.\033[0m\n"); 
			}
			else if(strcmp(argv1[1], "help") == 0){
                                printf("\033[93mYou already know how to use 'help'!\033[0m\n");
			}
			else if(strcmp(argv1[1], "switch") == 0){
				printf("\033[93mThis command calls the 'switch' function which allows you to switch the Tetris quicksave you are editing to another quicksave of the user's choice. You can call switch by typing: 'switch <Tetris quicksave file>'.\033[0m\n");
			}
			else if(strcmp(argv1[1], "info") == 0){
				printf("\033[93mThis command calls the 'info' functions which prints the current file name, score, and lines of the file being edited. It can be run by using: 'info'.\033[0m\n");
			}
			else if(strcmp(argv1[1], "visualize") == 0){
				printf("\033[93mThis command displays the game board of the current file that you are modifying. WARNING It is unstable, to get it off of your screen you MUST control+c and type reset (no letters will appear but you will be typing). To run the command type: 'visualize'.\033[0m\n");
			}
			else if(strcmp(argv1[1], "exit") == 0) {
				printf("\033[93mThis command exits the program. Type 'exit' and Tetrashell will close :(.\033[0m\n");
			}
			else{
				printf("\033[93mSorry! That command hasn't been created quite yet.\033[0m\n");
			}
		}
		//Switch takes the current file and argv1[1] arguments and prints the switching print statement, then it sets file_to_edit to be the save file that argv1[1] is. Once this is done successfully it will print: Switched!. If the file being switched to is invalid then an error will print after 'Switched!' prints. 
		else if(strcmp(argv1[0], "switch") == 0){
			printf("\033[91mSwitching current quicksave from '%s' to '%s'.\033[0m\n", file_to_edit, argv1[1]);
			strcpy(file_to_edit, argv1[1]);
			printf("\033[91mSwitched!\033[0m\n");
		}

		//Once modify has been run once, game2 is filled with its previous contents before it was edited, I take this data and write it back into file_to_edit which reverts it to it's previous state.
		else if(strcmp(argv1[0], "undo") == 0){
			FILE *fp3 = fopen(file_to_edit, "wb");
			if(fp3 == NULL){
				perror("Failed to open file");
				return 1;
			}
			fwrite(&game2, sizeof(TetrisGameState), 1, fp3);
		        fclose(fp3);
	 		printf("Previous changes undone.\n");		
		}

		else if(strcmp(argv1[0], "rank") == 0){ 
			if(argv1[1] == NULL){
                                argv1[1] = "score";
                        }
			char large_number[10];
			if(argv1[2] == NULL){
				sprintf(large_number, "%d", 100);
				argv1[2] = large_number;
			}
			char* path_of_file_to_rank = realpath(file_to_edit, NULL);
			argv1[3] = "uplink";
			argv1[4] = NULL;
			int pipefd[2] = {0};
			int pipefd1[2] = {0};
			if (pipe(pipefd) == -1){		
				perror( "Pipe error. \n");
				return 1;
			}
			if(pipe(pipefd1) == -1){
				perror("Pipe error. \n");
				return 1;
			}
			pid_t pid = fork();
			if (pid == -1){
				perror("Fork failed.\n");
				return 1;
			}
			else if (pid == 0){
				close(pipefd[0]);
				close(pipefd1[1]);
				dup2(pipefd1[0], fileno(stdin));
				close(pipefd1[0]);
                                dup2(pipefd[1], fileno(stdout));
				close(pipefd[1]);
                                execve("./rank", argv1, NULL);
                                perror("execve failed");
                                exit(1);
			}
			else{
				close(pipefd[1]);
				close(pipefd1[0]);
				char child_output[4096]; 
				write(pipefd1[1], path_of_file_to_rank, strlen(path_of_file_to_rank));
				close(pipefd1[1]);
				ssize_t bytes = 0;
				char** filepaths = malloc(sizeof(char*));
				filepaths[0] = malloc(sizeof(char)*4096);
				unsigned int child_idx = 0;
				unsigned int file_idx = 0;
				unsigned int file_size = 1; 
				while ((bytes = read(pipefd[0], child_output, 4096)) > 0) {
                                     for (ssize_t j = 0; j < bytes; ++j) {
                                        filepaths[file_idx][child_idx++] = child_output[j];
                                        if (child_output[j] == '\n') {
                                          filepaths[file_idx++][child_idx - 1] = 0;
                                          child_idx = 0;
                                          if (file_idx >= file_size) {
                                             file_size++;
                                             filepaths = realloc(filepaths, sizeof(char*)*file_size);
                                             filepaths[file_idx] = malloc(sizeof(char)*4096);
                                          }
                                        }
                                     }
				}
				close(pipefd[0]);
				int status;
				waitpid(pid, &status, 0);
				uid_t uid = getuid();
        			struct passwd *name = getpwuid(uid);
				int my_rank;
				const char *username = name->pw_name;
				char *last_slash = strrchr(path_of_file_to_rank, '/');
				int len = strlen(username) + strlen(last_slash) + 1;
				char *result = malloc(len);
				strcpy(result, username);
				strcat(result, "/");
				strcat(result, last_slash + 1);	
				for (int i = 0; i < file_size - 1; i++) {
    					if (strcmp(filepaths[i], result) == 0) {
        					my_rank = i;
        					break;
					}
				}
    				int start_idx = my_rank - 5;
    				if (start_idx < 0) {
        				start_idx = 0;
    				}
    				int end_idx = my_rank + 5;
    				if (end_idx >= file_size - 1) {
        				end_idx = file_size - 2;
    				}

    				for (int j = start_idx; j <= end_idx; j++) {
        				if (j == my_rank) {
            					printf("\033[93m\033[1m%d.   %s\033[0m\n", j + 1, filepaths[j]);
        				} 
					else {
            					printf("%d.   %s\n", j + 1, filepaths[j]);
        				}
    				}
			
			}	
		}		

		//Takes the inputted file and gives it to ./check which checks to see if the file is valid or not
		else if(strcmp(argv1[0], "check") == 0){
                        argv1[1] = file_to_edit;
			argv1[2] = NULL;
			pid_t pid = fork();
                        if (pid == -1) {
                                perror("Fork failed");
                                return 1;
                        }
                        else if (pid == 0) {
                                execve("./check", argv1, NULL);
                                perror("execve failed");
                                exit(1);
                        }
                        else{
                                int status;
                                waitpid(pid, &status, 0);
                        }
                }

		//First checks to see if the user has given a specific file in arguments, if not it uses the file inputted at the beginning of the program and reads its data and stores it in game2 before it's modified. If there is a given file in the arguments, we read into that file and store it in game2 then pass it into modify.
       		else if(strcmp(argv1[0], "modify") == 0){ 	
			if(argv1[3] == NULL){
				FILE *fp2 = fopen(file_to_edit, "rb");
                        	if (fp2 == NULL) {
                                	perror("Failed to open file");
                                	return 1;
                        	}
                		size_t read_size = fread(&game2, sizeof(TetrisGameState), 1, fp2);
                		fclose(fp2);
				argv1[3] = file_to_edit;
				argv1[4] = NULL;
			}
			else{
				FILE *fp2 = fopen(argv1[3], "rb");
                                TetrisGameState game2;
                                if (fp2 == NULL) {
                                        perror("Failed to open file");
                                        return 1;
                                }
                                size_t read_size = fread(&game2, sizeof(TetrisGameState), 1, fp2);
                                fclose(fp2);
			}
			pid_t pid = fork();
                        if (pid == -1) {
                                perror("Fork failed");
                                return 1;
                        } 
                        else if (pid == 0) {
                                execve("./modify", argv1, NULL);
                                perror("execve failed");
                                exit(1);
                        }
                        else{
			       	int status;
                                waitpid(pid, &status, 0);
                        }	
		}
		else if(strcmp(argv1[0], "visualize") == 0) {
                        TetrisGameState s;
                        TetrisDisplay windows;
                        argv1[1] = file_to_edit;
                        argv1[2] = NULL;
                        char* pathname = realpath(file_to_edit,NULL);
                        FILE* fp = fopen(file_to_edit,"rb");
                          if (fp == NULL) {
                                        perror("Failed to open file");
                                        return 1;
                                }

                        fread(&s,sizeof(TetrisGameState),1,fp);
                        fclose(fp);
                        pid_t pid = fork();
                        if(pid == 0) {
                                execvp("./tetris",argv1);
                                perror("execvp failed");
                                exit(1);
                        }
                        else if(pid == -1) {
                                perror("Fork failed");
                                return 1;
                        }
                        else {
                                int status=0;
                                waitpid(pid, &status, 0);
                        }
                        continue;
                }

		//Given a path to a disk image of deleted Tetris quicksaves recover will recover all valid and deleted save files from the path given.
		else if (strcmp(argv1[0], "recover") == 0) {
			int pipefd[2] = {0};
			if (pipe(pipefd) == -1){
				perror("Pipe error.\n");
				exit(1);
			}
			pid_t pid = fork();
			if (pid == -1) {
                		perror("Fork failed");
                		return 1;
            		} 
			else if (pid == 0) {
				close(pipefd[0]);
				dup2(pipefd[1], fileno(stdout));
				dup2(open("/dev/null", O_WRONLY), fileno(stderr));
                		execve("./recover", argv1, NULL);
                		perror("execve failed");
                		exit(1);
			}


			else{
				close(pipefd[1]);
				char child_output[4096];
				ssize_t bytes = 0;
				char** filenames = malloc(sizeof(char*));
				filenames[0] = malloc(sizeof(char)*4096);
				unsigned int child_idx = 0;
				unsigned int file_idx = 0;
				unsigned int file_size = 1;
				while ((bytes = read(pipefd[0], child_output, 4096)) > 0) {
				   
				     for (ssize_t j = 0; j < bytes; ++j) {
				        filenames[file_idx][child_idx++] = child_output[j];
				        if (child_output[j] == '\n') {
					  filenames[file_idx++][child_idx - 1] = 0;
					  child_idx = 0;
					  if (file_idx >= file_size) {
					     file_size++;
					     filenames = realloc(filenames, sizeof(char*)*file_size);
					     filenames[file_idx] = malloc(sizeof(char)*4096);
					  }
					}
				     }
				}
				close(pipefd[0]);
				int status;
				waitpid(pid, &status, 0);
				printf("Recovered Quicksaves:\n");
				 printf("-- --------------------- ------- ------- \n"
           "#  	 File path        Score   Lines  \n"
           "-- --------------------- ------- -------  \n");
				for (int i = 0; i < file_size - 1; i++) {
				   	FILE *fp4 = fopen(filenames[i], "rb");
                                	TetrisGameState game4;
                                	if (fp4 == NULL) {
                                        	perror("Failed to open file");
                                        	return 1;
                                	}
                                	size_t read_size = fread(&game4, sizeof(TetrisGameState), 1, fp4);
                                	fclose(fp4);
                                	int current_game_score1 = game4.score;
                                	int current_game_lines1 = game4.lines;
					printf("%d %s	 %d	  %d\n", i+1, filenames[i], current_game_score1, current_game_lines1);
						
				}
				char yes_no[3];
				char requested_file_num[10];
				int int_file_num;
				printf("Do you want to switch to one of these files? (y/n): ");
				fgets(yes_no, sizeof(yes_no), stdin);
				yes_no[strcspn(yes_no, "\n")] = 0;
				if(strcmp(yes_no, "y") == 0){
					printf("Which quicksave (enter a #): ");
					fgets(requested_file_num, sizeof(requested_file_num), stdin);
					requested_file_num[strcspn(requested_file_num, "\n")] = 0;
					int_file_num = atoi(requested_file_num) - 1;
					printf("\033[91mSwitching current quicksave from '%s' to '%s'.\033[0m\n", file_to_edit, filenames[int_file_num]);
                        	strcpy(file_to_edit, filenames[int_file_num]);
                        	printf("\033[91mSwitched!\033[0m\n");
                		}
			
			}
			
		}		
	
	}
}
