/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __TIME_H__
#define __TIME_H__	1

#ifdef __cplusplus
extern "C" {
#endif

/*Se declara structura necesara si tipul de date*/
typedef long time_t;

struct timespec {
    time_t tv_sec;
    long   tv_nsec;
};

/*Se declara procesele*/
int nanosleep(const struct timespec *rqtp, struct timespec *rmtp);
unsigned int sleep(unsigned int sec);

#ifdef __cplusplus
}
#endif

#endif
