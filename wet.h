#ifndef __WET_H_
#define __WET_H_

#include <stdbool.h>


/*==========================================================*/
/*  -----  DBMS course, 236363, Spring - 2016  ----- */
/*==========================================================*/
/* This is a sample header file for the "wet" assignment.   */
/* This file should not be submitted, it'll be replaced     */
/* during the automatic tests. Therefore, DO NOT change     */
/* anything in this file except your USERNAME and PASSWORD. */

/* Update these two macros to your account's details */
/* USERNAME = t2 user, PASSWORD = your id number     */
#define USERNAME "sbeccaba"
#define PASSWORD "311767750"

/* DO NOT change the names of the macros -      */
/* your submitted program will fail compilation */
#define ILL_PARAMS              "Illegal parameters\n"
#define EMPTY                   "Empty\n"
#define SUCCESSFUL              "Successful\n"
#define NOT_APPLICABLE			"Not applicable\n"

/* Function names */
#define COMMAND_ADD_ACCOUNT				"add_account"
#define COMMAND_WITHDRAWAL				"withdrawal"
#define COMMAND_TRANSFER             	"transfer"
#define COMMAND_MONEY_LAUNDERING		"money_laundering"
#define COMMAND_ASSOCIATES    			"associates"
#define COMMAND_BALANCES    			"balances"

#define ADD_ACCOUNT			"ADD ACCOUNT ANumber:%d ID:%d BrNumber:%d\n"
#define WITHDRAWAL      	"WITHDRAWAL ANumber:%d ID:%d BrNumber:%d\n"
#define WITHDRAWAL_SUCCESS	"WID:%d Amount:%.2f Commission:%.2f Balance:%.2f\n"
#define TRANSFER      		"TRANSFER IDF:%d ANumberF:%d IDT:%d ANumberT:%d\n"
#define TRANSFER_SUCCESS	"TID:%d Amount:%.2f Commission:%.2f BalanceT:%.2f BalanceF:%.2f\n"
#define BALANCES		 	"BALANCES ID:%d ANumber:%d\n"
#define CURRENT_BALANCES 	"Current Balance: %.2f\n"
#define MONEY_LAUNDERING 	"MONEY LAUNDERING\n"
#define ASSOCIATES			"ASSOCIATES %d\n"


#define BALANCES_HEADER   	" TID | Source | Amount    | Commission | Balance     |\n"

#define DEBIT_RESULT   		" %-4s| DEBIT  | %-10.2f| %-11.2f| %-12.2f|\n"
#define CREDIT_RESULT   	" %-4s| CREDIT | %-10.2f|            | %-12.2f|\n"

// This function is implemented in the parser.c file.
void parseInput();

// The functions you have to implement:
// All of the functions gets strings because we don't want you to process this input
// in the C language, but forward it as is to the SQL.
void* addAccount(int ANumber, int ID, int BrNumber);
void* withdraw(double WAmount, int BrNumber, int ID, int ANumber);
void* transfer(double TAmount, int IDF, int ANumberF, int IDT, int ANumberT);
void* balances(int ID, int ANumber);
void* associates(int ID);
void* moneyLaundering();

#endif  /* __WET_H_ */