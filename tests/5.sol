ADD ACCOUNT ANumber:1000 ID:100 BrNumber:10
Successful
ADD ACCOUNT ANumber:1001 ID:100 BrNumber:10
Successful
ADD ACCOUNT ANumber:1002 ID:102 BrNumber:11
Successful
ADD ACCOUNT ANumber:1004 ID:104 BrNumber:14
Successful
ADD ACCOUNT ANumber:1007 ID:107 BrNumber:17
Successful
TRANSFER IDF:104 ANumberF:1004 IDT:107 ANumberT:1007
TID:2 Amount:200.00 Commission:10.30 BalanceT:200.00 BalanceF:-210.30
TRANSFER IDF:104 ANumberF:1004 IDT:108 ANumberT:1112
TID:3 Amount:200.00 Commission:10.30 BalanceT:20200.00 BalanceF:-420.60
TRANSFER IDF:104 ANumberF:1004 IDT:104 ANumberT:1004
Illegal parameters
TRANSFER IDF:104 ANumberF:1004 IDT:102 ANumberT:1002
TID:4 Amount:400.00 Commission:0.00 BalanceT:400.00 BalanceF:-820.60
TRANSFER IDF:100 ANumberF:1000 IDT:107 ANumberT:1007
TID:5 Amount:400.00 Commission:0.00 BalanceT:600.00 BalanceF:-400.00
TRANSFER IDF:100 ANumberF:1000 IDT:107 ANumberT:1007
TID:6 Amount:400.00 Commission:0.00 BalanceT:1000.00 BalanceF:-800.00
TRANSFER IDF:100 ANumberF:1000 IDT:107 ANumberT:1007
Not applicable
BALANCES ID:104 ANumber:1004
Current Balance: -820.60
 TID | Source | Amount    | Commission | Balance     |
 2   | DEBIT  | 200.00    | 10.30      | -210.30     |
 3   | DEBIT  | 200.00    | 10.30      | -420.60     |
 4   | DEBIT  | 400.00    | 0.00       | -820.60     |
BALANCES ID:107 ANumber:1007
Current Balance: 1000.00
 TID | Source | Amount    | Commission | Balance     |
 2   | CREDIT | 200.00    |            | 200.00      |
 5   | CREDIT | 400.00    |            | 600.00      |
 6   | CREDIT | 400.00    |            | 1000.00     |
