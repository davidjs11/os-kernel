/*   arch/x86_64/usertest_blob.S   */

/* User-space shell flat binary, embedded in the kernel image. */

.section .rodata
.global user_shell_blob
.global user_shell_blob_end

user_shell_blob:
.incbin "build/shell-x86_64.bin"
user_shell_blob_end:
