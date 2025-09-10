#ifndef _X41_FCNTL_H
# define _X41_FCNTL_H 


/* --------------------------------------------------------------
 *  Access mode bits – mutually exclusive; mask with O_ACCMODE.
 * -------------------------------------------------------------- */

# define O_RDONLY        0x0000U          /* open for reading only */
# define O_WRONLY        0x0001U          /* open for writing only */
# define O_RDWR          0x0002U          /* open for reading and writing */
# define O_ACCMODE       0x0003U          /* mask for access mode */

/* --------------------------------------------------------------
 *  Flags (bit field).
 * -------------------------------------------------------------- */


# define O_CREAT         0x0040U          /* create file if it does not exist */
# define O_EXCL          0x0080U          /* error if O_CREAT and the file exists */
# define O_NOCTTY        0x0100U          /* don’t make this terminal the controlling tty */
# define O_TRUNC         0x0200U          /* truncate file to zero length */
# define O_APPEND        0x0400U          /* writes append to end of file */
# define O_NONBLOCK      0x0800U          /* non‑blocking I/O */
# define O_DSYNC         0x1000U          /* write I/O data integrity sync */
# define O_SYNC          0x101000U        /* write I/O file synchronisation */
# define O_RSYNC         0x101000U        /* read I/O synchronisation (same as O_SYNC) */

/* Miscellaneous */
# define O_CLOEXEC       0x80000U       /* set close‑on‑exec (FD_CLOEXEC) */
# define O_DIRECTORY     0x10000U       /* fail if not a directory */
# define O_NOFOLLOW      0x20000U       /* don’t follow symlinks */
# define O_DIRECT        0x40000U       /* minimize cache effects */
# define O_LARGEFILE     0x80000U       /* allow >2 GB files on 32‑bit kernels */
# define O_PATH          0x200000U      /* obtain a fd without opening */
# define O_TMPFILE       0x410000U      /* create unnamed temporary file */
# define O_NDELAY        O_NONBLOCK      /* non‑blocking I/O (4.2BSD) */
# define O_ASYNC         0x4000U        /* signal pgrp when data ready */
# define O_NOATIME       0x40000U       /* don’t update atime */
# define O_CLOFORK       0x80000000U    /* mark the fd as close-on-fork */

#endif /* _X41_FCNTL_H */
