#include "wet.h"
#include "libpq-fe.h"
#include <stdio.h>
#include <stdlib.h>


// This is the global connection object
PGconn     *conn;
PGresult    *res;

#define CMD_SIZE 550

/*************************************************************************************/

void PrepareForTest1();

int main(int argc, char** argv) {
    /* Make a connection to the DB. If parameters omitted, default values are used */
    char connect_param[CMD_SIZE + 2];
    sprintf(connect_param,"host=pgsql.cs.technion.ac.il dbname=%s user=%s password=%s",
            USERNAME, USERNAME, PASSWORD);
    conn = PQconnectdb(connect_param);
    if (!conn || PQstatus(conn) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to server failed: %s\n",
                PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }
    
    char cmd[CMD_SIZE];
    
    /*sprintf(cmd, "DROP TABLE Account; DROP TABLE OwnsAcc; DROP TABLE Withdrawal; DROP TABLE ManagesAcc; DROP TABLE Transfer; DROP TABLE Branch;");
    
    res = PQexec(conn, cmd);
    
    if(!res) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return 1; }
    
    sprintf(cmd, "CREATE TABLE Account AS SELECT * FROM course_Account; CREATE TABLE OwnsAcc AS SELECT * FROM course_OwnsAcc; CREATE TABLE Withdrawal AS SELECT * FROM course_Withdrawal; CREATE TABLE ManagesAcc AS SELECT * FROM course_ManagesAcc; CREATE TABLE Transfer AS SELECT * FROM course_Transfer; CREATE TABLE Branch AS SELECT * FROM course_Branch;");
    
    res = PQexec(conn, cmd);
    
    if(!res) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return 1; }
    */
    
    PrepareForTest1();
    
    parseInput();

    PQfinish(conn);
    
    return 0;
}

/*************************************************************************************/
// The functions you have to implement

/*
 1. הוספת חשבון
 add_account ANumber ID BrNumber קלט
 תיאור הפעולה הפעולה תוסיף לטבלת Accounts חשבון עם מזהה ANumber, יתרה 0 ומסגרת
 מינוס )1000 )Overdraft-. בנוסף הפעולה תעדכן את ManagesAcc ו-
 OwnsAcc בהתאם.
 הפעולה נכשלת אם מזהה הלקוח או מס' הסניף אינם קיימים במערכת או מספר
 החשבון כבר קיים במערכת.
 פלט בשורה ראשונה יודפס:
 ADD ACCOUNT ANumber:ANumber ID:ID BrNumber:BrNumber
 בשורה שנייה:
 במקרה של הצלחה, יש להדפיס את הקבוע SUCCESSFUL.
 במקרה של כישלון, יש להדפיס את הקבוע ILL_PARAMS.
 
 */


void* addAccount(int ANumber, int ID, int BrNumber) {
    
    printf(ADD_ACCOUNT, ANumber, ID, BrNumber);
    
    char cmd[CMD_SIZE];
    
    
    sprintf(cmd, "SELECT ID FROM Customer WHERE ID=%d;", ID);
    
    res = PQexec(conn, cmd);
    
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    
    if(PQntuples(res) == 0) {
        printf(ILL_PARAMS);
        PQclear(res); return NULL;
    }
    
    PQclear(res);
    
    sprintf(cmd, "SELECT BrNumber FROM Branch WHERE BrNumber=%d;", BrNumber);
    
    res = PQexec(conn, cmd);
    
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    if(PQntuples(res) == 0) {
        printf(ILL_PARAMS);
        PQclear(res); return NULL;
    }
    
    PQclear(res);
    sprintf(cmd, "SELECT ANumber FROM Account WHERE ANumber=%d;", ANumber);
    
    res = PQexec(conn, cmd);
    
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    if(PQntuples(res) != 0) {
        printf(ILL_PARAMS);
        PQclear(res); return NULL;
    }
    
    PQclear(res);
    
    sprintf(cmd, "INSERT INTO Account VALUES(%d,%d,%d);", ANumber, 0, -1000);
    
    res = PQexec(conn, cmd);
    
    if(!res) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    
    PQclear(res);
    
    sprintf(cmd, "INSERT INTO OwnsAcc VALUES(%d,%d);", ID, ANumber);
    
    res = PQexec(conn, cmd);
    
    if(!res) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    
    PQclear(res);
    
    sprintf(cmd, "INSERT INTO ManagesAcc VALUES(%d,%d);", BrNumber, ANumber);
    
    res = PQexec(conn, cmd);
    
    if(!res) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    
    printf(SUCCESSFUL);
    
    PQclear(res); return NULL;
}


