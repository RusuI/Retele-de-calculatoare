
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>


#define PORT 2024
static int callback(void *str, int argc, char **argv,
                    char **azColName)
                    {
    char* data = (char*) str;
    /*strcpy(data,"");*/

    for ( int i = 0; i < argc; i++)
    {
        strcat (data, azColName[i]);
        strcat (data, "=");
        if (argv[i])
        {
            strcat (data, argv[i]);
            strcat (data,"\n");
        }
        else
            strcat (data, "NULL");
    }
   /* printf("++%s++\n",str);*/
    return 0;
}

int main ()
{
    //baza de date
    sqlite3 *db;
    char *err_msg = 0;
    char str[100];
    char sql[100];

    int rc = sqlite3_open("TopMusic.db", &db);

    if (rc != SQLITE_OK) {

        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    struct sockaddr_in server;	// structura folosita de server
    struct sockaddr_in from;
    char msg[100];		//mesajul primit de la client
    char msgrasp[100]=" ";        //mesaj de raspuns pentru client
    int sd;			//descriptorul de socket


    int count_u=8; //numarul de inregistrari user
    int count_a=3; //numarul de inregistari admin
    int log_user=0, log_admin=0;
    int count_s=9;
    int count_com=6;


    /* crearea unui socket */
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("[server]Eroare la socket().\n");
        return errno;
    }

    /* pregatirea structurilor de date */
    bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));

    /* umplem structura folosita de server */
    /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;
    /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    /* utilizam un port utilizator */
    server.sin_port = htons (PORT);

    /* atasam socketul */
    if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
        perror ("[server]Eroare la bind().\n");
        return errno;
    }

    /* punem serverul sa asculte daca vin clienti sa se conecteze */
    if (listen (sd, 1) == -1)
    {
        perror ("[server]Eroare la listen().\n");
        return errno;
    }

    /* servim in mod concurent clientii... */
    while (1)
    {
        char type[100]; //tipul de utilizator pe care il initializez
        int client;
        int length = sizeof (from);

        printf ("[server]Asteptam la portul %d...\n",PORT);
        fflush (stdout);

        /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
        client = accept (sd, (struct sockaddr *) &from, &length);

        /* eroare la acceptarea conexiunii de la un client */
        if (client < 0)
        {
            perror ("[server]Eroare la accept().\n");
            continue;
        }

        int pid;
        if ((pid = fork()) == -1)
        {
            close(client);
            continue;
        }
        else if (pid > 0)
        {
            // parinte
            close(client);
            while(waitpid(-1,NULL,WNOHANG));
            continue;
        }
        else if (pid == 0)
        {
            int closing = 0;
            while (1)
            {
                // copil
                close(sd);
                 strcpy(str,"");
                 strcpy(sql,"");
                /* s-a realizat conexiunea, se astepta mesajul */
                bzero(msg, 100);
                printf("[server]Asteptam mesajul...\n");
                fflush(stdout);

                /* citirea mesajului */
                if (read(client, msg, 100) <= 0)
                {
                    perror("[server]Eroare la read() de la client.\n");
                    close(client);    /* inchidem conexiunea cu clientul */
                    continue;        /* continuam sa ascultam */
                }

                printf("[server]Mesajul a fost receptionat...%s\n", msg);

                //Logare user
                /*printf("||%s||\n",msg);*/
                if (strstr(msg, "Login_utilizator") != NULL)
                {
                    str[100] = "";
                    sql[100] = "";
                    strcpy(type, "user");
                    char credentiale[100] = "";
                    char x[100] = "";
                    strncpy(x, msg, (strlen(msg)));
                    strcpy(credentiale, x + 16);
                    /*printf("%s\n",credentiale);*/
                    char a[2][100];
                    int i = 1;
                    char *p = strtok(credentiale, "^");
                    /*printf("%s\n",p);*/
                    while (p != NULL)
                    {
                        strcpy(a[i], p);
                       /* printf("%s\n", a[i]);*/
                        p = strtok(NULL, "^");
                        i++;
                    }

                    sprintf(sql, "SELECT * FROM User WHERE Name LIKE '%s';", a[1]);
                    rc = sqlite3_exec(db, sql, callback, str, &err_msg);
                    /*printf("%s",str);*/
                    if (rc != SQLITE_OK) {
                        fprintf(stderr, "Failed to select data\n");
                        fprintf(stderr, "SQL error: %s\n", err_msg);
                        bzero(msgrasp, 100);
                        strcpy(msgrasp, "Failed to select data");
                        sqlite3_free(err_msg);
                        return 1;
                    }
                    if (strstr(str, a[1]) != NULL)
                    {
                        str[100] = "";
                        sql[100] = "";
                        sprintf(sql, "SELECT * FROM User WHERE password LIKE '%s';", a[2]);
                        rc = sqlite3_exec(db, sql, callback, str, &err_msg);
                        /*printf("%s",str);*/
                        if (rc != SQLITE_OK) {
                            fprintf(stderr, "Failed to select data\n");
                            fprintf(stderr, "SQL error: %s\n", err_msg);
                            bzero(msgrasp, 100);
                            strcpy(msgrasp, "Failed to select data");
                            sqlite3_free(err_msg);
                            return 1;
                        }
                        if (strstr(str, a[2]) != NULL)
                        {
                            bzero(msgrasp, 100);
                            strcpy(msgrasp, "Bun venit!");
                        } else
                            {   /*printf("%s\n",a[2]);*/
                                 bzero(msgrasp, 100);
                                 strcpy(msgrasp, "Parola incorecta!");
                            }

                    } else
                        {   /*printf("%s\n",a[1]);*/
                        bzero(msgrasp, 100);
                        strcpy(msgrasp, "Numele de utilizator inexitent!");
                        }
                }
                if (strstr(msg, "Adaugare_melodie") != NULL)
                {
                    str[100] = "";
                    sql[100] = "";
                    if (strstr(type, "user") != NULL)
                    {
                        char credentiale[100] = "";
                        char x[100] = "";
                        strncpy(x, msg, (strlen(msg)));
                        strcpy(credentiale, x + 16);
                        /*printf("%s\n", credentiale);*/
                        char a[5][500];
                        int i = 1;
                        char *p = strtok(credentiale, "^");
                        /*printf("%s\n",p);*/
                        while (p != NULL)
                        {
                            strcpy(a[i], p);
                            /*printf("%s\n", a[i]);*/
                            p = strtok(NULL, "^");
                            i++;
                        }

                        sprintf(sql, "INSERT INTO Song VALUES('%d','%s','%s','%s','%s',0);", count_s, a[1], a[2], a[3],a[4]);
                        rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
                        /*printf("%s",str);*/
                        if (rc != SQLITE_OK)
                        {
                            fprintf(stderr, "Failed to insert data\n");
                            fprintf(stderr, "SQL error: %s\n", err_msg);
                            bzero(msgrasp, 100);
                            strcpy(msgrasp, "Failed to insert data");
                            sqlite3_free(err_msg);
                            return 1;
                        } else
                            {
                              count_s = count_s + 1;
                              bzero(msgrasp, 100);
                              strcpy(msgrasp, "Melodie adaugata cu succes!");
                            }

                    } else
                        {
                          bzero(msgrasp, 100);
                          strcpy(msgrasp, "Nu sunteti conectat!Pentru a realiza comanda este necesara logarea.");
                       }

                }
                if (strstr(msg, "Adaugare_comentarii") != NULL)
                {
                    str[100] = "";
                    sql[100] = "";
                    /*printf("%s",msg);*/
                    if (strstr(type, "user") != NULL)
                    {
                        char credentiale[100] = "";
                        char x[100] = "";
                        strncpy(x, msg, (strlen(msg)));
                        strcpy(credentiale, x + 19);
                        char a[2][100];
                        int i = 1;
                        char *p = strtok(credentiale, "^");
                        while (p != NULL)
                        {
                            strcpy(a[i], p);
                            /*printf("%s\n", a[i]);*/
                            p = strtok(NULL, "^");
                            i++;
                        }
                        sprintf(sql, "SELECT Name FROM Coment WHERE Name = '%s';",a[1]);
                        rc = sqlite3_exec(db, sql, callback, str, &err_msg);
                        /*printf("%s", str);*/
                        if (rc != SQLITE_OK)
                        {
                            fprintf(stderr, "Failed to select data\n");
                            fprintf(stderr, "SQL error: %s\n", err_msg);
                            bzero(msgrasp, 100);
                            strcpy(msgrasp, "Failed to select data");
                            sqlite3_free(err_msg);
                        }
                        else if(strstr(str,a[1])!=NULL)
                        {   count_com=count_com+1;
                            sprintf(sql, "INSERT INTO Coment VALUES('%d','%s','%s');", count_com,a[1],a[2]);
                            rc = sqlite3_exec(db, sql, callback, str, &err_msg);
                            /*printf("%s", str);*/
                            if (rc != SQLITE_OK)
                            {
                                fprintf(stderr, "Failed to insert data\n");
                                fprintf(stderr, "SQL error: %s\n", err_msg);
                                bzero(msgrasp, 100);
                                strcpy(msgrasp, "Failed to insert data");
                                sqlite3_free(err_msg);
                            } else
                            {
                                bzero(msgrasp, 100);
                                strcpy(msgrasp, "Comentariu adaugat cu succes!");
                            }
                        } else
                        {
                            bzero(msgrasp, 100);
                            strcpy(msgrasp, "Melodie inexistenta!");
                        }


                    }
                    else
                        {
                        bzero(msgrasp, 100);
                        strcpy(msgrasp, "Nu sunteti conectat!");
                        }
                }
                if (strstr(msg, "Vot_song") != NULL)
                {
                    if (strstr(type, "user") != NULL)
                    {
                        str[100] = "";
                        sql[100] = "";
                        char credentiale[100] = "";
                        char x[100] = "";
                        strncpy(x, msg, (strlen(msg)));
                        strcpy(credentiale, x + 8);
                        /*printf("%s\n", credentiale);*/
                        char a[2][100];
                        int i = 1;
                        char *p = strtok(credentiale, "^");
                        /*printf("%s\n", p);*/
                        while (p != NULL)
                        {
                            strcpy(a[i], p);
                            /*printf("%s\n", a[i]);*/
                            p = strtok(NULL, "^");
                            i++;
                        }

                        sprintf(sql, "SELECT ACCES FROM User WHERE Name='%s';", a[1]);
                        rc = sqlite3_exec(db, sql, callback, str, &err_msg);
                        printf("%s", str);
                        if (rc != SQLITE_OK)
                        {
                            fprintf(stderr, "Failed to select data\n");
                            fprintf(stderr, "SQL error: %s\n", err_msg);
                            bzero(msgrasp, 100);
                            strcpy(msgrasp, "Failed to select data");
                            sqlite3_free(err_msg);
                            return 1;
                        } else if (strstr(str, "ACCES=1") != NULL)
                        {
                            str[100] = "";
                            sql[100] = "";
                            sprintf(sql, "UPDATE Song SET Votes=Votes+1;");
                            rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
                            /*printf("%s",str);*/
                            if (rc != SQLITE_OK)
                            {
                                fprintf(stderr, "Failed to update data\n");
                                fprintf(stderr, "SQL error: %s\n", err_msg);
                                bzero(msgrasp, 100);
                                strcpy(msgrasp, "Failed to update data");
                                sqlite3_free(err_msg);
                                return 1;
                            } else
                                {
                                bzero(msgrasp, 100);
                                strcpy(msgrasp, "Votare cu succes!");
                                }
                        } else
                            {
                              bzero(msgrasp, 100);
                              strcpy(msgrasp, "Dreptul la vot va este restrictionat!Nu puteti efectua comanda de votare.");
                            }

                    } else
                        {
                          bzero(msgrasp, 100);
                          strcpy(msgrasp, "Nu sunteti conectat!");
                       }

                }
                if (strstr(msg, "Afisare_top") != NULL)
                {
                    if (strstr(type, "user") != NULL)
                    {
                        char credentiale[100] = "";
                        char x[100] = "";
                        strncpy(x, msg, (strlen(msg)));
                        strcpy(credentiale, x + 11);
                        /*printf("%s\n", credentiale);*/
                        char sql[100] = "";
                        char str[100] = "";
                        sprintf(sql, "SELECT DISTINCT Name FROM Song ORDER BY Votes DESC;");
                        rc = sqlite3_exec(db, sql, callback, str, &err_msg);
                       /* printf("%s", str);*/
                        if (rc != SQLITE_OK)
                        {
                            fprintf(stderr, "Failed to select data\n");
                            fprintf(stderr, "SQL error: %s\n", err_msg);
                            bzero(msgrasp, 100);
                            strcpy(msgrasp, "Failed to select data");
                            sqlite3_free(err_msg);
                            return 1;
                        } else
                            {
                             bzero(msgrasp, 100);
                             /*char str1[100]="";
                             strncpy(str1,str,strlen(str));*/
                             str[strlen(str)-1]='\0';
                             strcpy(msgrasp, str);
                            }
                    } else
                        {
                          bzero(msgrasp, 100);
                          strcpy(msgrasp, "Nu sunteti logat");
                        }
                }
                if (strstr(msg, "Link_melodie") != NULL)
                {
                    if (strstr(type, "user") != NULL)
                    {
                        char credentiale[100] = "";
                        char x[100] = "";
                        strncpy(x, msg, (strlen(msg)));
                        strcpy(credentiale, x + 12);
                        printf("%s\n", credentiale);
                        char sql[100] = "";
                        char str[100] = "";
                            sprintf(sql, "SELECT Link From Song Where Name='%s';", credentiale);
                            rc = sqlite3_exec(db, sql, callback, str, &err_msg);
                            /*printf("%s",str);*/
                            if (rc != SQLITE_OK)
                            {
                                fprintf(stderr, "Failed to select data\n");
                                fprintf(stderr, "SQL error: %s\n", err_msg);
                                bzero(msgrasp, 100);
                                strcpy(msgrasp, "Failed to select data");
                                sqlite3_free(err_msg);
                                return 1;
                            } else
                                {
                                   bzero(msgrasp, 100);
                                   strcpy(msgrasp, str);
                                }
                            } else
                              {
                                bzero(msgrasp, 100);
                                strcpy(msgrasp, "Nu sunteti logat.Pentru a efectua comanda trebuie sa va logati.");
                              }
                }
                if (strstr(msg, "Afisare_gen") != NULL)
                {
                    if (strstr(type, "user") != NULL)
                    {
                        char credentiale[100] = "";
                        char x[100] = "";
                        strncpy(x, msg, (strlen(msg)));
                        strcpy(credentiale, x + 11);
                        /*printf("%s\n", credentiale);*/
                        char sql[100] = "";
                        char str[100] = "";
                        sprintf(sql, "SELECT DISTINCT Name FROM Song WHERE Genre='%s' ORDER BY Votes DESC;",credentiale);
                        rc = sqlite3_exec(db, sql, callback, str, &err_msg);
                        /*printf("%s", str);*/
                        if (rc != SQLITE_OK)
                        {
                            fprintf(stderr, "Failed to select data\n");
                            fprintf(stderr, "SQL error: %s\n", err_msg);
                            bzero(msgrasp, 100);
                            strcpy(msgrasp, "Failed to select data");
                            sqlite3_free(err_msg);
                            return 1;
                        } else
                            {
                            bzero(msgrasp, 100);
                            strcpy(msgrasp, str);
                            }
                    } else
                        {
                        bzero(msgrasp, 100);
                        strcpy(msgrasp, "Nu sunteti logat.Pentru a realiza comanda trebuie sa va logati.");
                        }
                }
                if (strstr(msg, "Login_admin") != NULL)
                {   str[100]="";
                    sql[100]="";
                    strcpy(type,"admin");
                    char credentiale[100]="";
                    char x[100]="";
                    strncpy(x,msg,(strlen(msg)));
                    strcpy(credentiale,x+11);
                    /*printf("%s\n",credentiale);*/
                    char a[2][100];
                    int i = 1;
                    char *p = strtok(credentiale, "^");
                    /*printf("%s\n",p);*/
                    while (p != NULL)
                    {
                        strcpy(a[i], p);
                       /* printf("%s\n", a[i]);*/
                        p = strtok(NULL, "^");
                        i++;
                    }

                    sprintf (sql, "SELECT * FROM Admin WHERE Name LIKE '%s';",a[1]);
                    rc = sqlite3_exec(db, sql, callback, str, &err_msg);
                   /* printf("%s",str);*/
                    if (rc != SQLITE_OK )
                    {
                        fprintf(stderr, "Failed to select data\n");
                        fprintf(stderr, "SQL error: %s\n", err_msg);
                        bzero(msgrasp,100);
                        strcpy(msgrasp,"Failed to select data");
                        sqlite3_free(err_msg);
                        return 1;
                    }
                    if(strstr(str,a[1])!=NULL)
                    {
                        str[100] = "";
                        sql[100] = "";
                        sprintf (sql, "SELECT * FROM Admin WHERE password LIKE '%s';",a[2]);
                        rc = sqlite3_exec(db, sql, callback, str, &err_msg);
                        /*printf("%s",str);*/
                        if (rc != SQLITE_OK )
                        {
                            fprintf(stderr, "Failed to select data\n");
                            fprintf(stderr, "SQL error: %s\n", err_msg);
                            bzero(msgrasp,100);
                            strcpy(msgrasp,"Failed to select data");
                            sqlite3_free(err_msg);
                            return 1;
                        }
                        if(strstr(str,a[2])!=NULL)
                        {
                            bzero(msgrasp, 100);
                            strcpy(msgrasp,"Bun venit!");
                        }
                        else
                        {   /*printf("%s\n",a[2]);*/
                            bzero(msgrasp, 100);
                            strcpy(msgrasp,"Parola incorecta!");
                        }

                        /*bzero(msgrasp, 100);
                        strcpy(msgrasp,"Bun venit!");*/
                    }
                    else
                    {   printf("%s\n",a[1]);
                        bzero(msgrasp, 100);
                        strcpy(msgrasp,"Numele de utilizator inexitent!");
                    }
                }
                //Logare admin + comenzi admin(restrictionare, stergere melodie, stergere comentarii)
                if(strstr(msg,"Restrictionare_vot"))
                {
                    str[100]="";
                    sql[100]="";
                    if(strstr(type,"admin")!=NULL)
                    {   char credentiale[100]="";
                        char x[100]="";
                        strncpy(x,msg,(strlen(msg)));
                        strcpy(credentiale,x+18);
                        /*printf("%s\n",credentiale);*/

                        sprintf (sql, "SELECT * FROM User WHERE Name LIKE '%s';",credentiale);
                        rc = sqlite3_exec(db, sql, callback, str, &err_msg);
                        printf("%s",str);
                        if (rc != SQLITE_OK )
                        {
                            fprintf(stderr, "Failed to select data\n");
                            fprintf(stderr, "SQL error: %s\n", err_msg);
                            bzero(msgrasp,100);
                            strcpy(msgrasp,"Failed to delete data");
                            sqlite3_free(err_msg);
                            return 1;
                        }
                        else if(strstr(str,credentiale)!=NULL)
                          {
                              str[100] = "";
                              sql[100] = "";
                            sprintf (sql, "UPDATE User SET ACCES=0 WHERE Name LIKE '%s';",credentiale);
                            rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
                           /* printf("%s",str);*/
                            if (rc != SQLITE_OK )
                            {
                                fprintf(stderr, "Failed to update data\n");
                                fprintf(stderr, "SQL error: %s\n", err_msg);
                                bzero(msgrasp,100);
                                strcpy(msgrasp,"Failed to update data");
                                sqlite3_free(err_msg);
                                return 1;
                            }
                            else
                                {
                                  bzero(msgrasp, 100);
                                  strcpy(msgrasp, "Resctrictionare cu succes");
                                 }

                          }

                        else
                            {   /*printf("%s\n",credentiale);*/
                                bzero(msgrasp, 100);
                                strcpy(msgrasp,"Numele de utilizator inexitent!");
                            }
                    }
                    else
                    {
                        bzero(msgrasp, 100);
                        strcpy(msgrasp,"Nu sunteti logat sau nu aveti dreptul de a executa aceste comenzi!");
                    }

                }
                if(strstr(msg,"Stergere_melodie")!=NULL)
                {
                    str[100] = "";
                    sql[100] = "";

                    if (strstr(type, "admin") != NULL)
                    {
                        char credentiale[100] = "";
                        char x[100] = "";
                        strncpy(x, msg, (strlen(msg)));
                        strcpy(credentiale, x + 16);
                        /*printf("%s\n", credentiale);*/

                        sprintf(sql, "SELECT Name FROM Song WHERE Name = '%s';",credentiale);
                        rc = sqlite3_exec(db, sql, callback, str, &err_msg);
                        /*printf("%s", str);*/
                        if (rc != SQLITE_OK)
                        {
                            fprintf(stderr, "Failed to select data\n");
                            fprintf(stderr, "SQL error: %s\n", err_msg);
                            bzero(msgrasp, 100);
                            strcpy(msgrasp, "Failed to select data");
                            sqlite3_free(err_msg);
                        }
                        else if(strstr(str,credentiale)!=NULL)
                        {   str[100] = "";
                            sql[100] = "";
                            sprintf(sql, "DELETE FROM Song WHERE Name LIKE '%s';", credentiale);
                            rc = sqlite3_exec(db, sql, callback, str, &err_msg);
                            /*printf("%s", str);*/
                            if (rc != SQLITE_OK)
                            {
                                fprintf(stderr, "Failed to delete data\n");
                                fprintf(stderr, "SQL error: %s\n", err_msg);
                                bzero(msgrasp, 100);
                                strcpy(msgrasp, "Failed to delete data");
                                sqlite3_free(err_msg);
                            } else
                                {
                                  bzero(msgrasp, 100);
                                  strcpy(msgrasp, "Melodie stearsa cu succes!");
                                }
                        } else
                           {
                             bzero(msgrasp, 100);
                             strcpy(msgrasp, "Melodie inexistenta!");
                           }

                    } else
                        {
                        bzero(msgrasp, 100);
                        strcpy(msgrasp, "Nu sunteti logat sau nu aveti dreptul de a executa aceste commenzi!");
                        }
                }
                if(strstr(msg,"Afisare_com")!=NULL)
                {
                   if(strstr(type,"admin")!=NULL)
                   {
                       char credentiale[100] = "";
                       char x[100] = "";
                       strncpy(x, msg, (strlen(msg)));
                       strcpy(credentiale, x + 11);
                       /*printf("%s\n", credentiale);*/
                       sql[100] = "";
                       str[100] = "";
                       sprintf(sql, "SELECT ID,Coment FROM Coment Where Name='%s';",credentiale);
                       rc = sqlite3_exec(db, sql, callback, str, &err_msg);
                      /* printf("%s", str);*/
                       if (rc != SQLITE_OK)
                       {
                           fprintf(stderr, "Failed to select data\n");
                           fprintf(stderr, "SQL error: %s\n", err_msg);
                           bzero(msgrasp, 100);
                           strcpy(msgrasp, "Failed to select data");
                           sqlite3_free(err_msg);
                           return 1;
                       } else
                           {
                              bzero(msgrasp, 100);
                              strcpy(msgrasp, str);
                           }
                   }
                   else
                       {
                          bzero(msgrasp, 100);
                          strcpy(msgrasp, "Nu sunteti logat!Pentru e efectua comanda va rog sa va autentificati");
                       }

                }
                if(strstr(msg,"Stergere_com")!=NULL)
                {
                    str[100] = "";
                    sql[100] = "";

                    if (strstr(type, "admin") != NULL)
                    {   /*printf("qwerty\n");*/
                        char credentiale[100] = "";
                        char x[100] = "";
                        strncpy(x, msg, (strlen(msg)));
                        strcpy(credentiale, x + 12);
                       /* printf("%s\n", credentiale);*/
                        char a[2][100];
                        int i = 1;
                        char *p = strtok(credentiale, "^");
                        /*printf("%s\n", p);*/
                        while (p != NULL) {
                            strcpy(a[i], p);
                            /*printf("%s\n", a[i]);*/
                            p = strtok(NULL, "^");
                            i++;
                        }
                        int c = a[2][0] - '0';

                        sprintf(sql, "SELECT Name FROM Song WHERE Name = '%s';", a[1]);
                        rc = sqlite3_exec(db, sql, callback, str, &err_msg);
                        if (rc != SQLITE_OK)
                        {
                            fprintf(stderr, "Failed to select data\n");
                            fprintf(stderr, "SQL error: %s\n", err_msg);
                            bzero(msgrasp, 100);
                            strcpy(msgrasp, "Failed to select data");
                            sqlite3_free(err_msg);
                        } else if (strstr(str, a[1]) != NULL)
                        {   str[100] = "";
                            sql[100] = "";
                            sprintf(sql, "SELECT Name FROM Coment WHERE ID='%d';", c);
                            rc = sqlite3_exec(db, sql, callback, str, &err_msg);
                           /* printf("__%s", str);*/
                            if (rc != SQLITE_OK)
                            {
                                fprintf(stderr, "Failed to select data\n");
                                fprintf(stderr, "SQL error: %s\n", err_msg);
                                bzero(msgrasp, 100);
                                strcpy(msgrasp, "Failed to delete data");
                                sqlite3_free(err_msg);
                            } else if (strstr(str, a[1]) != NULL)
                            {   /*printf("ce nu merge? %d\n",c);*/
                                str[100] = "";
                                sql[100] = "";
                                sprintf(sql, "DELETE FROM Coment WHERE ID=%d;", c);
                                rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
                                /*printf("%s", str);*/
                                if (rc != SQLITE_OK)
                                {
                                    fprintf(stderr, "Failed to delete data\n");
                                    fprintf(stderr, "SQL error: %s\n", err_msg);
                                    /*printf("EROAARREEEEE!!!!\n");*/
                                    bzero(msgrasp, 100);
                                    strcpy(msgrasp, "Failed to delete data");
                                    sqlite3_free(err_msg);
                                } else
                                    {
                                    /*printf("EROAAAARREEE!!!");*/
                                    bzero(msgrasp, 100);
                                    strcpy(msgrasp, "Stergere cu succes!");
                                    }
                            } else
                                {
                                bzero(msgrasp, 100);
                                strcpy(msgrasp, "Comentariu inexistent");
                                }

                        } else
                            {
                            bzero(msgrasp, 100);
                            strcpy(msgrasp, "Melodie inexistenta!");
                            }

                    } else
                        {
                        bzero(msgrasp, 100);
                        strcpy(msgrasp, "Nu sunteti logat sau nu aveti dreptul de a realiza aceasta comanda!");
                        }

                }
                //register user
                if (strstr (msg, "Register_user") != NULL)
                {
                    str[100]=""; sql[100]="";
                    char credentiale[100]="";
                    char x[100]="";
                    strncpy(x,msg,(strlen(msg)));
                    strcpy(credentiale,x+13);
                    /*printf("%s\n",credentiale);*/
                    char a[2][100];
                    int i = 1;
                    char *p = strtok(credentiale, "^");
                    /*printf("%s\n",p);*/
                    while (p != NULL)
                    {
                        strcpy(a[i], p);
                        /*printf("%s\n", a[i]);*/
                        p = strtok(NULL, "^");
                        i++;
                    }
                    sprintf (sql, "INSERT INTO User VALUES(%d,'%s',%s);",1,a[1],a[2]);
                    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
                    /*printf("%s",str);*/
                    if (rc != SQLITE_OK )
                    {
                        fprintf(stderr, "Failed to insert data\n");
                        fprintf(stderr, "SQL error: %s\n", err_msg);
                        bzero(msgrasp,100);
                        strcpy(msgrasp,"Failed to insert data");
                        sqlite3_free(err_msg);
                        return 1;
                    }
                    else
                    {
                        /*printf("%s\n",str);*/
                        bzero(msgrasp,100);
                        strcpy(msgrasp,"Pentru a continua, logati-va");
                        sqlite3_free(err_msg);

                    }
                }
                //register admin
                if (strstr (msg, "Register_admin") != NULL)
                {
                    str[100]="";
                    sql[100]="";
                    strcpy(type,"admin");
                    char credentiale[100]="";
                    char x[100]="";
                    strncpy(x,msg,(strlen(msg)));
                    strcpy(credentiale,x+14);
                    /*printf("%s\n",credentiale);*/
                    char a[2][100];
                    int i = 1;
                    char *p = strtok(credentiale, "^");
                    /*printf("%s\n",p);*/
                    while (p != NULL)
                    {
                        strcpy(a[i], p);
                        /*printf("%s\n", a[i]);*/
                        p = strtok(NULL, "^");
                        i++;
                    }
                    sprintf (sql, "INSERT INTO Admin VALUES(%d,'%s',%s);",count_a,a[1],a[2]);
                    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
                    /*printf("%s",str);*/
                    if (rc != SQLITE_OK )
                    {
                        fprintf(stderr, "Failed to insert data\n");
                        fprintf(stderr, "SQL error: %s\n", err_msg);
                        bzero(msgrasp,100);
                        strcpy(msgrasp,"Failed to insert data");
                        sqlite3_free(err_msg);
                        return 1;
                    }
                    else
                    {
                        /*printf("%s\n",str);*/
                        bzero(msgrasp,100);
                        strcpy(msgrasp,"Pentru a continua, logati-va");
                        sqlite3_free(err_msg);

                    }
                }

                if(strstr(msg, "quit")!=NULL)
                {
                    bzero(msgrasp, 100);
                    strcat(msgrasp,"Quitting...");
                    closing=1;

                }

                printf("[server]Trimitem mesajul inapoi...%s\n", msgrasp);

                /* returnam mesajul clientului */
                if (write(client, msgrasp, 100) <= 0)
                {
                    perror("[server]Eroare la write() catre client.\n");
                    continue;        /* continuam sa ascultam */
                } else
                    printf("[server]Mesajul a fost trasmis cu succes.\n");

                /* am terminat cu acest client, inchidem conexiunea */
                if (closing == 1)    //inchidere conexiune cu client
                {
                    close(client);
                    break;
                }
            }
            exit(0);
        }
    }				/* while */
}				/* main */