#include <stdlib.h>
#include <stdio.h>
#include <error.h>

#include "kvm/util.h"
#include "kvm/rvaf_help.h"
#include "kvm/rvaf_run.h"

struct cmd_struct {
    const char *cmd_name;
    int (*fn)(int, const char **, const char *);
    void (*help)(void);
    int option;
};

static struct cmd_struct kvm_commands[] = {
    //{"list",    kvm_cmd_list,   kvm_list_help,  0},
    {"help",    kvm_cmd_help,   NULL,           0},
    {"run",     kvm_cmd_run,    NULL,           0},
    {NULL,      NULL,           NULL,           NULL},
};

static struct cmds_struct *
kvm_get_command(struct cmd_struct *cmds, const char *name)
{
    struct cmd_struct *p = cmds;
    
    while (p->cmd_name) {
        if (!strcmp(p->cmd_name, name))
            return p;
        p++;
    }
    return NULL;
}

/**
 *   ./lkvm-static run -m 128 -c2 \
 *      --console serial -p "console=ttyS0 earlycon" \
 *      -k ./apps/Image --debug
 */
static int
handle_kvm_command(struct cmd_struct *cmds, int argc, const char **argv)
{
    int ret = 0;
    struct cmd_struct *p;
    const char *prefix = NULL;
    
    if (!argv || !*argv) {
        p = kvm_get_command(cmds, "help");
        BUG_ON(!p);
        return p->fn(argc, argv, prefix);
    }

    ret = p->fn(argc - 1, &argv[1], prefix);
    if (ret < 0) {
        if (errno == EPERM)
            die("Permission error => are you root? :)");
    }

    return ret;
}

int main(int argc, char *argv[])
{
    return handle_kvm_command(kvm_commands, argc - 1,
                (const char **) &argv[0]);
}
