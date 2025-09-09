#ifndef MINI_LIBC_SYS_WAIT_H
#define MINI_LIBC_SYS_WAIT_H

/* Encodage Linux du status (waitpid/wait4) :
   - bits 0..6  : numéro du signal de terminaison
   - bit 7      : flag core dump
   - bits 8..15 : code de retour (exit) si terminé normalement
   - si bits 0..6 == 0x7f => process stoppé, stop-signal dans bits 8..15
*/
#define WTERMSIG(status)   ((status) & 0x7f)
#define WCOREDUMP(status)  ((status) & 0x80)
#define WEXITSTATUS(status) (((status) >> 8) & 0xff)

#define WIFEXITED(status)   (WTERMSIG(status) == 0)
#define WIFSIGNALED(status) (WTERMSIG(status) != 0 && WTERMSIG(status) != 0x7f)
#define WIFSTOPPED(status)  (WTERMSIG(status) == 0x7f)
#define WSTOPSIG(status)    WEXITSTATUS(status)

#endif

