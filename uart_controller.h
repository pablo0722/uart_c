int uart_open(const char *uart_name, speed_t baud, int *err_val = NULL, const char **err_msg = NULL);
void uart_write(int uart_fd, const char *data, int size, int *err_val = NULL, const char **err_msg = NULL);
int uart_read(int uart_fd, char *data, int size, int *err_val = NULL, const char **err_msg = NULL);
void uart_close(int fd, int *err_val = NULL, const char **err_msg = NULL);