ADD ACCOUNT ANumber:1100 ID:100 BrNumber:10
Successful
ADD ACCOUNT ANumber:1101 ID:100 BrNumber:11
Successful
ADD ACCOUNT ANumber:1001 ID:101 BrNumber:11
Successful
ADD ACCOUNT ANumber:1002 ID:102 BrNumber:12
Successful
ADD ACCOUNT ANumber:1003 ID:103 BrNumber:13
Successful
TRANSFER IDF:100 ANumberF:1100 IDT:102 ANumberT:1002
TID:2 Amount:5.00 Commission:10.30 BalanceT:5.00 BalanceF:-15.30
TRANSFER IDF:100 ANumberF:1101 IDT:103 ANumberT:1003
TID:3 Amount:5.00 Commission:10.30 BalanceT:5.00 BalanceF:-15.30
TRANSFER IDF:100 ANumberF:1101 IDT:101 ANumberT:1001
TID:4 Amount:5.00 Commission:10.30 BalanceT:5.00 BalanceF:-30.60
ASSOCIATES 100
100
101
102
103
ASSOCIATES 101
100
101
102
103