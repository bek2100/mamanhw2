ADD ACCOUNT ANumber:1000 ID:100 BrNumber:10
Successful
ADD ACCOUNT ANumber:1001 ID:100 BrNumber:10
Successful
ADD ACCOUNT ANumber:1002 ID:100 BrNumber:11
Successful
ADD ACCOUNT ANumber:1003 ID:103 BrNumber:13
Successful
WITHDRAWAL ANumber:1000 ID:100 BrNumber:10
Not applicable
WITHDRAWAL ANumber:1000 ID:100 BrNumber:20
Illegal parameters
WITHDRAWAL ANumber:1000 ID:200 BrNumber:10
Illegal parameters
WITHDRAWAL ANumber:1003 ID:100 BrNumber:10
Not applicable
WITHDRAWAL ANumber:1000 ID:100 BrNumber:10
WID:0 Amount:100.00 Commission:3.80 Balance:-103.80
WITHDRAWAL ANumber:1000 ID:100 BrNumber:11
WID:1 Amount:890.55 Commission:5.65 Balance:-1000.00
WITHDRAWAL ANumber:1000 ID:100 BrNumber:10
Illegal parameters
WITHDRAWAL ANumber:1000 ID:100 BrNumber:10
Not applicable
BALANCES ID:100 ANumber:1000
Current Balance: -1000.00
 TID | Source | Amount    | Commission | Balance     |
 0   | DEBIT  | 100.00    | 3.80       | -103.80     |
 1   | DEBIT  | 890.55    | 5.65       | -1000.00    |
WITHDRAWAL ANumber:1111 ID:109 BrNumber:17
WID:2 Amount:10000.00 Commission:15.00 Balance:19985.00
WITHDRAWAL ANumber:1112 ID:108 BrNumber:15
WID:3 Amount:20000.00 Commission:30.00 Balance:-30.00
BALANCES ID:109 ANumber:1111
Current Balance: 19985.00
 TID | Source | Amount    | Commission | Balance     |
 1   | CREDIT | 10000.00  |            | 30000.00    |
 2   | DEBIT  | 10000.00  | 15.00      | 19985.00    |
