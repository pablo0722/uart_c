#include <stdio.h>
#include <signal.h>

#define MAX_SIGNALS 5

typedef struct {
    int signum;
    bool *var;
} signum_var_map_t;

static signum_var_map_t signum_var_map[MAX_SIGNALS];
static int signals_registered = 0;

static void signal_callback_handler(int signum) {
    printf("\nSignal %d catched\n", signum);
    for(int i=0; i<signals_registered; i++){
        if(signum_var_map[i].signum == signum){
            (*signum_var_map[i].var) = true;
        }
    }
}

void register_signal(int signum, bool *var, int *err_val = NULL, const char **err_msg = NULL) {
    if(signals_registered >= MAX_SIGNALS){
        if(NULL != err_msg) (*err_msg) = "[ERROR] signal_get_init: MAX_SIGNALS reached";
        if(NULL != err_val) (*err_val) = -1;

        return;
    }

    if(NULL == var)
    {
        if(NULL != err_msg) (*err_msg) = "[ERROR] signal_get_init: var is NULL";
        if(NULL != err_val) (*err_val) = -2;

        return;
    }

    (*var) = false;

    signum_var_map[signals_registered].signum = signum;
    signum_var_map[signals_registered].var = var;
    signals_registered++;

    signal(signum, signal_callback_handler);

    if(NULL != err_msg) (*err_msg) = NULL;
    if(NULL != err_val) (*err_val) = 0;

    return;
}