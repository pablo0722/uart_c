#include <signal.h>
#include <stdbool.h>
#include <stddef.h>

void register_signal(int signum, bool *var, int *err_val = NULL, const char **err_msg = NULL);