/*
 
 2. פעולת משיכה
 withdrawal WAmount BrNumber ID ANumber קלט
 תיאור הפעולה הפעולה תעדכן את טבלת המשיכות עם המשיכה החדשה. מזהה המשיכה יהיה
 המזהה המקסימאלי בטבלת המשיכות ועוד 1. אם טבלת המשיכות ריקה המזהה
 3
 יהיה 0. זמן הפעולה יהיה timestamp הנוכחי.
 העמלה על הפעולה תחושב באופן הבא:
 i. אם סכום המשיכה 10000₪ ומעלה, %0.15 משווי העסקה.
 ii. )אחרת( בסניף בו מתנהל החשבון, 3.8 ₪ לפעולה.
 iii. )אחרת( בסניף אחר, 5.65 ₪ לפעולה.
 הטבלה Account תעודכן עם היתרה החדשה.
 הפעולה נכשלת אם:
 1. מזהה הלקוח, מס' הסניף או מספר החשבון אינם קיימים במערכת, או סכום
 המשיכה קטן או שווה ל-0.
 2. החשבון אינו שייך ללקוח או סכום המשיכה כולל העמלה גורם לחריגה ממסגרת
 המינוס )Overdraft(. חריגה ממש ולא שוויון.
 פלט בשורה ראשונה יודפס:
 WITHDRAWAL ANumber:ANumber ID:ID BrNumber:BrNumber
 בשורה שנייה:
 במקרה של הצלחה, יש להדפיס:
 WID:WID Amount:WAmount Commission:WCommission Balance:Balance
 כאשר Balance היא היתרה אחרי ביצוע הפעולה ו- WCommission היא
 העמלה שנגבתה על הפעולה.
 במקרה של כישלון מסוג 1, יש להדפיס ILL_PARAMS.
 במקרה של כישלון מסוג 2, יש להדפיס NOT_APPLICABLE
 
 */


void* withdraw(double WAmount, int BrNumber, int ID, int ANumber) {
    
    printf(WITHDRAWAL, ANumber, ID, BrNumber);
    
    char cmd[CMD_SIZE];
    
    if (WAmount <= 0) {
        printf(ILL_PARAMS);
        return NULL;
    }
    
    sprintf(cmd, "SELECT ID FROM Customer WHERE ID=%d;", ID);
    
    res = PQexec(conn, cmd);
    
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    if(PQntuples(res) == 0) {
        printf(ILL_PARAMS);
        PQclear(res);
        return NULL;
    }
    
    PQclear(res);
    sprintf(cmd, "SELECT BrNumber FROM Branch WHERE BrNumber=%d;", BrNumber);
    
    res = PQexec(conn, cmd);
    
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    if(PQntuples(res) == 0) {
        printf(ILL_PARAMS);
        PQclear(res);
        return NULL;
    }
    
    PQclear(res);
    sprintf(cmd, "SELECT ANumber FROM Account WHERE ANumber=%d;", ANumber);
    
    res = PQexec(conn, cmd);
    
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    if(PQntuples(res) == 0) {
        printf(ILL_PARAMS);
        PQclear(res);
        return NULL;
    }
    
    PQclear(res);
    
    sprintf(cmd, "SELECT * FROM OwnsAcc WHERE ANumber=%d AND ID=%d;", ANumber, ID);
    
    res = PQexec(conn, cmd);
    
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    
    if(!PQntuples(res)) {
        printf(NOT_APPLICABLE);
        PQclear(res); return NULL;
    }
    
    
    double WCommission = 0;
    
    if (WAmount >= 10000) WCommission = 0.0015 * WAmount;
    else{
        PQclear(res);  sprintf(cmd, "SELECT BrNumber FROM ManagesAcc WHERE ANumber=%d;", ANumber);
        res = PQexec(conn, cmd);
        if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
        if(atoi(PQgetvalue(res, 0, 0)) == BrNumber) WCommission += 3.8;
        else WCommission += 5.65;
    }
    
    PQclear(res);
    
    sprintf(cmd, "SELECT * FROM Account WHERE ANumber=%d;", ANumber);
    
    res = PQexec(conn, cmd);
    
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    
    double Balance = 0;
    
    if((Balance = (atof(PQgetvalue(res, 0, 1)) - (WCommission + WAmount))) <= atoi(PQgetvalue(res, 0, 2))){
        printf(NOT_APPLICABLE);
        PQclear(res);
        return NULL;
    }
    
    PQclear(res);
    sprintf(cmd, "SELECT MAX(WID),COUNT(WID) FROM Withdrawal;");
    
    res = PQexec(conn, cmd);
    
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    
    int WID =0;
    
    if (atoi(PQgetvalue(res, 0, 1)) == 0 ) WID = 0;
    else WID = atoi(PQgetvalue(res, 0, 0)) + 1;
    
    PQclear(res);
    
    sprintf(cmd, "INSERT INTO Withdrawal VALUES (%d, %lf, %lf, %d, %d, %d);", WID, WAmount, WCommission, BrNumber, ANumber, ID);
    
    res = PQexec(conn, cmd);
    
    if(!res) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    
    PQclear(res);
    
    sprintf(cmd, "UPDATE Account SET Balance = %lf WHERE ANumber = %d;", Balance, ANumber);
    
    res = PQexec(conn, cmd);
    
    if(!res) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    
    
    printf(WITHDRAWAL_SUCCESS, WID, WAmount, WCommission, Balance);
    PQclear(res); return NULL;
}

