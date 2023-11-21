#include "loop_select.h"

#include <string.h>

LoopSelect::LoopSelect() {
}

LoopSelect::~LoopSelect() {
    clear_fds(&this->readfds);
    clear_fds(&this->writefds);
    clear_fds(&this->exceptfds);
}

void LoopSelect::add_fd(int fd, func_t func, void *arg, fds_t *fds) {
    remove_fd(fd, fds);
    fd_map_value_t *value = new fd_map_value_t;
    value->func = func;
    value->arg = arg;
    (*fds)[fd] = value;
}

void LoopSelect::remove_fd(int fd, fds_t *fds) {
    if((*fds).find(fd) != (*fds).end()) {
        delete (*fds)[fd];
        (*fds)[fd] = NULL;
    }
    (*fds).erase(fd);
}

void LoopSelect::clear_fds(fds_t *fds) {
    for(auto &it: *fds) {
        delete it.second;
        it.second = NULL;
    }
    (*fds).clear();
}

bool LoopSelect::add_readfd(int fd, func_t func, void *arg, int *err_val, const char **err_msg) {
    if(err_val) *err_val = 0;
    if(err_msg) *err_msg = NULL;

    add_fd(fd, func, arg, &this->readfds);

    return true;
}

bool LoopSelect::add_writefd(int fd, func_t func, void *arg, int *err_val, const char **err_msg) {
    if(err_val) *err_val = 0;
    if(err_msg) *err_msg = NULL;

    add_fd(fd, func, arg, &this->writefds);

    return true;
}

bool LoopSelect::add_exceptfd(int fd, func_t func, void *arg, int *err_val, const char **err_msg) {
    if(err_val) *err_val = 0;
    if(err_msg) *err_msg = NULL;

    add_fd(fd, func, arg, &this->exceptfds);

    return true;
}

bool LoopSelect::set_timeout_func(func_t timeout_func, void  *arg, int *err_val, const char **err_msg) {
    if(err_val) *err_val = 0;
    if(err_msg) *err_msg = NULL;

    this->timeout_func = timeout_func;
    this->timeout_arg = arg;

    return true;
}

bool LoopSelect::loop(bool *exit_cond, __time_t timeout_sec, __suseconds_t timeout_usec, int *err_val, const char **err_msg) {
    if(err_val) *err_val = 0;
    if(err_msg) *err_msg = NULL;

    bool ret = true;
    struct timeval timeout;
    fd_set readfds, writefds, exceptfds;
    int select_ret;

    do {
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&exceptfds);

        for(auto &it: this->readfds) {
            int fd = it.first;
            FD_SET (fd, &readfds);
        }

        for(auto &it: this->writefds) {
            int fd = it.first;
            FD_SET (fd, &writefds);
        }

        for(auto &it: this->exceptfds) {
            int fd = it.first;
            FD_SET (fd, &exceptfds);
        }

        timeout.tv_sec = timeout_sec;
        timeout.tv_usec = timeout_usec;

        select_ret = select(FD_SETSIZE, &readfds, &writefds, &exceptfds, &timeout);

        if (select_ret > 0) {
            // Event
            for(auto &it: this->readfds) {
                if(FD_ISSET(it.first, &readfds)) {
                    it.second->func(it.second->arg);
                }
            }
            for(auto &it: this->writefds) {
                if(FD_ISSET(it.first, &writefds)) {
                    it.second->func(it.second->arg);
                }
            }
            for(auto &it: this->exceptfds) {
                if(FD_ISSET(it.first, &exceptfds)) {
                    it.second->func(it.second->arg);
                }
            }
        }
        else if (0 == select_ret) {
            // Timeout
            if (NULL != this->timeout_func) {
                this->timeout_func(this->timeout_arg);
            }
        }
        else { // -1 == select_ret
            // error or interrupt
            if (errno == EINTR) continue;
            ret = false;
            if(err_val) *err_val = errno;
            if(err_msg) *err_msg = strerror(errno);
            break;
        }
    } while (false == (*exit_cond));

    return ret;
}
