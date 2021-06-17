#include <sqlite3.h>
#include <stdio.h>

int main(void) {
    
    sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open("TopMusic.db", &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
    }
    
    char *sql = "DROP TABLE IF EXISTS User;" 
                "CREATE TABLE User (ACCES INT, Name TEXT, password INT);"  
                "INSERT INTO User VALUES(0, 'Ioana', 52642);" 
                "INSERT INTO User VALUES(1, 'Violeta', 57127);" 
                "INSERT INTO User VALUES(1, 'Ionut', 9000);" 
                "INSERT INTO User VALUES(1, 'Marius', 29000);" 
                "INSERT INTO User VALUES(1, 'MIhai', 350000);" 
                "INSERT INTO User VALUES(1, 'Cristina', 21000);" 
                "INSERT INTO User VALUES(1, 'Dana2000', 41400);" 
                "INSERT INTO User VALUES(1, 'Didi', 21600);";
              

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        
        return 1;
    } 
    
    sqlite3_close(db);
    
    return 0;
}
