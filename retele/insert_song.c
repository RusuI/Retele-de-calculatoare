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
    
    char *sql = "DROP TABLE IF EXISTS Song;" 
                "CREATE TABLE Song (ID INT, Name TEXT, Descr TEXT, Genre TEXT, Link TEXT, Votes INT);"  
                "INSERT INTO Song VALUES(1 , 'Uptown Funk' ,'Mark Ronson-Uptown Funk (Official Video) ft. Bruno Mars' , 'Pop' , 'https://www.youtube.com/watch?v=OPf0YbXqDm0', 3);" 
                "INSERT INTO Song VALUES(2, 'Watermelon Sugar', 'Harry Styles - Watermelon Sugar (Official Video)', 'Pop', 'https://www.youtube.com/watch?v=7-x3uD5z1bQ', 2);"  
                "INSERT INTO Song VALUES(3, 'My Oh My', 'Camila Cabello - My Oh My (Lyrics) ft. DaBaby', 'Pop', 'https://www.youtube.com/watch?v=5X4c-AvhQts', 5);" 
                "INSERT INTO Song VALUES(4, 'It is My Life', 'Bon Jovi - It is My Life (Official Music Video) ', 'Rock' , 'https://www.youtube.com/watch?v=vx2u5uUu3DE&ab_channel=BonJoviVEVO', 8);" 
                "INSERT INTO Song VALUES(4, 'It is My Life', 'Bon Jovi - It is My Life (Official Music Video) ', 'Pop' ,'https://www.youtube.com/watch?v=vx2u5uUu3DE&ab_channel=BonJoviVEVO', 8);";
                
              

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
