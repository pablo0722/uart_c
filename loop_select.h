#include <sys/time.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/time.h>
#include <unordered_map>

class LoopSelect {
    public:
        typedef void(*func_t)(void *arg);

    private:
        typedef struct {
            func_t func;
            void *arg;
        } fd_map_value_t;
        typedef std::unordered_map<int, fd_map_value_t *> fds_t;

    private:
        fds_t readfds;
        fds_t writefds;
        fds_t exceptfds;
        func_t timeout_func = NULL;
        void *timeout_arg = NULL;

    public:
        LoopSelect();
        ~LoopSelect();

        bool add_readfd(int fd, func_t func, void *arg, int *err_val = NULL, const char **err_msg = NULL);
        bool add_writefd(int fd, func_t func, void *arg, int *err_val = NULL, const char **err_msg = NULL);
        bool add_exceptfd(int fd, func_t func, void *arg, int *err_val = NULL, const char **err_msg = NULL);
        bool set_timeout_func(func_t timeout_func, void *arg, int *err_val = NULL, const char **err_msg = NULL);
        bool loop(bool *exit_cond, __time_t timeout_sec, __suseconds_t timeout_usec, int *err_val = NULL, const char **err_msg = NULL);

    private:
        void add_fd(int fd, func_t func, void *arg, fds_t *fds);
        void remove_fd(int fd, fds_t *fds);
        void clear_fds(fds_t *fds);
};
