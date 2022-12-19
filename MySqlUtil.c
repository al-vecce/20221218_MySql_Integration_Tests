#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>

#define USERNAMELENGHT 100
#define QUERYLENGHT 1024

struct mySQLConnection {
    MYSQL *connection;
    MYSQL_RES *result;
    MYSQL_ROW row;

    char hostname[USERNAMELENGHT];
    char port[USERNAMELENGHT];
    char username[USERNAMELENGHT];
    char password[USERNAMELENGHT];
    char database[USERNAMELENGHT];
};

/* Query per vedere se username e password sono presenti nel database.
 * Restituisce 0 in caso di successo, 1 in caso di fallimento, o un intero
 * negativo per indicare una situazione d'errore. */
int selectQuery(struct mySQLConnection* LSO2223, char username[], char password[]) {
    char query[QUERYLENGHT];
    int ret_value;
    unsigned int num_fields;
    unsigned long *field_lengths;

    ret_value = 0;

    if(LSO2223 == NULL) {
        fprintf(stderr, ":DB_STRUCT ERROR: struct is not initialized");
        return -1;
    }

    // Composizione query.
    sprintf(query, "SELECT * "
                   "FROM Profile "
                   "WHERE username = '%s';", username);
    //printf("DEBUG: query \"%s\".\n", query);

    // Esecuzione query.
    if (mysql_query(LSO2223->connection, query)) {
        fprintf(stderr, ":DB_STRUCT ERROR: error while querying server \"%s\"\n", mysql_error(LSO2223->connection));
        return -1;
    }

    // Ricezione dei record.
    LSO2223->result = mysql_use_result(LSO2223->connection);
    if (LSO2223->result == NULL) {
        return 0;
    }

    // Controlla tutti i record.
    num_fields = mysql_num_fields(LSO2223->result);
    while ((LSO2223->row = mysql_fetch_row(LSO2223->result))) {
        field_lengths = mysql_fetch_lengths(LSO2223->result);
        if (field_lengths == NULL) {
            fprintf(stderr, ":DB_STRUCT ERROR: failed to get field lengths \"%s\"\n", mysql_error(LSO2223->connection));
            return -1;
        }
        if(strcmp(LSO2223->row[0], username) != 0) {
            //printf("DEBUG: Wrong username \"%s\" - \"%s\".\n", LSO2223->row[0], username);
        }
        else if (strcmp(LSO2223->row[1], password) != 0) {
            //printf("DEBUG: Wrong password \"%s\" - \"%s\".\n", LSO2223->row[1], password);
        }
        else {
            //printf("DEBUG: correct.\n");
            ret_value = 1;
        }
        /*for (int i = 0; i < num_fields; i++) {
            printf("[%.*s] ", (int) field_lengths[i], LSO2223->row[i] ? LSO2223->row[i] : "NULL");
        }*/
    }

    mysql_free_result(LSO2223->result);

    return ret_value;
}

/* Query per inserire una nuova riga al database. Controlla che la riga
 * non sia già presente con una chiamata a selectQuery.
 * Restituisce 0 in caso di successo, 1 in caso di fallimento, o un intero
 * negativo per indicare una situazione d'errore. */
int insertQuery(struct mySQLConnection* LSO2223, char username[], char password[]) {
    char query[QUERYLENGHT];
    int ret_value;
    unsigned int num_fields;
    unsigned long *field_lengths;

    ret_value = 0;

    if(LSO2223 == NULL) {
        fprintf(stderr, ":DB_STRUCT ERROR: struct is not initialized");
        return -1;
    }

    // Esecuzione query di controllo per verificare che il campo non esista.

    // Composizione query.
    sprintf(query, "INSERT INTO Profile "
                   "VALUES ('%s', '%s');", username, password);
    printf("DEBUG: query \"%s\".\n", query);

    // Esecuzione query.
    if (mysql_query(LSO2223->connection, query)) {
        fprintf(stderr, ":DB_STRUCT ERROR: error while querying server \"%s\"\n", mysql_error(LSO2223->connection));
        if((ret_value = mysql_errno(LSO2223->connection)) == 1062) {
            //printf("Duplicate Key\n");
        }
        else {
            //printf("%d\n", ret_value);
            ret_value = -1;
        }
    }
    else {
        ret_value = 1;
    }

    return ret_value;
}

/* Inizializzatore della connessione col database.
 * Alloca dinamicamente lo spazio per la struttura della connessione.
 * Restituisce il puntatore alla struttura appena creata */
struct mySQLConnection* establishDBConnection(void) {
    struct mySQLConnection* LSO2223 = NULL;
    while(LSO2223 == NULL) {
        if ((LSO2223 = (struct mySQLConnection *) malloc(sizeof(struct mySQLConnection))) == NULL) {
            perror(":MALLOC DBCONNECTION");
            exit(1);
        } else {
            printf("MAIN: Inizializzazione connessione al database. Prego inserire i dati richiesti...\n");

            //strcpy(LSO2223->hostname, "localhost");
            //strcpy(LSO2223->port, "3306");
            //strcpy(LSO2223->username, "osboxes");
            //strcpy(LSO2223->password, "password");
            //strcpy(LSO2223->database, "LSO2223_Database");

            printf("\t\tIndirizzo del server: ");
            fgets(LSO2223->hostname, sizeof(LSO2223->hostname), stdin);
            LSO2223->hostname[strlen(LSO2223->hostname) - 1] = '\0';

            printf("\t\tPorta del server: ");
            fgets(LSO2223->port, sizeof(LSO2223->port), stdin);
            LSO2223->port[strlen(LSO2223->port) - 1] = '\0';

            printf("\t\tUsername: ");
            fgets(LSO2223->username, sizeof(LSO2223->username), stdin);
            LSO2223->username[strlen(LSO2223->username) - 1] = '\0';

            printf("\t\tPassword: ");
            fgets(LSO2223->password, sizeof(LSO2223->password), stdin);
            LSO2223->password[strlen(LSO2223->password) - 1] = '\0';

            printf("\t\tDatabase: ");
            fgets(LSO2223->database, sizeof(LSO2223->database), stdin);
            LSO2223->database[strlen(LSO2223->database) - 1] = '\0';

            /* Inizializzazione della connessione */
            LSO2223->connection = mysql_init(NULL);
            if (LSO2223->connection == NULL) {
                fprintf(stderr, ":DB_CONN_OBJ FAILED TO INITIALIZE: %s\n", mysql_error(LSO2223->connection));
                printf("MAIN: Tentativo di connessione fallito. Riprovare.\n");
                free(LSO2223);
                LSO2223 = NULL;
                continue;
            }

            /* printf("\nTentativo di connessione con le seguenti credenziali:\n"
                   "\t\tHostname: %s\n"
                   "\t\tPort: %s\n"
                   "\t\tUsername: %s\n"
                   "\t\tPassword: %s\n"
                   "\t\tDatabase: %s\n",
                   LSO2223->hostname, LSO2223->port, LSO2223->username, LSO2223->password, LSO2223->database); */

            /* Tentativo di connessione */
            if (!mysql_real_connect(LSO2223->connection, LSO2223->hostname, LSO2223->username, LSO2223->password,
                                    LSO2223->database,
                                    atoi(LSO2223->port), NULL, 0)) {
                fprintf(stderr, ":DB_SERVER CONNECT ERROR: %s\n", mysql_error(LSO2223->connection));
                printf("MAIN: Tentativo di connessione fallito. Riprovare.\n");
                free(LSO2223);
                LSO2223 = NULL;
                continue;
            }

            printf("\nMAIN: Connessione riuscita.\n");
        }
    }
    return LSO2223;
}

