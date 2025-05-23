/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#ifndef __PRINTK_H
#define __PRINTK_H

#include <bao.h>

size_t sprintk(char* dst,const char *fmt, ...);
size_t printk(const char *fmt, ...);

#define BAO_LOG(format, ...)\
  printk("\33[1;35m[%s:%d,%s,cpu %d] " format "\33[0m\n", \
      __FILE__, __LINE__, __func__, cpu()->id, ## __VA_ARGS__)

#endif /* __PRINTK_H */
