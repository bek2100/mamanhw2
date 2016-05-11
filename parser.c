#include <stdio.h>
#include <string.h>
#include "wet.h"
#include <stdlib.h>
#include <stdbool.h>


#define MAX_LINE_SIZE 128

char	command_name[MAX_LINE_SIZE + 2];
char	command_line[MAX_LINE_SIZE + 2];
char*	arguments;
int     missing_argc;
int     argc;



void goto_next_line(FILE* file)
{
    while (!feof(file) && fgetc(file) != '\n');
}

int check_command(char* expected_name) 
{
    return (!strcmp(command_name,expected_name));
}

int check_arguments(int expected_argc) 
{
    return ((missing_argc = expected_argc - argc) <= 0);
}

void illegalCommand() {
    printf("Illegal command!\n");
}

void parseInput() {
	bool exit = false;
    while ((!feof(stdin))&&(!exit)) {
        if (fgets(command_line,MAX_LINE_SIZE + 2,stdin) == NULL) break;
                
        if (strlen(command_line) > MAX_LINE_SIZE) 
        {
            printf("Command too long, can not parse\n");
            goto_next_line(stdin);
        }
        else 
        {
            missing_argc = 0;
            sprintf(command_name,"");
            sscanf(command_line,"%s",command_name);
			arguments=command_line + strlen(command_name) + 1;
                 
            if (check_command(COMMAND_ADD_ACCOUNT)) {
            	int ID, ANumber, BrNumber;
                argc = sscanf(arguments, "%d %d %d", &ID, &ANumber, &BrNumber);
                check_arguments(3) ? addAccount(ID, ANumber, BrNumber) : illegalCommand();
            } else if (check_command(COMMAND_WITHDRAWAL)) {
            	int ID, ANumber, BrNumber;
            	double WAmount;
            	argc = sscanf(arguments, "%lf %d %d %d", &WAmount, &BrNumber, &ID, &ANumber);
                check_arguments(4) ? withdraw(WAmount, BrNumber, ID, ANumber) : illegalCommand();
            } else if (check_command(COMMAND_TRANSFER)){
            	int IDF, IDT, ANumberF, ANumberT;
            	double TAmount;
            	argc = sscanf(arguments, "%lf %d %d %d %d", &TAmount, &IDF, &ANumberF, &IDT, &ANumberT);
                check_arguments(5) ? transfer(TAmount, IDF, ANumberF, IDT, ANumberT) : illegalCommand();
            } else if(check_command(COMMAND_BALANCES)) {
            	int ID, ANumber;
                argc = sscanf(arguments, "%d %d", &ID, &ANumber);
                check_arguments(2) ? balances(ID, ANumber) : illegalCommand();
            } else if (check_command(COMMAND_ASSOCIATES)) {
            	int ID;
                argc = sscanf(arguments, "%d", &ID);
                check_arguments(1) ? associates(ID) : illegalCommand();
            } else if (check_command(COMMAND_MONEY_LAUNDERING)) {
            	moneyLaundering();
            } else if (missing_argc > 0) {
                printf("Missing %d argument(s) in command %s!\n",missing_argc,command_name);
            } else if (argc > 0) {
                illegalCommand();
            } else {
                illegalCommand();
            }
        }
    }
}
