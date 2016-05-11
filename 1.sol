ADD ACCOUNT ANumber:1000 ID:100 BrNumber:10
Successful
ADD ACCOUNT ANumber:1001 ID:101 BrNumber:11
Successful
ADD ACCOUNT ANumber:1002 ID:102 BrNumber:12
Successful
ADD ACCOUNT ANumber:1003 ID:220 BrNumber:12
Illegal parameters
WITHDRAWAL ANumber:1000 ID:100 BrNumber:10
WID:0 Amount:100.00 Commission:3.80 Balance:-103.80
WITHDRAWAL ANumber:1000 ID:100 BrNumber:10
Not applicable
WITHDRAWAL ANumber:1111 ID:109 BrNumber:10
WID:1 Amount:10000.00 Commission:15.00 Balance:19985.00
TRANSFER IDF:100 ANumberF:1000 IDT:101 ANumberT:1001
TID:0 Amount:100.13 Commission:10.30 BalanceT:100.13 BalanceF:-214.23
TRANSFER IDF:101 ANumberF:1001 IDT:100 ANumberT:1000
TID:1 Amount:200.00 Commission:10.30 BalanceT:-14.23 BalanceF:-110.17
MONEY LAUNDERING
Empty
TRANSFER IDF:101 ANumberF:1001 IDT:102 ANumberT:1002
TID:2 Amount:50.00 Commission:10.30 BalanceT:50.00 BalanceF:-170.47
TRANSFER IDF:102 ANumberF:1002 IDT:101 ANumberT:1001
TID:3 Amount:50.00 Commission:10.30 BalanceT:-120.47 BalanceF:-10.30
ASSOCIATES 101
100
101
102
MONEY LAUNDERING
101
BALANCES ID:100 ANumber:1000
Current Balance: -14.23
 TID | Source | Amount    | Commission | Balance     |
 0   | DEBIT  | 100.00    | 3.80       | -103.80     |
 0   | DEBIT  | 100.13    | 10.30      | -214.23     |
 1   | CREDIT | 200.00    |            | -14.23      |
BALANCES ID:101 ANumber:1001
Current Balance: -120.47
 TID | Source | Amount    | Commission | Balance     |
 0   | CREDIT | 100.13    |            | 100.13      |
 1   | DEBIT  | 200.00    | 10.30      | -110.17     |
 2   | DEBIT  | 50.00     | 10.30      | -170.47     |
 3   | CREDIT | 50.00     |            | -120.47     |
BALANCES ID:109 ANumber:1111
Current Balance: 19985.00
 TID | Source | Amount    | Commission | Balance     |
 1   | DEBIT  | 10000.00  | 15.00      | 19985.00    |
