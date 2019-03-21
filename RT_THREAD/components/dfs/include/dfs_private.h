/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#ifndef DFS_PRIVATE_H__
#define DFS_PRIVATE_H__

#include <dfs.h>

// #define DBG_ENABLE
#define DBG_SECTION_NAME	"DFS"
#define DBG_COLOR
#ifndef DBG_LEVEL
#define DBG_LEVEL			DBG_LOG
#endif
#include <rtdbg.h>

#define NO_WORKING_DIR  "system does not support working directory\n"

/* extern variable */
extern const struct dfs_filesystem_ops *filesystem_operation_table[];
extern struct dfs_filesystem filesystem_table[];
extern const struct dfs_mount_tbl mount_table[];

extern char working_directory[];

#endif
