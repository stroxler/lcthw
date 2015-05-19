#include <unistd.h>
#include <apr_file_io.h>
#include <apr_errno.h>
#include <stdlib.h>
#include "db.h"
#include "bstrlib.h"
#include "dbg.h"

/* TO SEARCH BSTRINGLIB DOCS search this text file
 * https://raw.githubusercontent.com/websnarf/bstrlib/master/bstrlib.txt
 */

/* Note: this is the first use Zed makes of static functions.
 * The static keyword in a local variable means it is persistent.
 * The static keyword in a function def is completely unrelated: it means
 *     the function is private to the file (it basically ensures that if you
 *     tried to put the function in a header file, the compiler would barf).
 *   This provides a little bit of encapluation, and also reduces namespace
 *   clashes in big programs
 */

/* utility functions */

// renamed version of fopen
static FILE * DB_open(const char * filename, const char * mode) {
    return fopen(filename, mode);
}

// renamed version of flcose (except it doesn't return the err code)
static void DB_close(FILE * db) {
    fclose(db);
}

/* return the db contents as a bstring, or NULL if there's an error */
static bstring DB_load() {
    FILE *db = NULL;
    bstring data = NULL;
    // note: DB_FILE is #defined in db.h
    db = DB_open(DB_FILE, "r");
    check(db, "Failed to open the database: %s", DB_FILE);

    // this is a bit confusing to a newcommer.
    // Here's what's going on:
    //  we are calling the function
    //   `bstring bread(bNread readPtr, void * parm)`,
    // which takes a bNread-type function pointer. C allows us to cast db to
    // void *, but we have to explicitly cast the function fread to be of
    // type bNread.
    // ...
    // Why does bstrlib do this? Basically because it allows the bread function
    // to abstract away the read function and the read data source, because
    //  `typedef bNread (*bstring)(void *ptr, size_t sz, size_t n, void *dao)`
    // lets you use any function with approximately the same signature as fread
    // ...
    // Technically, the bstrlib.txt docs say that the cast (bNread)fread
    // doesn't quite work... you aren't supposed to be able to cast
    // the argument types of a function, by casting fread to type bNread,
    // we are forcing the File * stream arg to become void *. But it seems to
    // work.
    data = bread((bNread)fread, db);
    check(db, "Failed to read from db file: %s", DB_FILE);

    DB_close(db);
    return data;
error:
    if (db) DB_close(db);
    if (data) free(data);
    return NULL;
}

/* If `url` isn't found in the db, append it (and a newline) to the end.
 * returns 0 for normal run, -1 for error. */
int DB_update(const char *url) {
    if (DB_find(url)) {
       log_info("Already recorded as installed: %s", url);
       return 0;  // this isn't in Zed's version, but seems like it should be.
    }
    // I'm not really clear on why this is "a+" rather than just "a"...
    FILE *db = DB_open(DB_FILE, "a+");
    check(db, "Failed to open the database: %s", DB_FILE);
    // new functions:
    //  - `bstring bfromcstr(char * s): convert a (valid!) c string to b string
    //  - `int bconchar(bstring b, char c)`: concat c to end of b.
    //  - `int blength(bstring b)`: return the length; 0 if NULL
    bstring line = bfromcstr(url);
    bconchar(line, '\n');
    int rc = fwrite(line->data, blength(line), 1, db);
    check(rc == 1, "Failed to append to the db."); // check: why is this 1?
    return 0;
error:
    if (db) DB_close(db);
    return -1;
}

/* returns -1 for error, 0 if url not found, 1 if url found in the db */
int DB_find(const char * url) {
    bstring data = NULL;
    bstring line = bfromcstr(url);
    int res = -1;
    data = DB_load();
    check(data, "Failed to load: %s", DB_FILE);
    // new function: `int binstr(const_bstring s1, int pos, const_bstring s2)`
    //               searches `s1`, starting at position `pos`, for `s2`.
    //               Returns BSTR_ERR if it fails to find a match, otherwise
    //               it returns the match index in s1.
    //    (uses brute force)
    if (binstr(data, 0, line) == BSTR_ERR) { res = 0; }
    else { res = 1; }
    // fallthrough to the error block!
error:
    // new function: `int bdestroy(bstring b)` : destroy a bstr. It returns
    //                an error code, although usually we don't really care.
    if (data) bdestroy(data);
    if (line) bdestroy(line);
    return res;
}


int DB_init() {

    // an apr_pool_t is basically a managed memory session: you can allocate
    // memory from it, and then free it to free all the objects you allocated.
    //   Why are we using it? Because apr relies on it, at least for now, to do
    // internal memory management (the author said this is a mistake and might
    // change in the future)
    apr_pool_t *p = NULL;
    // the docs say you don't actually need to call this, you should call
    // apr_initialize() instead, but I'm trusting Zed here. Inits internal
    // structure of the apr_pool system.
    apr_pool_initialize();
    // `void apr_pool_create(apr_pool_t **new, apr_pool_t* parent)` sets
    // up a new pool inside `parent`, or if `parent` is NULL a root pool.
    apr_pool_create(&p, NULL);

    /* STEP 1: MAKE SURE DB_DIR EXISTS
     * ------------------------------- */
    // new library func: `int access(const char *path, int amode)` in
    // unistd.h: the path is a path, the amode is either F_OK, which checks
    // that the file exists, or a bitwise or of any combo of R_OK, W_OK, X_OK,
    // which checks whether this pid has all of the permissions you are asking.
    // (note that it's bitwise or, but logically an 'and' in terms of prmssns)
    // ... here we are using to check that the directory exists [implicit], is
    //     executable (meaning we can look inside it) and is writeable.
    // returns 0 for success, -1 for error. God I hate C sometimes...
    //     by the way, this may be the first unix-specific system call we are
    // making. I think most of the others would have worked in windows.
    if (access(DB_FILE, W_OK | X_OK) == -1) {
        // this function is apr's version of mkdir -p plus a chmod op.
        // The bitwise-or is really a logical and: it says give all permissions
        // to user and group.
        //  defntn: `apr_status_t apr_dir_make_recursive(
        //        const char *pth, apr_fileperms_t perm, apr_pool_t * pool)`
        apr_status_t rc = apr_dir_make_recursive(
                DB_DIR, (APR_UREAD | APR_UWRITE | APR_UEXECUTE |
                         APR_GREAD | APR_GWRITE | APR_GEXECUTE), p
        );
        check(rc = APR_SUCCESS, "Failed to make database dir: %s", DB_DIR);
    }

    /* STEP 2: MAKE SURE DB_FILE EXISTS
     * -------------------------------- */
    if (access(DB_FILE, W_OK) == -1) {
        // to 'touch' a file in C, just open it with "w" and close it.
        FILE * db = DB_open(DB_FILE, "w");
        check(db, "Cannot open database file: %s", DB_FILE);
        DB_close(db);
    }

    // this frees the memory pool - it cleans up any apr resources created.
    apr_pool_destroy(p);
    return 0;
error:
    apr_pool_destroy(p);
    return(-1);
}

int DB_list() {
    bstring data = DB_load();
    check(data, "Failed to read load: %s", DB_FILE);
    // new function: `char * bdata (bstring b)` return the data portion of a
    //               bstring, NULL if NULL.
    printf("%s", bdata(data));
    bdestroy(data);
    return 0;
error:
    return -1;
}