/*
 
 3. פעולת העברה
 transfer TAmount IDF ANumberF IDT ANumberT קלט
 תיאור הפעולה הפקודה מייצגת פעולת העברה שיש לבצע מהמפקיד )IDF,ANumberF( למוטב
 )IDT,ANumberT(. הפעולה תעדכן את טבלת ההעברות עם ההעברה החדשה.
 מזהה ההעברה יהיה המזהה המקסימאלי בטבלת ההעברות ועוד 1. אם טבלת
 ההעברות ריקה אז המזהה יהיה 0. זמן הפעולה יהיה timestamp הנוכחי.
 העמלה על הפעולה נלקחת מחשבון המפקיד, ותחושב באופן הבא:
 10.30 .i ₪ להעברה. אבל, במקרה והמפקיד-IDF קרוב משפחה של
 המוטב-IDT )יש להם אותו שם משפחה(, יש פטור מעמלה זו.
 ii. )בנוסף( אם סכום ההפקדה 10000₪ ומעלה, %0.15 משווי
 העסקה.
 הטבלה Account של המפקיד ושל המוטב תעודכן עם היתרה החדשה.
 הפעולה נכשלת אם:
 1. ת"ז של הלקוחות או מספרי החשבונות אינם קיימים במערכת, או המוטב זהה
 למפקיד, או סכום ההעברה קטן או שווה ל-0.
 2. אם החשבונות אינם שייכים ללקוחות בהתאם, או סכום ההעברה כולל העמלה
 גורם לחריגה ממסגרת המינוס )Overdraft( של המפקיד.
 פלט בשורה ראשונה יודפס:
 TRANSFER IDF:IDF ANumberF:ANumberF IDT:IDT ANumberT:ANumberT
 4
 בשורה שנייה:
 במקרה של הצלחה, יש להדפיס:
 TID:TID Amount:TAmount Commission:TCommission BalanceT:BalanceT
 BalanceF:BalanceF
 כאשר TCommission היא העמלה שנגבתה על הפעולה, BalanceT היא
 היתרה בחשבון ANumberT של המוטב אחרי ביצוע הפעולה, ו- BalanceF
 היא היתרה בחשבון ANumberF של המפקיד אחרי ביצוע הפעולה.
 במקרה של כישלון מסוג 1, יש להדפיס ILL_PARAMS.
 במקרה של כישלון מסוג 2, יש להדפיס NOT_APPLICABLE
 
 */

