#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "magic_nums.h"

typedef struct {
    char * username;
    char * hostname;
    char * cdir;
} Prefix;

int new_prefix(Prefix * result) {

    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    if (pw == NULL) {
        return 1;
    }

    char hostname[HOSTNAME_LENGTH];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        char *dot = strchr(hostname, '.');
        if (dot)
            *dot = '\0';
    } else {
        return 1;
    }

    char dirbuf[PATH_LENGTH];
    getcwd(dirbuf, sizeof(dirbuf));
    char *last_slash = strrchr(dirbuf, '/');
    char* curdir = last_slash ? last_slash + 1 : (char *)dirbuf;

    Prefix p = {
        strdup(pw->pw_name),
        strdup(hostname),
        strdup(curdir)
    };

    *result = p;

    return 0;
}

void update_dir(Prefix * target, char * dirbuf) {

    free(target->cdir);
    char * last_slash = strrchr(dirbuf, '/');
    char * curdir = last_slash ? last_slash + 1 : dirbuf;

    target->cdir = strdup(curdir);
}

void print_prefix(const Prefix * target) {
    printf("%s@%s %s &: ", target->username, target->hostname, target->cdir);
}

void free_prefix(const Prefix * target) {
    free(target->username);
    free(target->hostname);
    free(target->cdir);
}
