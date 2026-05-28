#ifndef PIPSH_PREFIX_H
#define PIPSH_PREFIX_H

typedef struct {
    char * username;
    char * hostname;
    char * cdir;
} Prefix;

int new_prefix(Prefix *);
void update_dir(Prefix *, char *);
void print_prefix(const Prefix *);
void free_prefix(const Prefix *);

#endif //PIPSH_PREFIX_H
