#include <stdio.h>
#include <apr_general.h>
#include <apr_getopt.h>
#include <apr_strings.h>
#include <apr_lib.h>

#include "dbg.h"
#include "db.h"
#include "commands.h"


// the const char const *argv says you can't modify any of the pointers
// pointed to by argv (which is basically of type char **), *and* you can't
// modify the data pointed to by any of those pointers.
//
// the 'const char' part is what makes the data immutable
//   (you can see this below: we make const char * pointers which we *can*
//    modify to point to new data - we just aren't allowed to mutate the
//    data via these pointers)
// the const * is what makes the pointers themselves immutable.
int main(int argc, const char const *argv[]) {

    // this is the usual apr pool init you need to use apr. See db.c for disc.
    apr_pool_t *p = NULL;
    apr_pool_initialize();
    apr_pool_create(&p, NULL);

    // apr_status_t is used for err codes.
    // apr_getopt_t is used for apr get opt code, which is sort of a C
    //   equivalent of python's argparse - a tool to make argument parsing
    //   much easier.
    // apr_getopt_init is sets up opt so that we can use apr_getopt later.
    apr_getopt_t *opt;
    apr_status_t rv;
    rv = apr_getopt_init(&opt, p, argc, argv);
    check(rv == APR_SUCCESS, "Failed to initialize apr option structure.");

    // data for argument parsing
    char ch = '\0'; // used to store the parsed option flag
    const char *optarg = NULL; // used to store any option arguments

    // data to send to commands
    const char *url = NULL;
    const char *config_opts = NULL;
    const char *make_opts = NULL;
    const char *install_opts = NULL;
    enum CommandType request = COMMAND_NONE;

    // func: `apr_status_t apr_getopt(apr_getopt_t *opt, char * fmt,
    //                                char * flag, char **arg)
    // what does it do? It parses the command line args which have been
    // put into `opt` via apr_getopt_init. All of the options must have
    // single-character flags which are given in `fmt`; a colon following an
    // option says that that option takes a string argument.
    //    The option is stored in `flag`, and the argument, if `fmt` says
    // there is one, is stored in `arg`. So here `ch` gets the flag and
    // `optarg` gets any arguments.
    while(apr_getopt(opt, "SLI:L:F:B:c:m:i:", &ch, &optarg) == APR_SUCCESS) {
        // - the commands are S = init, L = list, I = install, F = fetch,
        //   and B = build. All of them except S and L take a url argument.
        // - the options -c, -m, and -i all take an argument; they are used to
        //   add arguments to ./config, make, and install respectively.
        //   Each of these can only be used once, so if you need to add
        //   multiple options you need to make sure they get tokenized as a
        //   single argument by using quotes.
        switch(ch) {
            case 'S':
                request = COMMAND_LIST;
                break;
            case 'L':
                request = COMMAND_LIST;
                break;
            case 'F':
                request = COMMAND_FETCH;
                url = optarg;
                break;
            case 'B':
                request = COMMAND_BUILD;
                url = optarg;
                break;
            case 'I':
                request = COMMAND_INSTALL;
                url = optarg;
                break;
            case 'c':
                config_opts = optarg;
                break;
            case 'm':
                make_opts = optarg;
                break;
            case 'i':
                install_opts = optarg;
                break;
        }
        switch(request) {
            case COMMAND_INIT:
                rv = DB_init();
                check(rv == 0, "Failed to make database.");
                break;
            case COMMAND_LIST:
                DB_list();
                break;
            case COMMAND_FETCH:
                check(url != NULL, "You must give a URL.");
                Command_fetch(p, url, 1);
                log_info("Downloaded to %s and in /tmpt/", BUILD_DIR);
                break;
            case COMMAND_BUILD:
                check(url != NULL, "You must give a URL.");
                Command_build(p, url, config_opts, make_opts, install_opts);
                break;
            case COMMAND_INSTALL:
                check(url != NULL, "You must give a URL.");
                Command_install(p, url, config_opts, make_opts, install_opts);
                break;
            default:
                sentinel("Invalid command given.");
        }
    }
    return 0;
error:
    // note once again that in main we don't need to clean up. In fact
    // we don't even destroy p!
    return 1;
}
