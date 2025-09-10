#include"../include/libc/x41_errno.h"
#if defined(__GNUC__)
    #ifdef TLS_ENABLED
        extern __thread int x41_errno;
    #else 
        int x41_errno;
    #endif
#else
    #error "Thread‑local storage errno not implemented"
#endif


char *x41_strerror(int errnum) {
    switch (errnum) {
        case 0: return "No error";
        case EPERM: return "Operation not permitted";
        case ENOENT: return "No such file or directory";
        case ESRCH: return "No such process";
        case EINTR: return "Interrupted system call";
        case EIO: return "Input/output error";
        case ENXIO: return "No such device or address";
        case E2BIG: return "Argument list too long";
        case ENOEXEC: return "Exec format error";
        case EBADF: return "Bad file descriptor";
        case ECHILD: return "No child processes";
        case EAGAIN: return "Resource temporarily unavailable";
        case ENOMEM: return "Cannot allocate memory";
        case EACCES: return "Permission denied";
        case EFAULT: return "Bad address";
        case ENOTBLK: return "Block device required";
        case EBUSY: return "Device or resource busy";
        case EEXIST: return "File exists";
        case EXDEV: return "Invalid cross-device link";
        case ENODEV: return "No such device";
        case ENOTDIR: return "Not a directory";
        case EISDIR: return "Is a directory";
        case EINVAL: return "Invalid argument";
        case ENFILE: return "Too many open files in system";
        case EMFILE: return "Too many open files";
        case ENOTTY: return "Inappropriate ioctl for device";
        case ETXTBSY: return "Text file busy";
        case EFBIG: return "File too large";
        case ENOSPC: return "No space left on device";
        case ESPIPE: return "Illegal seek";
        case EROFS: return "Read-only file system";
        case EMLINK: return "Too many links";
        case EPIPE: return "Broken pipe";
        case EDOM: return "Numerical argument out of domain";
        case ERANGE: return "Result too large";
        case EDEADLK: return "Resource deadlock would occur";
        case ENAMETOOLONG: return "File name too long";
        case ENOLCK: return "No record locks available";
        case ENOSYS: return "Function not implemented";
        case ENOTEMPTY: return "Directory not empty";
        case ELOOP: return "Too many symbolic links encountered";
        case EWOULDBLOCK: return "Operation would block";
        case ENOMSG: return "No message of desired type";
        case EIDRM: return "Identifier removed";
        case ECHRNG: return "Channel number out of range";
        case EL2NSYNC: return "Level 2 not synchronized";
        case EL3HLT: return "Level 3 halted";
        case EL3RST: return "Level 3 reset";
        case ELNRNG: return "Link number out of range";
        case EUNATCH: return "Protocol driver not attached";
        case ENOCSI: return "No CSI structure available";
        case EL2HLT: return "Level 2 halted";
        case EBADE: return "Invalid exchange";
        case EBADR: return "Invalid request descriptor";
        case EXFULL: return "Exchange full";
        case ENOANO: return "No anode";
        case EBADRQC: return "Invalid request code";
        case EBADSLT: return "Invalid slot";
        case EDEADLOCK: return "File locking deadlock error";
        case EBFONT: return "Bad font file format";
        case ENOSTR: return "Device not a stream";
        case ENODATA: return "No data available";
        case ETIME: return "Timer expired";
        case ENOSR: return "Out of streams resources";
        case ENONET: return "Machine is not on the network";
        case ENOPKG: return "Package not installed";
        case EREMOTE: return "Object is remote";
        case ENOLINK: return "Link has been severed";
        case EADV: return "Advertise error";
        case ESRMNT: return "Srmount error";
        case ECOMM: return "Communication error on send";
        case EPROTO: return "Protocol error";
        case EMULTIHOP: return "Multihop attempted";
        case EDOTDOT: return "RFS specific error";
        case EBADMSG: return "Not a data message";
        case EOVERFLOW: return "Value too large for defined data type";
        default:
            return "Unknown error code"; // Generic message for unknown codes
    }
}