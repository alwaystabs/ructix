void reverse(char *buf, int len) {
    int i = 0;
    int j = len - 1;
    char tmp;
    while (i < j) {
        tmp = buf[i];
        buf[i] = buf[j];
        buf[j] = tmp;
        i++;
        j--;
    }
}

void itoa(unsigned long long val, char *buf) {
    int i = 0;
    if (val == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    while (val > 0) {
        int digit = val % 10;
        buf[i] = '0' + digit;
        val = val / 10;
        i++;
    }
    buf[i] = '\0';
    reverse(buf, i);
}
int strlen(const char *str) {
    int len = 0;
    while (*str != '\0') {
        len++;
        str++;
    }
    return len;
}