void* transfer(double TAmount, int IDF, int ANumberF, int IDT, int ANumberT) {
    
    char cmd[CMD_SIZE];
    
    printf(TRANSFER, IDF, ANumberF, IDT,ANumberT);
    
    if (TAmount <= 0 || IDF == IDT) {
        printf(ILL_PARAMS);
        return NULL;
    }
    
    
    sprintf(cmd, "SELECT ID FROM Customer WHERE ID=%d", IDF);
    
    res = PQexec(conn, cmd);
    
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    if(PQntuples(res) == 0) {
        printf(ILL_PARAMS);
        PQclear(res);
        return NULL;
    }
    
    PQclear(res);
    
    sprintf(cmd, "SELECT ID FROM Customer WHERE ID=%d", IDT);
    
    res = PQexec(conn, cmd);
    
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    if(PQntuples(res) == 0) {
        printf(ILL_PARAMS);
        PQclear(res); return NULL;
    }
    
    PQclear(res);
    
    sprintf(cmd, "SELECT * FROM OwnsAcc WHERE (ANumber=%d AND ID = %d) OR (ANumber=%d AND ID = %d)", ANumberT, IDT, ANumberF, IDF);
    
    res = PQexec(conn, cmd);
    
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    if(PQntuples(res) != 2) {
        printf(NOT_APPLICABLE);
        PQclear(res); return NULL;
    }
    
    double TCommission = 0;
    
    
    PQclear(res);
    
    sprintf(cmd, "SELECT COUNT(Family) FROM (SELECT SPLIT_PART(Name,' ',2) AS Family FROM (SELECT *  FROM Customer WHERE ID=%d OR ID=%d)AS IDS) AS FAM GROUP BY Family" , IDT, IDF);
    
    res = PQexec(conn, cmd);
    
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    
    if(atoi(PQgetvalue(res, 0, 0)) != 2) TCommission = 10.3;
    
    if(TAmount>=10000) TCommission+= 0.0015*TAmount;
    
    
    PQclear(res);
    
    sprintf(cmd, "SELECT * FROM Account WHERE ANumber=%d OR ANumber=%d ORDER BY ANumber", ANumberT, ANumberF);
    
    res = PQexec(conn, cmd);
    
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    
    int i = (ANumberT>ANumberF)? 0:1;
    int j = 1-i;
    

    double BalanceF = atof(PQgetvalue(res, i, 1)) - (TCommission + TAmount);
    
    if(BalanceF <= atof(PQgetvalue(res, i, 2))){
        printf(NOT_APPLICABLE);
        PQclear(res);
        return NULL;
    }
    
    double BalanceT = atof(PQgetvalue(res, j, 1)) + TAmount;

    
    PQclear(res);
    
    
    sprintf(cmd, "UPDATE Account SET Balance = %lf WHERE ANumber = %d; UPDATE Account SET Balance = %lf WHERE ANumber = %d;", BalanceT, ANumberT, BalanceF, ANumberF);
    
    res = PQexec(conn, cmd);
    
    if(!res) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    
    PQclear(res);
    
    sprintf(cmd, "SELECT MAX(TID),COUNT(TID) FROM Transfer;");
    
    res = PQexec(conn, cmd);
    
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    
    int TID =0;
    
    if (atoi(PQgetvalue(res, 0, 1)) == 0 ) TID = 0;
    else TID = atoi(PQgetvalue(res, 0, 0)) + 1;
    
    PQclear(res);
    sprintf(cmd, "INSERT INTO Transfer VALUES(%d, %lf, %lf, %d, %d, %d, %d);", TID, TAmount, TCommission, ANumberF, IDF, ANumberT, IDT);
    
    res = PQexec(conn, cmd);
    
    if(!res) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    
    printf(TRANSFER_SUCCESS, TID, TAmount, TCommission, BalanceT, BalanceF);
    PQclear(res); return NULL;
}

/*
 
 4. דף תנועות בחשבון
 balances ID ANumber קלט
 תיאור הפעולה הפעולה תדפיס את פרוט הפעולות של הלקוח ID בעל חשבון מס' ANumber.
 הפלט יכלול את המשיכות וההעברות שבהן היה מעורב הלקוח, וימוין בסדר עולה
 לפי זמן ביצוע הפעולה.
 הפעולה נכשלת אם:
 1. ת"ז של הלקוח או מספר החשבון אינם קיימים במערכת.
 2. החשבון אינו בבעלות הלקוח.
 פלט בשורה ראשונה יודפס:
 BALANCES ID:ID ANumber:ANumber
 בשורה שנייה:
 במקרה של כישלון מסוג 1, יש להדפיס ILL_PARAMS ולסיים.
 במקרה של כישלון מסוג 2, יש להדפיס NOT_APPLICABLE ולסיים.
 במקרה של הצלחה,
 Current Balance: CBalance
 כאשר CBalance היא היתרה הנוכחית.
 בשורה שלישית תודפס הכותרת )במדויק, כולל רווחים(:
 TID | Source | Amount | Commission | Balance |
 בשורות הבאות יודפסו הפעולות הבנקאיות בסדר עולה לפי זמן ביצוע הפעולה
 באופן הבא:
 לכל משיכה או העברה שבה הלקוח היה המפקיד יודפס:
 TID | DEBIT | Amount | Commission | Balance
 printf(" %-4s| DEBIT | %-10.2f| %-11.2f| %-12.2f|\n",…) י"ע
 לכל העברה שבה הלקוח היה המוטב יודפס:
 TID | CREDIT | Amount | | Balance
 printf(" %-4s| CREDIT | %-10.2f| | %-12.2f|\n,…) "ע
 כאשר TID הוא מזהה הפעולה, Amount ו-Commission סכומים ממשיים, ו-
 Balance היא היתרה לאחר ביצוע הפעולה.
 אם הלקוח לא ביצע אף פעולה יש להדפיס את הקבוע EMPTY.
 
 */

