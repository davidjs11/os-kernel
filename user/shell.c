/*   user/shell.c   */

/* Minimal user-space shell. It only uses system calls to talk to the kernel. */

#include <syscall.h>

#define LINE_MAX 128

/* String functions (no libc in userspace). */
static usize_t ustrlen(const char *s)
{
    usize_t n = 0;
    while (s[n] != '\0') {
        n++;
    }
    return n;
}

static int ustrcmp(const char *a, const char *b)
{
    while (*a != '\0' && *a == *b) {
        a++;
        b++;
    }
    return (int) ((unsigned char) *a) - (int) ((unsigned char) *b);
}

static int ustrncmp(const char *a, const char *b, usize_t n)
{
    for (usize_t i = 0; i < n; i++) {
        if (a[i] != b[i] || a[i] == '\0') {
            return (int) ((unsigned char) a[i]) - (int) ((unsigned char) b[i]);
        }
    }
    return 0;
}

/* Output functions. */
static void puts_(const char *s)
{
    sys_write(1, s, ustrlen(s));
}

static void putc_(char c)
{
    sys_write(1, &c, 1);
}

static void put_ulong(unsigned long value)
{
    char buf[24];
    usize_t i = sizeof(buf);

    buf[--i] = '\0';
    do {
        buf[--i] = (char) ('0' + (value % 10));
        value /= 10;
    } while (value != 0 && i > 0);

    puts_(&buf[i]);
}

/*
 * Read a line from stdin. Characters are echoed back because the kernel
 * doesn't do it. Returns the length of the line.
 */
static usize_t read_line(char *buf, usize_t max)
{
    usize_t len = 0;

    for (;;) {
        char c;
        if (sys_read(0, &c, 1) != 1) {
            continue;
        }

        /* End of line. */
        if (c == '\r' || c == '\n') {
            puts_("\n");
            break;
        }

        /* Backspace (or DEL). */
        if (c == 0x7f || c == '\b') {
            if (len > 0) {
                len--;
                puts_("\b \b");
            }
            continue;
        }

        if (len + 1 < max) {
            buf[len++] = c;
            putc_(c);
        }
    }

    buf[len] = '\0';
    return len;
}

static void cmd_help(void)
{
    puts_(
        "Available commands:\n"
        "  help         show this message\n"
        "  echo <text>  print text back\n"
        "  pid          print the shell process id\n"
        "  clear        clear the screen\n"
        "  exit         terminate the shell\n"
    );
}

void shell_main(void)
{
    char line[LINE_MAX];

    puts_("\nType 'help' for commands.\n");

    for (;;) {
        puts_("$ ");
        read_line(line, sizeof(line));

        if (line[0] == '\0') {
            continue;
        }

        if (ustrcmp(line, "help") == 0) {
            cmd_help();
        } else if (ustrcmp(line, "echo") == 0) {
            puts_("\n");
        } else if (ustrncmp(line, "echo ", 5) == 0) {
            puts_(line + 5);
            puts_("\n");
        } else if (ustrcmp(line, "pid") == 0) {
            puts_("pid: ");
            put_ulong((unsigned long) sys_getpid());
            puts_("\n");
        } else if (ustrcmp(line, "clear") == 0) {
            puts_("\033[2J\033[H");
        } else if (ustrcmp(line, "exit") == 0) {
            puts_("bye\n");
            sys_exit(0);
        } else {
            puts_(line);
            puts_(": command not found\n");
        }
    }
}

/* Entry point, placed at the beginning of the binary (see user.ld). */
__attribute__((section(".text.start"), used))
void _start(void)
{
    shell_main();
    sys_exit(0);
}
