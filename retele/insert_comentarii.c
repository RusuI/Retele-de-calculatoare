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
    
    char *sql = "DROP TABLE IF EXISTS Coment;" 
                "CREATE TABLE Coment (ID INT, Name TEXT, Coment TEXT);"  
                "INSERT INTO Coment VALUES(1, 'Uptown Funk','This is the perfect cheer song ');"
                "INSERT INTO Coment VALUES(2, 'Uptown Funk','You ever get into a mood at 2:50am and this song just hits different and you can listen to it 20 times');"
                "INSERT INTO Coment VALUES(3, 'Watermelon Sugar', 'Tastes like strawberries...Um sir that is a watermelon');"  
                "INSERT INTO Coment VALUES(4, 'My Oh My', 'LOVE this song!!');" 
                "INSERT INTO Coment VALUES(5, 'My Oh My', 'This song is just Havana, but with extra sauce.');"; 
                /*"INSERT INTO Coment VALUES(4, 'It is My Life', ' ');"
                "INSERT INTO Coment VALUES(4, 'It is My Life', ' ');"
                "INSERT INTO Coment VALUES(4, 'It is My Life', ' ');"
                "INSERT INTO Coment VALUES(4, 'It is My Life', ' ');";*/
                
              

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
