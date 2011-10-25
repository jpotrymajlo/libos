#ifndef _ERRNO_H_
#define _ERRNO_H_

extern int* get_errno_ptr(void);

#define errno (*get_errno_ptr())

#define ENOERROR  0
#define EINVAL    1

#endif
