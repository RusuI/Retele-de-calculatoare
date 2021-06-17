
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

extern int errno;

int port;

int main (int argc, char *argv[])
{
  int sd;
  struct sockaddr_in server;
  char msg[200],username[100]="", password[100];

  if (argc != 3)
    {
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }

  port = atoi (argv[2]);

  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return errno;
    }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(argv[1]);
  server.sin_port = htons (port);

  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return errno;
    }
    printf ("--------------------------> PROGRAM STARTED <--------------------------;\n");
    printf ( "                       Welcome to TopMusic!\n");
    printf("          MENIU                 \n");
    printf("1.Login_utilizator\n");
    printf("2.Login_admin\n");
    printf("3.Inregistrare_utilizator\n");
    printf("4.Inregistrare_admin\n");
    printf("5.Comenzi_utilizator\n");
    printf("6.Comenzi_administrator\n");
    printf("7.quit\n");


while(1)
{  int sw=0;
    bzero(msg, 100);
    printf("Introduceti o comanda: ");
    fflush(stdout);
    //citirea de la tastatura
    read(0, msg, 100);
    if(strstr(msg,"Comenzi_administrator")!=NULL)
    {
        printf(" 1.Afisare_comentarii\n 2.Stergere_comentariu\n 3.Restrictionare_vot\n 4.Sterge_melodie\n");
        sw=1;
    }
        else if(strstr(msg,"Comenzi_utilizator")!=NULL)
    {
        printf(" 1.Afisare_top\n 2.Afisare_gen_top\n 3.Votare\n 4.Adaugare_melodie\n 5.Adaugare_comentarii\n 6.Link_melodie\n");
        sw=1;
    }
        else if(strstr(msg,"Link_melodie")!=NULL)
    {
        char mesaj[100];
        bzero(mesaj, 100);
        strcat(mesaj, "Link_melodie");

        //titlul melodiei
        bzero(username, 100);
        printf("Introduceti numele melodiei:");
        fflush(stdout);
        read(0, username, 100);
        username[strlen(username)-1]='\0';
        strcat(mesaj,username);
        bzero(msg,100);
        strcpy(msg,mesaj);
    }
        else if(strstr(msg, "Login_utilizator")!=NULL)
   {
        char mesaj[100] = "";
        bzero(mesaj, 100);
        strcat(mesaj, "Login_utilizator");
        //username-ul
        bzero(username, 100);
        printf("Introduceti numele:");
        fflush(stdout);
        read(0, username, 100);
        username[strlen(username) - 1] = '\0';
        /*printf(".%s.\n",username);*/
        strcat(mesaj, username);
        strcat(mesaj, "^");

        //parola
        printf("Introduceti parola:");
        bzero(password, 100);
        fflush(stdout);
        read(0, password, 100);
        password[strlen(password) - 1] = '\0';
        strcat(mesaj, password);
        bzero(msg, 100);
        strcpy(msg, mesaj);
        /*printf("%s\n", msg);//mesajul care este trimis la server, de forma cmd+user+parola*/
    }
        else if(strstr(msg,"Adaugare_melodie")!=NULL)
     {
         char mesaj[200]="", descriere[100]="",gen[100]="",link[100]="";
         bzero(mesaj, 200);
         strcat(mesaj, "Adaugare_melodie");

         //titlul melodiei
         bzero(username, 100);
         printf("Introduceti numele melodiei:");
         fflush(stdout);
         read(0, username, 100);
         username[strlen(username)-1]='\0';
         strcat(mesaj,username);
         strcat(mesaj,"^");

         //descrierea melodiei
         bzero(descriere, 100);
         printf("Introduceti o descriere:");
         fflush(stdout);
         read(0, descriere, 100);
         descriere[strlen(descriere)-1]='\0';
         strcat(mesaj,descriere);
         strcat(mesaj,"^");

         //genul
         bzero(gen, 100);
         printf("Introduceti genul:");
         fflush(stdout);
         read(0, gen, 100);
         gen[strlen(gen)-1]='\0';
         strcat(mesaj,gen);
         strcat(mesaj,"^");

         //link
         bzero(gen, 100);
         printf("Introduceti link-ul:");
         fflush(stdout);
         read(0, link, 100);
         link[strlen(link)-1]='\0';
         strcat(mesaj,link);
         //lipim la mesajul final
         strcpy(msg, mesaj);
        /* printf("%s\n", msg);*/

     }
        else if(strstr(msg,"Votare_melodie")!=NULL)
        {
            char mesaj[100]="";
            bzero(mesaj, 100);
            strcat(mesaj, "Vot_song");
            //username-ul
            bzero(username, 100);
            printf("Introduceti numele dumneavoastra de utilizator:");
            fflush(stdout);
            read(0, username, 100);
            username[strlen(username)-1]='\0';
            /*printf(".%s.\n",username);*/
            strcat(mesaj,username);
            strcat(mesaj,"^");

            //numele melodiei
            char song[100]="";
            printf("Introduceti titlul melodiei:");
            bzero(song,100);
            fflush(stdout);
            read(0, song, 100);
            song[strlen(song)-1]='\0';
            strcat(mesaj,song);
            bzero(msg,100);
            strcpy(msg, mesaj);
            /*printf("%s\n", msg);//mesajul care este trimis la server, de forma cmd+user+parola*/


        }
        else if(strstr(msg,"Afisare_top")!=NULL)
        {
        char mesaj[100]="";
        bzero(mesaj, 100);
        strcat(mesaj, "Afisare_top");
        bzero(msg,100);
        strcpy(msg, mesaj);
        /*printf("%s\n", msg);*/
        }
        else if(strstr(msg,"Afisare_gen_top")!=NULL)
        {
           char mesaj[100]="";
           bzero(mesaj, 100);
           strcat(mesaj, "Afisare_gen");
           //genul muzical:
           bzero(username, 100);
           printf("Introduceti genul:");
           fflush(stdout);
           read(0, username, 100);
           username[strlen(username)-1]='\0';
           strcat(mesaj,username);
           bzero(msg,100);
           strcpy(msg, mesaj);
           /*printf("%s\n", msg);*/
           /*printf(".%s.\n",username);*/

        }
        else if(strstr(msg,"Adaugare_comentarii")!=NULL)
        {
         char mesaj[100]="", descriere[100]="";
         bzero(mesaj, 100);
         strcat(mesaj, "Adaugare_comentarii");

         //titlul melodiei
         bzero(username, 100);
         printf("Introduceti numele melodiei:");
         fflush(stdout);
         read(0, username, 100);
         username[strlen(username)-1]='\0';
         strcat(mesaj,username);
         strcat(mesaj,"^");

         //descrierea melodiei
         bzero(descriere, 100);
         printf("Introduceti un comentariu:");
         fflush(stdout);
         read(0, descriere, 100);
         descriere[strlen(descriere)-1]='\0';
         strcat(mesaj,descriere);
         bzero(msg,100);
         strcpy(msg, mesaj);
        }
        else if(strstr(msg, "Login_admin")!=NULL)
        {
            char mesaj[100]="";
            bzero(mesaj, 100);
            strcat(mesaj, "Login_admin");
            //username-ul
            bzero(username, 100);
            printf("Introduceti numele:");
            fflush(stdout);
            read(0, username, 100);
            username[strlen(username)-1]='\0';
            /*printf(".%s.\n",username);*/
            strcat(mesaj,username);
            strcat(mesaj,"^");

            //parola
            printf("Introduceti parola:");
            bzero(password,100);
            fflush(stdout);
            read(0, password, 100);
            password[strlen(password)-1]='\0';
            strcat(mesaj,password);
            bzero(msg,100);
            strcpy(msg, mesaj);
            /*printf("%s\n", msg);//mesajul care este trimis la server, de forma cmd+user+parola*/


        }
        else if(strstr(msg, "Inregistrare_admin")!=NULL)
    {
        char mesaj[100]="";
        bzero(mesaj, 100);
        strcat(mesaj, "Register_admin");
        //username-ul
        bzero(username, 100);
        printf("Introduceti numele de inregistrare:");
        fflush(stdout);
        read(0, username, 100);
        username[strlen(username)-1]='\0';
        /*printf(".%s.\n",username);*/
        strcat(mesaj,username);
        strcat(mesaj,"^");

        //parola
        printf("Introduceti parola de inregistrare:");
        bzero(password,100);
        fflush(stdout);
        read(0, password, 100);
        password[strlen(password)-1]='\0';
        strcat(mesaj,password);
        bzero(msg,100);
        strcpy(msg, mesaj);
       /* printf("%s\n", msg);//mesajul care este trimis la server, de forma cmd+user+parola*/


    }
        else if(strstr(msg, "Inregistrare_utilizator")!=NULL)
    {
        char mesaj[100]="";
        bzero(mesaj, 100);
        strcat(mesaj, "Register_user") ; //username-ul
        bzero(username, 100);
        printf("Introduceti numele de inregistrare:");
        fflush(stdout);
        read(0, username, 100);
        username[strlen(username)-1]='\0';
        /*printf(".%s.\n",username);*/
        strcat(mesaj,username);
        strcat(mesaj,"^");

        //parola
        printf("Introduceti parola de inregistrare:");
        bzero(password,100);
        fflush(stdout);
        read(0, password, 100);
        password[strlen(password)-1]='\0';
        strcat(mesaj,password);
        bzero(msg,100);
        strcpy(msg, mesaj);
        /*printf("%s\n", msg);//mesajul care este trimis la server, de forma cmd+user+parola*/


    }
        else if(strstr(msg,"Restrictionare_vot"))
    {
        char mesaj[100]="";
        bzero(mesaj, 100);
        strcat(mesaj, "Restrictionare_vot");
        bzero(username, 100);
        printf("Introduceti numele utilizatorului:");
        fflush(stdout);
        read(0, username, 100);
        username[strlen(username)-1]='\0';
        /*printf(".%s.\n",username);*/
        strcat(mesaj,username);
        bzero(msg,100);
        strcpy(msg, mesaj);
        /*printf("%s\n", msg); //mesajul care este trimis la server, de forma cmd+user+parola*/

    }
        else if(strstr(msg,"Stergere_melodie")!=NULL)
    {
        char mesaj[100]="";
        bzero(mesaj, 100);
        strcat(mesaj, "Stergere_melodie");
        bzero(username, 100);
        printf("Introduceti titlul melodiei:");
        fflush(stdout);
        read(0, username, 100);
        username[strlen(username)-1]='\0';
        /*printf(".%s.\n",username);*/
        strcat(mesaj,username);
        bzero(msg,100);
        strcpy(msg, mesaj);
        /*printf("%s\n", msg); //mesajul care este trimis la server, de forma cmd+user+parola*/

    }
        else if(strstr(msg,"Stergere_comentariu")!=NULL)
    {
        char mesaj[100]="";
        bzero(mesaj, 100);
        strcat(mesaj, "Stergere_com");
        bzero(username, 100);
        printf("Introduceti titlul melodiei:");
        fflush(stdout);
        read(0, username, 100);
        username[strlen(username)-1]='\0';
        /*printf(".%s.\n",username);*/
        strcat(mesaj,username);
        strcat(mesaj,"^");
        //ID
        printf("Introduceti numarul comentariului:");
        bzero(password,100);
        fflush(stdout);
        read(0, password, 100);
        password[strlen(password)-1]='\0';
        strcat(mesaj,password);
        bzero(msg,100);
        strcpy(msg, mesaj);
       /* printf("%s\n", msg);//mesajul care este trimis la server, de forma cmd+user+parola*/

    }
        else if(strstr(msg,"Afisare_comentarii")!=NULL)
    {
        char mesaj[100]="";
        bzero(mesaj, 100);
        strcat(mesaj, "Afisare_com");
        bzero(username, 100);
        printf("Introduceti titlul melodiei:");
        fflush(stdout);
        read(0, username, 100);
        username[strlen(username)-1]='\0';
        /*printf(".%s.\n",username);*/
        strcat(mesaj,username);
        bzero(msg,100);
        strcpy(msg, mesaj);
        /*printf("%s\n", msg);*/
    }
        else if(strstr(msg,"quit")==NULL)
    {
        printf("Comanda neinregistrata.\nIncercati din nou!\n");
        sw=1;


    }

    //trimitere comanda la server
    if (write(sd, msg, 100) <= 0) {
        perror("[client]Eroare la write() spre server.\n");
        return errno;
    }

    //citire raspuns de la server
    if (read(sd, msg, 100) < 0) {
        perror("[client]Eroare la read() de la server.\n");
        return errno;
    }

    /*char msg1[100]="";*/
    /*strncpy(msg1,msg,strlen(msg));*/
    if(sw==0)
    printf("%s\n", msg);
    if(strcmp(msg,"Quitting...")==0)
        break;
}
  close (sd);
  return 0;
}