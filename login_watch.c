#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LOGIN_PAT "session opened for user"
#define CMD       "journalctl -f -n 0 --output=short 2>/dev/null"

static void now_str(char *out, size_t n) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(out, n, "%Y-%m-%d %H:%M:%S", tm);
}

int main(void) {
    FILE *fp = popen(CMD, "r");
    if (!fp) { perror("popen journalctl"); return 1; }

    fprintf(stderr, "watching journal for logins — press Ctrl-C to stop\n");

    char line[4096];
    while (fgets(line, sizeof(line), fp)) {
        if (!strstr(line, LOGIN_PAT))
            continue;

        /* strip trailing newline */
        int len = strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r'))
            line[--len] = '\0';

        char ts[32];
        now_str(ts, sizeof(ts));
        printf("LOGIN DETECTED *** [%s] %s\n", ts, line);
        fflush(stdout);
    }

    pclose(fp);
    return 0;
}