void* balances(int ID, int ANumber) {
    
    printf(BALANCES, ID, ANumber);
    
    char cmd[CMD_SIZE];
    
    
    sprintf(cmd, "SELECT * FROM Customer WHERE ID=%d", ID);
    
    res = PQexec(conn, cmd);
    
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    if(PQntuples(res) == 0) {
        printf(ILL_PARAMS);
        PQclear(res); return NULL;
    }
    
    PQclear(res);
    
    sprintf(cmd, "SELECT * FROM Account WHERE ANumber=%d", ANumber);
    
    res = PQexec(conn, cmd);
    
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    if(PQntuples(res) == 0) {
        printf(ILL_PARAMS);
        PQclear(res); return NULL;
    }
    
    double CBalance = atof(PQgetvalue(res, 0, 1));
    
    PQclear(res);
    
    printf(CURRENT_BALANCES, CBalance);
    
    sprintf(cmd, "SELECT * FROM OwnsAcc WHERE ANumber=%d AND ID = %d", ANumber, ID);
    
    res = PQexec(conn, cmd);
    
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    if(PQntuples(res) == 0) {
        printf(NOT_APPLICABLE);
        PQclear(res); return NULL;
    }
    
    
    PQclear(res);
    
    
    sprintf(cmd, "SELECT WID, WAmount, WCommission, WTime, 0 AS type, WCommission + WAmount AS diff FROM Withdrawal WHERE ANumber = %d "" UNION ALL "" SELECT TID, TAmount, TCommission, TTime,  0 AS type, TCommission + TAmount AS diff FROM Transfer Where ANumberF = %d "" UNION ALL "" SELECT TID, TAmount, TCommission, TTime,  1 AS type,  - TAmount AS diff FROM Transfer Where ANumberT = %d ""ORDER BY WTime ", ANumber, ANumber, ANumber);
    
    res = PQexec(conn, cmd);
    
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    
    int t_num = PQntuples(res);
    
    int i;
    
    PGresult *sum;
    
    sprintf(cmd, "SELECT SUM(diff) FROM (SELECT WID, WAmount, WCommission, WTime, 0 AS type, WCommission + WAmount AS diff FROM Withdrawal WHERE ANumber = %d "" UNION ALL "" SELECT TID, TAmount, TCommission, TTime,  0 AS type, TCommission + TAmount AS diff FROM Transfer Where ANumberF = %d "" UNION ALL "" SELECT TID, TAmount, TCommission, TTime,  1 AS type, 0-TAmount AS diff FROM Transfer Where ANumberT = %d) AS A", ANumber, ANumber, ANumber);
    
    sum = PQexec(conn, cmd);
    
    if(!sum || PQresultStatus(sum) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(sum)); return NULL; }

    
    double Diff= CBalance + atof(PQgetvalue(sum, 0, 0));
    PQclear(sum);
    
    
    if(t_num == 0)
        printf(EMPTY);
    else {
        printf(BALANCES_HEADER);
        for(i = 0; i < t_num; i++){
            Diff-=atof(PQgetvalue(res, i, 5));
            if(atof(PQgetvalue(res, i, 4)))
                printf(CREDIT_RESULT, PQgetvalue(res, i, 0), atof(PQgetvalue(res, i, 1)), Diff);
            else printf(DEBIT_RESULT, PQgetvalue(res, i, 0), atof(PQgetvalue(res, i, 1)), atof(PQgetvalue(res, i, 2)), Diff);
        }
    }
    
    
    
    
    PQclear(res); return NULL;
    
    
}

