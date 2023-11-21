#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include "utils.h"
#include "uart_controller.h"
#include "signal_controller.h"
#include "loop_select.h"

#define UART_FIFO_TX_FILENAME "uart_fifo_tx"
#define UART_FIFO_RX_FILENAME "uart_fifo_rx"

typedef struct {
    bool lam_send;
    int uart_dev_fd;
    int uart_fifo_rx_fd;
    int uart_fifo_tx_fd;
} args_t;

void read_uart_dev_func(void *arg) {
    args_t *args = (args_t *) arg;
    char data;

    uart_read(args->uart_dev_fd, &data, 1);
    write(args->uart_fifo_rx_fd, &data, 1);
}

void read_uart_fifo_tx_func(void *arg) {
    args_t *args = (args_t *) arg;
    char data;

    read(args->uart_fifo_tx_fd, &data, 1);
    write(args->uart_dev_fd, &data, 1);
}

void timeout_func(void *arg) {
    args_t *args = (args_t *) arg;
    if(true == args->lam_send) {
        uart_write(args->uart_dev_fd, "lam", 4);
        args->lam_send = false;
    }
}

int main() {
    int err_val = 0;
    const char *err_msg = NULL;
    bool exit_loop = false;
    LoopSelect loop_select;
    args_t args;

    args.uart_dev_fd = uart_open("/dev/ttyUSB0", B115200, &err_val, &err_msg);
    if(0 != err_val) goto __exit;

    unlink(UART_FIFO_TX_FILENAME); // remove if already exists
    if(-1 == mkfifo(UART_FIFO_TX_FILENAME, 0666)) 
    {
        err_msg = strerror(errno);
        err_val = -1;
        goto __close_uart;
    }

    args.uart_fifo_tx_fd = open(UART_FIFO_TX_FILENAME, O_RDWR);
    if(-1 == args.uart_fifo_tx_fd) 
    {
        err_msg = strerror(errno);
        err_val = -1;
        goto __close_fifo;
    }

    unlink(UART_FIFO_RX_FILENAME); // remove if already exists
    if(-1 == mkfifo(UART_FIFO_RX_FILENAME, 0666)) 
    {
        err_msg = strerror(errno);
        err_val = -1;
        goto __close_uart;
    }

    args.uart_fifo_rx_fd = open(UART_FIFO_RX_FILENAME, O_RDWR);
    if(-1 == args.uart_fifo_rx_fd) 
    {
        err_msg = strerror(errno);
        err_val = -1;
        goto __close_fifo;
    }

    loop_select.add_readfd(args.uart_dev_fd, read_uart_dev_func, (void *) &args);
    loop_select.add_readfd(args.uart_fifo_tx_fd, read_uart_fifo_tx_func, (void *) &args);
    
    loop_select.set_timeout_func(timeout_func, (void *) &args);

    register_signal(SIGINT, &exit_loop, &err_val, &err_msg);
    if(0 != err_val) goto __close_fifo;

    register_signal(SIGTSTP, &args.lam_send, &err_val, &err_msg);
    if(0 != err_val) goto __close_fifo;

    if(!loop_select.loop(&exit_loop, 0, 100000, &err_val, &err_msg)) {
        err_msg = strerror(errno);
        err_val = -1;
        goto __close_fifo;
    }

__close_fifo:
    close(args.uart_fifo_tx_fd);
    unlink(UART_FIFO_TX_FILENAME);

    close(args.uart_fifo_rx_fd);
    unlink(UART_FIFO_RX_FILENAME);

__close_uart:
    uart_close(args.uart_dev_fd);

__exit:
    if(NULL != err_msg) printf("%s\n", err_msg);

    return err_val;
}