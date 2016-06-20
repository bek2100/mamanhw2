ADD ACCOUNT ANumber:1000 ID:100 BrNumber:10
Successful
ADD ACCOUNT ANumber:1001 ID:100 BrNumber:10
Successful
ADD ACCOUNT ANumber:1002 ID:102 BrNumber:11
Successful
TRANSFER IDF:100 ANumberF:1002 IDT:101 ANumberT:1001
Not applicable
TRANSFER IDF:101 ANumberF:1000 IDT:101 ANumberT:1000
Illegal parameters
TRANSFER IDF:10 ANumberF:1001 IDT:102 ANumberT:1002
Illegal parameters
TRANSFER IDF:100 ANumberF:1000 IDT:102 ANumberT:1002
Not applicable
ADD ACCOUNT ANumber:1002 ID:102 BrNumber:11
Illegal parameters
ADD ACCOUNT ANumber:1004 ID:104 BrNumber:14
Successful
ADD ACCOUNT ANumber:1007 ID:107 BrNumber:17
Successful
TRANSFER IDF:108 ANumberF:1112 IDT:104 ANumberT:1004
TID:2 Amount:10000.00 Commission:25.30 BalanceT:10000.00 BalanceF:9974.70
TRANSFER IDF:104 ANumberF:1004 IDT:102 ANumberT:1002
TID:3 Amount:10000.00 Commission:15.00 BalanceT:10000.00 BalanceF:-15.00
TRANSFER IDF:104 ANumberF:1004 IDT:107 ANumberT:1007
Not applicable
TRANSFER IDF:104 ANumberF:1004 IDT:102 ANumberT:1002
Not applicable
BALANCES ID:104 ANumber:1004
Current Balance: -15.00
 TID | Source | Amount    | Commission | Balance     |
 2   | CREDIT | 10000.00  |            | 10000.00    |
 3   | DEBIT  | 10000.00  | 15.00      | -15.00      |
BALANCES ID:108 ANumber:1112
Current Balance: 9974.70
 TID | Source | Amount    | Commission | Balance     |
 1   | DEBIT  | 10000.00  | 25.30      | 20000.00    |
 2   | DEBIT  | 10000.00  | 25.30      | 9974.70     |