/*
 
 5. שותפים עסקיים
 associates ID קלט
 תיאור הפעולה הפעולה תדפיס את ת"ז של השותפים העסקיים של הלקוח ID.
 ID1 הוא שותף עסקי של ID אם נרשמה פעולת העברה שמערבת את שניהם, או,
 אם נרשמה פעולת העברה שמערבת את ID1 ואת ID2, ול-ID2 קיימת פעולת
 העברה שמערבת אותו עם ID. )חברים + חברים של חברים(
 הפעולה נכשלת אם ת"ז של הלקוח אינה קיימת במערכת.
 פלט בשורה ראשונה יודפס:
 ASSOCIATES ID
 במקרה של הצלחה, יודפסו מספרי ת"ז של השותפים העסקיים של ID בשורות
 נפרדות ממוינים בסדר עולה. אם ל-ID אין שותפים עסקיים יודפס הקבוע
 .EMPTY
 במקרה של כישלון יודפס הקבוע ILL_PARAMS.
 
 */

void* associates(int ID) {
    
    printf(ASSOCIATES, ID);
    
    char cmd[CMD_SIZE];
    
    sprintf(cmd, "SELECT ID FROM Customer WHERE ID=%d", ID);
    
    res = PQexec(conn, cmd);
    
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    if(PQntuples(res) == 0) {
        printf(ILL_PARAMS);
        PQclear(res); return NULL;
    }
    
    PQclear(res);
    
    sprintf(cmd, "SELECT T.IDF FROM (SELECT T.IDF FROM Transfer T WHERE EXISTS(SELECT T1.IDF FROM (SELECT IDF FROM Transfer WHERE IDT=%d UNION SELECT IDT FROM Transfer WHERE IDF=%d) T1 WHERE T.IDT=T1.IDF UNION SELECT T.IDT FROM Transfer T WHERE EXISTS(SELECT T1.IDF FROM (SELECT IDF FROM Transfer WHERE IDT=%d UNION SELECT IDT FROM Transfer WHERE IDF=%d) T1 WHERE T.IDF=T1.IDF)ORDER BY T.IDF", ID, ID,ID, ID);
    
    res = PQexec(conn, cmd);
    
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    
    int t_num = PQntuples(res);
    
    int i;
    
    if(!t_num) printf(EMPTY);
    else{
        for(i=0; i<t_num;i++)
            printf("%d\n", atoi(PQgetvalue(res, i, 0)));
    }
    
    PQclear(res);
    return NULL;
}

/*
 
 
 6. גילוי פעולות הלבנת כספים
 money_laundering קלט
 תיאור הפעולה הבולשת הפדראלית בטכניון )TBI( גילתה כי לארגוני הלבנת כספים יש דפוס
 פעולה מסוים בכל הקשור להעברות בנקאיות. "ראש" הארגון מעביר כמות כסף
 מאחד מחשבונותיו לחשבון לקוח אחר, הלקוח הזה יכול לנצל חלק מהכסף
 ולהעביר את היתרה לחשבונו של לקוח נוסף, וכך הלאה עד שהלקוח האחרון
 מעביר את מה שנותר מהכסף לאחד מחשבונות ה"ראש". באופן הזה נוצר מעגל
 העברות.
 מעגלי העברות מתאפיינים ע"י התכונות הבאות:
 1. ההעברות מתבצעות בסדר עולה לפי הזמן.
 2. הסכום בהעברה מסוימת )לא הראשונה( קטן או שווה לסכום בהעברה
 שלפניה.
 הערה: מעגלי העברות לא מוגבלים באורכם.
 פלט בשורה ראשונה יודפס:
 MONEY LAUNDERING
 בשורות הבאות,יודפסו מספרי ת"ז של "ראשי" ארגוני הלבנת כספים פוטנציאליים.
 המספרים יודפסו בשורות נפרדות וימוינו בסדר עולה. אם התוצאה ריקה יודפס
 הקבוע EMPTY.
 
 */

