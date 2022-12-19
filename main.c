#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>

#include "MySqlUtil.c"

int main(int argc, char *argv[])
{
    char temp_buf[USERNAMELENGHT];
    char username[USERNAMELENGHT];
    char password[USERNAMELENGHT];
    int count, temp;

    struct mySQLConnection* LSO2223 = NULL;

    LSO2223 = establishDBConnection();

    count = 5;
    while(count--) {
        printf("\t\tUsername: ");
        fgets(username, sizeof(username), stdin);
        username[strlen(username) - 1] = '\0';

        printf("\t\tPassword: ");
        fgets(password, sizeof(password), stdin);
        password[strlen(password) - 1] = '\0';

        printf("\t\tMAIN: Select 1 for SelectQuery, 0 for InsertQuery\n"
               "\t\tMode: ");
        fgets(temp_buf, sizeof(temp_buf), stdin);
        temp_buf[strlen(temp_buf) - 1] = '\0';
        temp = atoi(temp_buf);
        if(temp) {
            if((temp = selectQuery(LSO2223, username, password)) == 1)
                printf("MAIN: Utente presente.\n");
            else if(temp == 0)
                printf("MAIN: Utente non presente.\n");
            else
                printf("MAIN: Errore del select.\n");
        }
        else if(temp == 0) {
            if((temp = insertQuery(LSO2223, username, password)) == 1)
                printf("MAIN: Inserimento riuscito.\n");
            else if(temp == 0)
                printf("MAIN: Inserimento non riuscito.\n");
            else if(temp == 1062)
                printf("MAIN: Valore duplicato.\n");
            else
                printf("MAIN: Errore del insert.\n");
        }
        else {
            printf("MAIN: Comando non riconosciuto\n.");
        }
        printf("\n");
    }

    free(LSO2223);
    mysql_close(LSO2223->connection);

    return 0;
}