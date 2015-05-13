#include <stdio.h>  // FILE, fread, and perror live here
#include <errno.h>  // errno lives here
#include <assert.h>
#include <stdlib.h>  // exit lives here, as do malloc and free
#include <string.h>  // strncpy lives here

#define MAX_DATA 512
#define MAX_ROWS 100

/*
 * High-level overview of this code
 * --------------------------------
 *
 * This code defines an executable which can perform exactly one of several
 * database operations: create a database ('c'), get - that is, print out - the
 * entry for an id ('g'), set an id to a particular Address ('s'), or delete an
 * id ('d'), or list all entries ('l').
 *
 * We create a persistent database which is written as a binary file. This
 * is very straightforward for our simple case because the size of the Address
 * and Database are both fixed in advance, so we can read and write the entire
 * Database in one shot without any branching logic.
 *
 * Notes
 * -----
 * I have a lot of notes in the code, but a few more: both fread and fwrite
 * accept a pointer of any type (actually void * in the definition) as the
 * first argument, and a size of the type as the second. In our case, the third
 * argument is always 1, because we are reading a single big static object.
 * But in general, the pointer you pass could be just the beginning of a big
 * block of memory you own, and the read/write command would write that whole
 * block as if your pointer were to the first entry of an array.
 *
 * If we modified the database to include a size, this is exactly what we would
 * likely do: write the size as an int or a long at the beginning, so that
 * we could read the size first, and then allocate an appropriate
 * block of memory and call fread on an Address pointer directly, passing it
 * the size.
 */

/* Data types */

// id is identifier (also equal to position in Db in this implementation)
// set is used as a bool: is this id in use?
// name and email are data
struct Address {
    int id;
    int set;
    char name[MAX_DATA];
    char email[MAX_DATA];
};

struct Database {
    struct Address rows[MAX_ROWS];
};

struct Connection {
    FILE *file;
    struct Database *db;
};

/* helper method to handle failures. perror knows how to interperet errno */
void die(const char *message) {
    if (errno) {
        perror(message);
    } else {
        printf("ERROR: %s\n", message);
    }
    exit(1);
}

/* pretty print address */
void Address_print(struct Address *addr) {
    printf("%d %s %s\n", addr->id, addr->name, addr->email);
}

/* load (fill out in-place) the Database inside a Connection
 *     assumes the Connection has already been opened (so the file is open and
 *     the Database is already allocated) */
void Database_load(struct Connection *conn) {
    int rc = fread(conn->db, sizeof(struct Database), 1, conn->file);
    if (rc != 1) die("Failed to load the database");
}

/* Creates a Connection, allocates the Database, opens the file, and
 * then loads information from the file to the Database if the mode isn't 'c'.
 * (all the other modes behave the same with respect to this function) */
struct Connection *Database_open(const char *filename, char mode) {
    struct Connection *conn = malloc(sizeof(struct Connection));
    if (!conn) die("Memory error");
    conn->db = malloc(sizeof(struct Database));
    if (!conn->db) die("Memory error");
    if (mode == 'c') {
        // "w" creates a new file, and truncates if it exists
        conn->file = fopen(filename, "w");
    } else {
        // "r+" allows both reading and appending. Positions at the beginning.
        conn->file = fopen(filename, "r+");
        if (conn->file) {
            Database_load(conn);
        }
    }
    if (!conn->file) die("Failed to open file");
    return conn;
}

/* Close a connection. Note that to do this safely we have to check every
 * entry for NULL. Also note that we don't free the file b/c fclose does */
void Database_close(struct Connection *conn) {
    if (conn) {
        if (conn->file) fclose(conn->file);
        if (conn->db) free(conn->db);
        free(conn);
    }
}

/* Write the Database in a Connection out to the file.
 * First we have to rewind the head to the beginning (because for all
 * operations other than create, we already read the contents). Then, we
 * directly write the memory contents out to the file as binary data. */
void Database_write(struct Connection *conn) {
    rewind(conn->file); // rewind is part of stdio.h, takes head 'back to 0'
    int rc = fwrite(conn->db, sizeof(struct Database), 1, conn->file);
    if (rc != 1) die("Failed to write database.");
    rc = fflush(conn->file); // flush ensures that buffered writes write
    if (rc == -1) die("Cannot flush database.");
}

/* Command for creating a database (once it has already been opened) */
void Database_create(struct Connection *conn) {
    int i;
    for (i = 0; i < MAX_ROWS; i++) {
        struct Address addr = { .id = i, .set = 0 };
        // note that this assignment is a copy of memory... probably not
        // the most efficient but it's very readable
        conn->db->rows[i] = addr;
    }
}

/* Command to set data for the Address with a given id */
void Database_set(struct Connection *conn, int id, const char *name,
        const char *email) {
    // note that the & has lower precedence than ->. It's modifying rows[id],
    // not conn.
    struct Address *addr = &conn->db->rows[id];
    if (addr->set) die("Already set this id, delete it first");
    addr->set = 1;
    // Using strncopy is operation is not safe because - assuming it's a valid
    // C string, if not all bets are off - `name` might be bigger than
    // MAX_DATA. Hence we manually make sure the last entry of addr->name is
    // '\0'.
    //  [It's still much better than strcopy, which has no limit on the writes]
    char *res = strncpy(addr->name, name, MAX_DATA);
    addr->name[MAX_DATA-1] = '\0';
    if(!res) die("Name copy failed");
    res = strncpy(addr->email, email, MAX_DATA);
    addr->email[MAX_DATA-1] = '\0';
    if(!res) die("Email copy failed");
}

/* Command to get data for a given id */
void Database_get(struct Connection *conn, int id) {
    struct Address *addr = &conn->db->rows[id];
    if (addr->set) {
        Address_print(addr);
    } else {
        die("ID is not set");
    }
}

/* Command to delete an address by id */
void Database_delete(struct Connection *conn, int id) {
    struct Address addr = { .id = id, .set = 0 };
    conn->db->rows[id] = addr;
}

void Database_list(struct Connection *conn) {
    int i;
    for (i = 0; i < MAX_ROWS; i++) {
        struct Address *addr = &conn->db->rows[i];
        if (addr->set) {
            Address_print(addr);
        }
    }
}

int main(int argc, char *argv[])
{
    if(argc < 3) die("USAGE: ex17 <dbfile> <action> [action params]");
    char *filename = argv[1];
    char action = argv[2][0];
    int id = 0;
    if(argc > 3) id = atoi(argv[3]);

    struct Connection *conn = Database_open(filename, action);
    if(id >= MAX_ROWS) die("There's not that many records.");

    switch(action) {
        case 'c':
            Database_create(conn);
            Database_write(conn);
            break;
        case 'g':
            if(argc != 4) die("Need an id to get");
            Database_get(conn, id);
            break;
        case 's':
            if(argc != 6) die("Need id, name, email to set");
            Database_set(conn, id, argv[4], argv[5]);
            Database_write(conn);
            break;
        case 'd':
            if(argc != 4) die("Need id to delete");
            Database_delete(conn, id);
            Database_write(conn);
            break;
        case 'l':
            Database_list(conn);
            break;
        default:
            die("Invalid action, only: c=create, g=get, s=set, d=del, l=list");
    }
    Database_close(conn);
    return 0;
}