void* moneyLaundering() {
    
    printf(MONEY_LAUNDERING);
    
    char cmd[CMD_SIZE];
    
   int i=0;
    
    sprintf(cmd, "DROP TABLE money");
    res = PQexec(conn, cmd);
    
    if(!res) { fprintf(stderr, "5Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    
    
    sprintf(cmd, "CREATE TABLE money AS SELECT TID, IDF,IDT,TAmount FROM Transfer; SELECT * FROM money");
    res = PQexec(conn, cmd);
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "1Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
    
    /*sprintf(cmd, "INSERT INTO money (IDF, IDT, Amount) SELECT IDF, IDT, TAmount FROM Transfer");
    res = PQexec(conn, cmd);
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "2Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }*/
    
    int num_id = PQntuples(res);
    
    /*PQclear(res);
    
    sprintf(cmd, "INSERT INTO money (TID, IDF, IDT, TAmount) SELECT T1.TID, T.IDF, T1.IDT, T1.TAmount FROM money T INNER JOIN money T1 ON T.IDT=T1.IDF AND T.TAmount>=T1.TAmount AND T.TID<T1.TID");
    
    res = PQexec(conn, cmd);
    
    if(!res) { fprintf(stderr, "3Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }*/
    
   while (num_id){
    PQclear(res);
        
       sprintf(cmd, "INSERT INTO money (TID, IDF, IDT, TAmount) SELECT T1.TID, T.IDF, T1.IDT, T1.TAmount FROM money T INNER JOIN money T1 ON T.IDT=T1.IDF AND T.TAmount>=T1.TAmount AND T.TID<T1.TID AND NOT EXISTS(SELECT * FROM money M WHERE T1.TID=M.TID AND T.IDF=M.IDF AND T1.IDT=M.IDT AND T1.TAmount=M.TAmount)");
       
     res = PQexec(conn, cmd);
     
     if(!res) { fprintf(stderr, "2Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }
        
        num_id--;
    }
    
    sprintf(cmd, "SELECT IDF FROM money WHERE IDT=IDF ORDER BY IDF");
    
    res = PQexec(conn, cmd);
    if(!res || PQresultStatus(res) != PGRES_TUPLES_OK) { fprintf(stderr, "4Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }

    res = PQexec(conn, cmd);
    
     int t_num = PQntuples(res);
    
     if(!t_num) printf(EMPTY);
     else{
     for(i=0; i<t_num;i++)
     printf("%d\n", atoi(PQgetvalue(res, i, 0)));
     }
     
    sprintf(cmd, "DROP TABLE money");
    res = PQexec(conn, cmd);
    
    if(!res) { fprintf(stderr, "5Error executing query: %s\n", PQresultErrorMessage(res)); return NULL; }

    PQclear(res);
    return NULL;
}






/*FOR TEST*/

bool Execute(char * command, ExecStatusType type) {
    res = PQexec(conn, command);
    if (!res || PQresultStatus(res) != type) {
        fprintf(stderr, "Error executing query: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        exit(1);
    }
    return true;
}

void InitSystem() {
    /* DROPPING */
    Execute("DROP TABLE customer", PGRES_COMMAND_OK);
    PQclear(res);
    Execute("DROP TABLE account", PGRES_COMMAND_OK);
    PQclear(res);
    Execute("DROP TABLE ownsacc", PGRES_COMMAND_OK);
    PQclear(res);
    Execute("DROP TABLE branch", PGRES_COMMAND_OK);
    PQclear(res);
    Execute("DROP TABLE managesacc", PGRES_COMMAND_OK);
    PQclear(res);
    Execute("DROP TABLE withdrawal", PGRES_COMMAND_OK);
    PQclear(res);
    Execute("DROP TABLE transfer", PGRES_COMMAND_OK);
    PQclear(res);
    /* INITING */
    Execute("CREATE TABLE customer (LIKE course_customer INCLUDING DEFAULTS INCLUDING CONSTRAINTS INCLUDING INDEXES)", PGRES_COMMAND_OK);
    PQclear(res);
    Execute("INSERT INTO customer SELECT * FROM course_customer", PGRES_COMMAND_OK);
    PQclear(res);
    Execute("CREATE TABLE account (LIKE course_account INCLUDING DEFAULTS INCLUDING CONSTRAINTS INCLUDING INDEXES)", PGRES_COMMAND_OK);
    PQclear(res);
    Execute("INSERT INTO account SELECT * FROM course_account", PGRES_COMMAND_OK);
    PQclear(res);
    Execute("CREATE TABLE ownsacc (LIKE course_ownsacc INCLUDING DEFAULTS INCLUDING CONSTRAINTS INCLUDING INDEXES)", PGRES_COMMAND_OK);
    PQclear(res);
    Execute("INSERT INTO ownsacc SELECT * FROM course_ownsacc", PGRES_COMMAND_OK);
    PQclear(res);
    Execute("CREATE TABLE branch (LIKE course_branch INCLUDING DEFAULTS INCLUDING CONSTRAINTS INCLUDING INDEXES)", PGRES_COMMAND_OK);
    PQclear(res);
    Execute("INSERT INTO branch SELECT * FROM course_branch", PGRES_COMMAND_OK);
    PQclear(res);
    Execute("CREATE TABLE managesacc (LIKE course_managesacc INCLUDING DEFAULTS INCLUDING CONSTRAINTS INCLUDING INDEXES)", PGRES_COMMAND_OK);
    PQclear(res);
    Execute("INSERT INTO managesacc SELECT * FROM course_managesacc", PGRES_COMMAND_OK);
    PQclear(res);
    Execute("CREATE TABLE withdrawal (LIKE course_withdrawal INCLUDING DEFAULTS INCLUDING CONSTRAINTS INCLUDING INDEXES)", PGRES_COMMAND_OK);
    PQclear(res);
    Execute("INSERT INTO withdrawal SELECT * FROM course_withdrawal", PGRES_COMMAND_OK);
    PQclear(res);
    Execute("CREATE TABLE transfer (LIKE course_transfer INCLUDING DEFAULTS INCLUDING CONSTRAINTS INCLUDING INDEXES)", PGRES_COMMAND_OK);
    PQclear(res);
    Execute("INSERT INTO transfer SELECT * FROM course_transfer", PGRES_COMMAND_OK);
    PQclear(res);
}

void Prepare_Aux(int x, char * name, char * address) {
    char cmd[512];
    sprintf(cmd, "INSERT INTO customer VALUES (%d, '%s', '%s')",x,name,address);
    Execute(cmd, PGRES_COMMAND_OK);
    PQclear(res);
    addAccount(x + 100, x, (x % 5) + 10);
}

void PrepareForTest1() {
    InitSystem();
    char cmd[512];
    Prepare_Aux(1, "Jon Snow", "Castle Black");
    Prepare_Aux(2, "Ned Stark", "Winterfell");
    Prepare_Aux(3, "Tyrion Lannister", "Kings Landing");
    Prepare_Aux(4, "Sansa Stark", "Winterfell");
    Prepare_Aux(5, "Arya Stark", "Winterfell");
    Prepare_Aux(6, "Theon Greyjoy", "Pike");
    Prepare_Aux(7, "Jaime Lannister", "Kings Landing");
    Prepare_Aux(8, "Cersei Lannister", "Kings Landing");
    Prepare_Aux(9, "Daenerys Targaryen_The_Unburnt_Queen_of_the_Andals_the_Rhoynar_and_of_the_First_Men_Queen_of_Meereen_Khaleesi_of_the_Great_Grass_Sea_Breaker_of_Chains_Mother_of_Dragons", "Meereen");
    Prepare_Aux(10, "Viserys Targaryen", "Heaven or Hell");
    Prepare_Aux(11, "Joffrey Baratheon", "Hell for sure");
    Prepare_Aux(12, "Robert Baratheon", "Heaven probably");
    Prepare_Aux(13, "Robb Stark", "Winterfell");
    Prepare_Aux(14, "Ramsay Bolton", "We dont know");
    Prepare_Aux(15, "Jorah Mormont", "Meereen");
    sprintf(cmd, "UPDATE account SET Balance = 100, Overdraft = -1000 WHERE anumber = 100");
    Execute(cmd, PGRES_COMMAND_OK);
    PQclear(res);
    sprintf(cmd, "UPDATE account SET Balance = 10000,Overdraft = -100000 WHERE ANumber = 102");
    Execute(cmd, PGRES_COMMAND_OK);
    PQclear(res);
    sprintf(cmd, "UPDATE account SET Balance = 21000,Overdraft = -123456 WHERE ANumber = 103");
    Execute(cmd, PGRES_COMMAND_OK);
    PQclear(res);
    sprintf(cmd, "UPDATE account SET Balance = 31000,Overdraft = -22356 WHERE ANumber = 107");
    Execute(cmd, PGRES_COMMAND_OK);
    PQclear(res);
    sprintf(cmd, "UPDATE account SET Balance = 39900,Overdraft = -22456 WHERE ANumber = 108");
    Execute(cmd, PGRES_COMMAND_OK);
    PQclear(res);
    printf("INIT DONE!\n");
}














