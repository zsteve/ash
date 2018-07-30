#include "process.h"

void make_pt(int* fdm, int* fds){
    /**
     * create pseudoterminal
     */
    
    int rc;

    *fdm = posix_openpt(O_RDWR);
    assert(*fdm >= 0);
    rc = grantpt(*fdm);
    assert(rc == 0);
    rc = unlockpt(*fdm);
    assert(rc == 0);
    *fds = open(ptsname(*fdm), O_RDWR);
    assert(*fds >= 0);
}