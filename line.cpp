#include "line.h"

bool Line::add_char(char c) {
    if(has_more_space) {
        string[idx] = c;
        ++idx;
        string[idx] = '\0';
        if(LINE_SIZE - 1 == idx) has_more_space = false;
    }

    return has_more_space;
}

bool Line::is_empty() {
    return idx == 0;
}

bool Line::is_full() {
    return !has_more_space;
}

const char *Line::pop() {
    const char *str = string;
    idx = 0;
    has_more_space = true;

    return str;
}