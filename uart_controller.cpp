#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

static bool exit_loop = 0;

int uart_open(const char *uart_name, speed_t baud, int *err_val = NULL, const char **err_msg = NULL)
{
    struct termios newtermios;
    int fd;
    fd = open(uart_name, O_RDWR | O_NOCTTY);
    if(-1 == fd) {
        if(NULL != err_msg) (*err_msg) = strerror(errno);
        if(NULL != err_val) (*err_val) = -1;

        return -1;
    }

    newtermios.c_cflag= CBAUD | CS8 | CLOCAL | CREAD;
    newtermios.c_iflag=IGNPAR;
    newtermios.c_oflag=0;
    newtermios.c_lflag=0;
    newtermios.c_cc[VMIN]=1;
    newtermios.c_cc[VTIME]=0;
    cfsetospeed(&newtermios,baud);
    cfsetispeed(&newtermios,baud);
    if (tcflush(fd,TCIFLUSH)==-1) return -1;
    if (tcflush(fd,TCOFLUSH)==-1) return -1;
    if (tcsetattr(fd,TCSANOW,&newtermios)==-1) return -1;

    if(NULL != err_msg) (*err_msg) = NULL;
    if(NULL != err_val) (*err_val) = 0;
    
    return fd;
}

void uart_write(int uart_fd, const char *data, int size, int *err_val = NULL, const char **err_msg = NULL)
{
    write(uart_fd, data, size);

    if(NULL != err_msg) (*err_msg) = NULL;
    if(NULL != err_val) (*err_val) = 0;
}

int uart_read(int uart_fd, char *data, int size, int *err_val = NULL, const char **err_msg = NULL)
{
    int n;

    n = read(uart_fd, data, size);

    if(NULL != err_msg) (*err_msg) = NULL;
    if(NULL != err_val) (*err_val) = 0;

    return n;
}

void uart_close(int fd, int *err_val = NULL, const char **err_msg = NULL)
{
    close(fd);

    if(NULL != err_msg) (*err_msg) = NULL;
    if(NULL != err_val) (*err_val) = 0;
}