/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-04-30     Bernard      first implementation
 * 2006-05-04     Bernard      add list_thread,
 *                                 list_sem,
 *                                 list_timer
 * 2006-05-20     Bernard      add list_mutex,
 *                                 list_mailbox,
 *                                 list_msgqueue,
 *                                 list_event,
 *                                 list_fevent,
 *                                 list_mempool
 * 2006-06-03     Bernard      display stack information in list_thread
 * 2006-08-10     Bernard      change version to invoke rt_show_version
 * 2008-09-10     Bernard      update the list function for finsh syscall
 *                                 list and sysvar list
 * 2009-05-30     Bernard      add list_device
 * 2010-04-21     yi.qiu       add list_module
 * 2012-04-29     goprife      improve the command line auto-complete feature.
 * 2012-06-02     lgnq         add list_memheap
 * 2012-10-22     Bernard      add MS VC++ patch.
 * 2016-06-02     armink       beautify the list_thread command
 * 2018-11-22     Jesven       list_thread add smp support
 */

#include <rthw.h>
#include <rtthread.h>

#ifdef RT_USING_FINSH

#include "finsh.h"

long hello(void)
{
    rt_kprintf("Hello RT-Thread!\n");

    return 0;
}
FINSH_FUNCTION_EXPORT(hello, say hello world);

extern void rt_show_version(void);
long version(void)
{
    rt_show_version();

    return 0;
}
FINSH_FUNCTION_EXPORT(version, show RT-Thread version information);
MSH_CMD_EXPORT(version, show RT-Thread version information);

static int object_name_maxlen(const char *type_name, struct rt_list_node *list)
{
    struct rt_list_node *node;
    struct rt_object *object = NULL;
    int max_length = rt_strlen(type_name), length;

    rt_enter_critical();
    for (node = list->next; node != list; node = node->next)
    {
        object = rt_list_entry(node, struct rt_object, list);

        length = rt_strlen(object->name);
        if (length > max_length) max_length = length;
    }
    rt_exit_critical();

    if (max_length > RT_NAME_MAX || max_length == 0) max_length = RT_NAME_MAX;

    return max_length;
}

rt_inline void object_split(int len)
{
    while (len--) rt_kprintf("-");
}

static long _list_thread(struct rt_list_node *list)
{
    int maxlen;
    rt_uint8_t *ptr;
    struct rt_thread *thread;
    struct rt_list_node *node;
    const char *item_title = "thread";

    maxlen = object_name_maxlen(item_title, list);

#ifdef RT_USING_SMP
    rt_kprintf("%-*.s cpu pri  status      sp     stack size max used left tick  error\n", maxlen, item_title); object_split(maxlen);
    rt_kprintf(     " --- ---  ------- ---------- ----------  ------  ---------- ---\n");
#else
    rt_kprintf("%-*.s pri  status      sp     stack size max used left tick  error\n", maxlen, item_title); object_split(maxlen);
    rt_kprintf(     " ---  ------- ---------- ----------  ------  ---------- ---\n");
#endif /*RT_USING_SMP*/
    for (node = list->next; node != list; node = node->next)
    {
        rt_uint8_t stat;
        thread = rt_list_entry(node, struct rt_thread, list);
#ifdef RT_USING_SMP
        if (thread->oncpu != RT_CPU_DETACHED)
            rt_kprintf("%-*.*s %3d %3d ", maxlen, RT_NAME_MAX, thread->name, thread->oncpu, thread->current_priority);
        else
            rt_kprintf("%-*.*s N/A %3d ", maxlen, RT_NAME_MAX, thread->name, thread->current_priority);

#else
        rt_kprintf("%-*.*s %3d ", maxlen, RT_NAME_MAX, thread->name, thread->current_priority);
#endif /*RT_USING_SMP*/
        stat = (thread->stat & RT_THREAD_STAT_MASK);
        if (stat == RT_THREAD_READY)        rt_kprintf(" ready  ");
        else if (stat == RT_THREAD_SUSPEND) rt_kprintf(" suspend");
        else if (stat == RT_THREAD_INIT)    rt_kprintf(" init   ");
        else if (stat == RT_THREAD_CLOSE)   rt_kprintf(" close  ");

#if defined(ARCH_CPU_STACK_GROWS_UPWARD)
        ptr = (rt_uint8_t *)thread->stack_addr + thread->stack_size;
        while (*ptr == '#')ptr --;

        rt_kprintf(" 0x%08x 0x%08x    %02d%%   0x%08x %03d\n",
                   ((rt_ubase_t)thread->sp - (rt_ubase_t)thread->stack_addr),
                   thread->stack_size,
                   ((rt_ubase_t)ptr - (rt_ubase_t)thread->stack_addr) * 100 / thread->stack_size,
                   thread->remaining_tick,
                   thread->error);
#else
        ptr = (rt_uint8_t *)thread->stack_addr;
        while (*ptr == '#')ptr ++;

        rt_kprintf(" 0x%08x 0x%08x    %02d%%   0x%08x %03d\n",
                   thread->stack_size + ((rt_ubase_t)thread->stack_addr - (rt_ubase_t)thread->sp),
                   thread->stack_size,
                   (thread->stack_size - ((rt_ubase_t) ptr - (rt_ubase_t) thread->stack_addr)) * 100
                        / thread->stack_size,
                   thread->remaining_tick,
                   thread->error);
#endif
    }

    return 0;
}

long list_thread(void)
{
    rt_ubase_t level;
    struct rt_object_information *info;
    long ret;

    level = rt_hw_interrupt_disable();
    info = rt_object_get_information(RT_Object_Class_Thread);
    ret = _list_thread(&info->object_list);
    rt_hw_interrupt_enable(level);
    return ret;
}
FINSH_FUNCTION_EXPORT(list_thread, list thread);
MSH_CMD_EXPORT(list_thread, list thread);

static void show_wait_queue(struct rt_list_node *list)
{
    struct rt_thread *thread;
    struct rt_list_node *node;

    for (node = list->next; node != list; node = node->next)
    {
        thread = rt_list_entry(node, struct rt_thread, tlist);
        rt_kprintf("%s", thread->name);

        if (node->next != list)
            rt_kprintf("/");
    }
}

#ifdef RT_USING_SEMAPHORE
static long _list_sem(struct rt_list_node *list)
{
    int maxlen;
    struct rt_semaphore *sem;
    struct rt_list_node *node;
    const char *item_title = "semaphore";

    maxlen = object_name_maxlen(item_title, list);

    rt_kprintf("%-*.s v   suspend thread\n", maxlen, item_title); object_split(maxlen);
    rt_kprintf(     " --- --------------\n");
    for (node = list->next; node != list; node = node->next)
    {
        sem = (struct rt_semaphore *)(rt_list_entry(node, struct rt_object, list));
        if (!rt_list_isempty(&sem->parent.suspend_thread))
        {
            rt_kprintf("%-*.*s %03d %d:",
                       maxlen, RT_NAME_MAX,
                       sem->parent.parent.name,
                       sem->value,
                       rt_list_len(&sem->parent.suspend_thread));
            show_wait_queue(&(sem->parent.suspend_thread));
            rt_kprintf("\n");
        }
        else
        {
            rt_kprintf("%-*.*s %03d %d\n",
                       maxlen, RT_NAME_MAX,
                       sem->parent.parent.name,
                       sem->value,
                       rt_list_len(&sem->parent.suspend_thread));
        }
    }

    return 0;
}

long list_sem(void)
{
    struct rt_object_information *info;

    info = rt_object_get_information(RT_Object_Class_Semaphore);

    return _list_sem(&info->object_list);
}
FINSH_FUNCTION_EXPORT(list_sem, list semaphone in system);
MSH_CMD_EXPORT(list_sem, list semaphore in system);
#endif

#ifdef RT_USING_EVENT
static long _list_event(struct rt_list_node *list)
{
    int maxlen;
    struct rt_event *e;
    struct rt_list_node *node;
    const char *item_title = "event";

    maxlen = object_name_maxlen(item_title, list);

    rt_kprintf("%-*.s      set    suspend thread\n", maxlen, item_title); object_split(maxlen);
    rt_kprintf(     "  ---------- --------------\n");
    for (node = list->next; node != list; node = node->next)
    {
        e = (struct rt_event *)(rt_list_entry(node, struct rt_object, list));
        if (!rt_list_isempty(&e->parent.suspend_thread))
        {
            rt_kprintf("%-*.*s  0x%08x %03d:",
                       maxlen, RT_NAME_MAX,
                       e->parent.parent.name,
                       e->set,
                       rt_list_len(&e->parent.suspend_thread));
            show_wait_queue(&(e->parent.suspend_thread));
            rt_kprintf("\n");
        }
        else
        {
            rt_kprintf("%-*.*s  0x%08x 0\n",
                       maxlen, RT_NAME_MAX, e->parent.parent.name, e->set);
        }
    }

    return 0;
}

long list_event(void)
{
    struct rt_object_information *info;

    info = rt_object_get_information(RT_Object_Class_Event);
    return _list_event(&info->object_list);
}
FINSH_FUNCTION_EXPORT(list_event, list event in system);
MSH_CMD_EXPORT(list_event, list event in system);
#endif

#ifdef RT_USING_MUTEX
static long _list_mutex(struct rt_list_node *list)
{
    int maxlen;
    struct rt_mutex *m;
    struct rt_list_node *node;
    const char *item_title = "mutex";

    maxlen = object_name_maxlen(item_title, list);

    rt_kprintf("%-*.s   owner  hold suspend thread\n", maxlen, item_title); object_split(maxlen);
    rt_kprintf(     " -------- ---- --------------\n");
    for (node = list->next; node != list; node = node->next)
    {
        m = (struct rt_mutex *)(rt_list_entry(node, struct rt_object, list));
        rt_kprintf("%-*.*s %-8.*s %04d %d\n",
                   maxlen, RT_NAME_MAX,
                   m->parent.parent.name,
                   RT_NAME_MAX,
                   m->owner->name,
                   m->hold,
                   rt_list_len(&m->parent.suspend_thread));
    }

    return 0;
}

long list_mutex(void)
{
    struct rt_object_information *info;

    info = rt_object_get_information(RT_Object_Class_Mutex);

    return _list_mutex(&info->object_list);
}
FINSH_FUNCTION_EXPORT(list_mutex, list mutex in system);
MSH_CMD_EXPORT(list_mutex, list mutex in system);
#endif

#ifdef RT_USING_MAILBOX
static long _list_mailbox(struct rt_list_node *list)
{
    int maxlen;
    struct rt_mailbox *m;
    struct rt_list_node *node;
    const char *item_title = "mailbox";

    maxlen = object_name_maxlen(item_title, list);

    rt_kprintf("%-*.s entry size suspend thread\n", maxlen, item_title); object_split(maxlen);
    rt_kprintf(     " ----  ---- --------------\n");
    for (node = list->next; node != list; node = node->next)
    {
        m = (struct rt_mailbox *)(rt_list_entry(node, struct rt_object, list));
        if (!rt_list_isempty(&m->parent.suspend_thread))
        {
            rt_kprintf("%-*.*s %04d  %04d %d:",
                       maxlen, RT_NAME_MAX,
                       m->parent.parent.name,
                       m->entry,
                       m->size,
                       rt_list_len(&m->parent.suspend_thread));
            show_wait_queue(&(m->parent.suspend_thread));
            rt_kprintf("\n");
        }
        else
        {
            rt_kprintf("%-*.*s %04d  %04d %d\n",
                       maxlen, RT_NAME_MAX,
                       m->parent.parent.name,
                       m->entry,
                       m->size,
                       rt_list_len(&m->parent.suspend_thread));
        }
    }

    return 0;
}

long list_mailbox(void)
{
    struct rt_object_information *info;

    info = rt_object_get_information(RT_Object_Class_MailBox);
    return _list_mailbox(&info->object_list);
}
FINSH_FUNCTION_EXPORT(list_mailbox, list mail box in system);
MSH_CMD_EXPORT(list_mailbox, list mail box in system);
#endif

#ifdef RT_USING_MESSAGEQUEUE
static long _list_msgqueue(struct rt_list_node *list)
{
    int maxlen;
    struct rt_messagequeue *m;
    struct rt_list_node *node;
    const char *item_title = "msgqueue";

    maxlen = object_name_maxlen(item_title, list);

    rt_kprintf("%-*.s entry suspend thread\n", maxlen, item_title); object_split(maxlen);
    rt_kprintf(     " ----  --------------\n");
    for (node = list->next; node != list; node = node->next)
    {
        m = (struct rt_messagequeue *)(rt_list_entry(node, struct rt_object, list));
        if (!rt_list_isempty(&m->parent.suspend_thread))
        {
            rt_kprintf("%-*.*s %04d  %d:",
                       maxlen, RT_NAME_MAX,
                       m->parent.parent.name,
                       m->entry,
                       rt_list_len(&m->parent.suspend_thread));
            show_wait_queue(&(m->parent.suspend_thread));
            rt_kprintf("\n");
        }
        else
        {
            rt_kprintf("%-*.*s %04d  %d\n",
                       maxlen, RT_NAME_MAX,
                       m->parent.parent.name,
                       m->entry,
                       rt_list_len(&m->parent.suspend_thread));
        }
    }

    return 0;
}

long list_msgqueue(void)
{
    struct rt_object_information *info;

    info = rt_object_get_information(RT_Object_Class_MessageQueue);
    return _list_msgqueue(&info->object_list);
}
FINSH_FUNCTION_EXPORT(list_msgqueue, list message queue in system);
MSH_CMD_EXPORT(list_msgqueue, list message queue in system);
#endif

#ifdef RT_USING_MEMHEAP
static long _list_memheap(struct rt_list_node *list)
{
    int maxlen;
    struct rt_memheap *mh;
    struct rt_list_node *node;
    const char *item_title = "memheap";

    maxlen = object_name_maxlen(item_title, list);

    rt_kprintf("%-*.s  pool size  max used size available size\n", maxlen, item_title); object_split(maxlen);
    rt_kprintf(      " ---------- ------------- --------------\n");
    for (node = list->next; node != list; node = node->next)
    {
        mh = (struct rt_memheap *)rt_list_entry(node, struct rt_object, list);

        rt_kprintf("%-*.*s %-010d %-013d %-05d\n",
                   maxlen, RT_NAME_MAX,
                   mh->parent.name,
                   mh->pool_size,
                   mh->max_used_size,
                   mh->available_size);
    }

    return 0;
}

long list_memheap(void)
{
    struct rt_object_information *info;

    info = rt_object_get_information(RT_Object_Class_MemHeap);
    return _list_memheap(&info->object_list);
}
FINSH_FUNCTION_EXPORT(list_memheap, list memory heap in system);
MSH_CMD_EXPORT(list_memheap, list memory heap in system);
#endif

#ifdef RT_USING_MEMPOOL
static long _list_mempool(struct rt_list_node *list)
{
    int maxlen;
    struct rt_mempool *mp;
    struct rt_list_node *node;
    const char *item_title = "mempool";

    maxlen = object_name_maxlen(item_title, list);

    rt_kprintf("%-*.s block total free suspend thread\n", maxlen, item_title); object_split(maxlen);
    rt_kprintf(     " ----  ----  ---- --------------\n");
    for (node = list->next; node != list; node = node->next)
    {
        mp = (struct rt_mempool *)rt_list_entry(node, struct rt_object, list);
        if (mp->suspend_thread_count > 0)
        {
            rt_kprintf("%-*.*s %04d  %04d  %04d %d:",
                       maxlen, RT_NAME_MAX,
                       mp->parent.name,
                       mp->block_size,
                       mp->block_total_count,
                       mp->block_free_count,
                       mp->suspend_thread_count);
            show_wait_queue(&(mp->suspend_thread));
            rt_kprintf("\n");
        }
        else
        {
            rt_kprintf("%-*.*s %04d  %04d  %04d %d\n",
                       maxlen, RT_NAME_MAX,
                       mp->parent.name,
                       mp->block_size,
                       mp->block_total_count,
                       mp->block_free_count,
                       mp->suspend_thread_count);
        }
    }

    return 0;
}

long list_mempool(void)
{
    struct rt_object_information *info;

    info = rt_object_get_information(RT_Object_Class_MemPool);
    return _list_mempool(&info->object_list);
}
FINSH_FUNCTION_EXPORT(list_mempool, list memory pool in system)
MSH_CMD_EXPORT(list_mempool, list memory pool in system);
#endif

static long _list_timer(struct rt_list_node *list)
{
    int maxlen;
    struct rt_timer *timer;
    struct rt_list_node *node;
    const char *item_title = "timer";

    maxlen = object_name_maxlen(item_title, list);

    rt_kprintf("%-*.s  periodic   timeout       flag\n", maxlen, item_title); object_split(maxlen);
    rt_kprintf(     " ---------- ---------- -----------\n");
    for (node = list->next; node != list; node = node->next)
    {
        timer = (struct rt_timer *)(rt_list_entry(node, struct rt_object, list));
        rt_kprintf("%-*.*s 0x%08x 0x%08x ",
                   maxlen, RT_NAME_MAX,
                   timer->parent.name,
                   timer->init_tick,
                   timer->timeout_tick);
        if (timer->parent.flag & RT_TIMER_FLAG_ACTIVATED)
            rt_kprintf("activated\n");
        else
            rt_kprintf("deactivated\n");
    }

    rt_kprintf("current tick:0x%08x\n", rt_tick_get());

    return 0;
}

long list_timer(void)
{
    struct rt_object_information *info;

    info = rt_object_get_information(RT_Object_Class_Timer);
    return _list_timer(&info->object_list);
}
FINSH_FUNCTION_EXPORT(list_timer, list timer in system);
MSH_CMD_EXPORT(list_timer, list timer in system);

#ifdef RT_USING_DEVICE
static long _list_device(struct rt_list_node *list)
{
    int maxlen;
    struct rt_device *device;
    struct rt_list_node *node;
    char *const device_type_str[] =
    {
        "Character Device",
        "Block Device",
        "Network Interface",
        "MTD Device",
        "CAN Device",
        "RTC",
        "Sound Device",
        "Graphic Device",
        "I2C Bus",
        "USB Slave Device",
        "USB Host Bus",
        "SPI Bus",
        "SPI Device",
        "SDIO Bus",
        "PM Pseudo Device",
        "Pipe",
        "Portal Device",
        "Timer Device",
        "Miscellaneous Device",
        "Unknown"
    };
    const char *item_title = "device";

    maxlen = object_name_maxlen(item_title, list);

    rt_kprintf("%-*.s         type         ref count\n", maxlen, item_title); object_split(maxlen);
    rt_kprintf(     " -------------------- ----------\n");
    for (node = list->next; node != list; node = node->next)
    {
        device = (struct rt_device *)(rt_list_entry(node, struct rt_object, list));
        rt_kprintf("%-*.*s %-20s %-8d\n",
                   maxlen, RT_NAME_MAX,
                   device->parent.name,
                   (device->type <= RT_Device_Class_Unknown) ?
                   device_type_str[device->type] :
                   device_type_str[RT_Device_Class_Unknown],
                   device->ref_count);
    }

    return 0;
}

long list_device(void)
{
    struct rt_object_information *info;

    info = rt_object_get_information(RT_Object_Class_Device);
    return _list_device(&info->object_list);
}
FINSH_FUNCTION_EXPORT(list_device, list device in system);
MSH_CMD_EXPORT(list_device, list device in system);
#endif

long list(void)
{
#ifndef FINSH_USING_MSH_ONLY
    struct finsh_syscall_item *syscall_item;
    struct finsh_sysvar_item *sysvar_item;
#endif

    rt_kprintf("--Function List:\n");
    {
        struct finsh_syscall *index;
        for (index = _syscall_table_begin;
                index < _syscall_table_end;
                FINSH_NEXT_SYSCALL(index))
        {
            /* skip the internal command */
            if (strncmp((char *)index->name, "__", 2) == 0) continue;

#ifdef FINSH_USING_DESCRIPTION
            rt_kprintf("%-16s -- %s\n", index->name, index->desc);
#else
            rt_kprintf("%s\n", index->name);
#endif
        }
    }

#ifndef FINSH_USING_MSH_ONLY
    /* list syscall list */
    syscall_item = global_syscall_list;
    while (syscall_item != NULL)
    {
        rt_kprintf("[l] %s\n", syscall_item->syscall.name);
        syscall_item = syscall_item->next;
    }

    rt_kprintf("--Variable List:\n");
    {
        struct finsh_sysvar *index;
        for (index = _sysvar_table_begin;
                index < _sysvar_table_end;
                FINSH_NEXT_SYSVAR(index))
        {
#ifdef FINSH_USING_DESCRIPTION
            rt_kprintf("%-16s -- %s\n", index->name, index->desc);
#else
            rt_kprintf("%s\n", index->name);
#endif
        }
    }

    sysvar_item = global_sysvar_list;
    while (sysvar_item != NULL)
    {
        rt_kprintf("[l] %s\n", sysvar_item->sysvar.name);
        sysvar_item = sysvar_item->next;
    }
#endif

    return 0;
}
FINSH_FUNCTION_EXPORT(list, list all symbol in system)

#ifndef FINSH_USING_MSH_ONLY
static int str_is_prefix(const char *prefix, const char *str)
{
    while ((*prefix) && (*prefix == *str))
    {
        prefix ++;
        str ++;
    }

    if (*prefix == 0)
        return 0;

    return -1;
}

static int str_common(const char *str1, const char *str2)
{
    const char *str = str1;

    while ((*str != 0) && (*str2 != 0) && (*str == *str2))
    {
        str ++;
        str2 ++;
    }

    return (str - str1);
}

void list_prefix(char *prefix)
{
    struct finsh_syscall_item *syscall_item;
    struct finsh_sysvar_item *sysvar_item;
    rt_uint16_t func_cnt, var_cnt;
    int length, min_length;
    const char *name_ptr;

    func_cnt = 0;
    var_cnt  = 0;
    min_length = 0;
    name_ptr = RT_NULL;

    /* checks in system function call */
    {
        struct finsh_syscall *index;
        for (index = _syscall_table_begin;
                index < _syscall_table_end;
                FINSH_NEXT_SYSCALL(index))
        {
            /* skip internal command */
            if (str_is_prefix("__", index->name) == 0) continue;

            if (str_is_prefix(prefix, index->name) == 0)
            {
                if (func_cnt == 0)
                {
                    rt_kprintf("--function:\n");

                    if (*prefix != 0)
                    {
                        /* set name_ptr */
                        name_ptr = index->name;

                        /* set initial length */
                        min_length = strlen(name_ptr);
                    }
                }

                func_cnt ++;

                if (*prefix != 0)
                {
                    length = str_common(name_ptr, index->name);
                    if (length < min_length)
                        min_length = length;
                }

#ifdef FINSH_USING_DESCRIPTION
                rt_kprintf("%-16s -- %s\n", index->name, index->desc);
#else
                rt_kprintf("%s\n", index->name);
#endif
            }
        }
    }

    /* checks in dynamic system function call */
    syscall_item = global_syscall_list;
    while (syscall_item != NULL)
    {
        if (str_is_prefix(prefix, syscall_item->syscall.name) == 0)
        {
            if (func_cnt == 0)
            {
                rt_kprintf("--function:\n");
                if (*prefix != 0 && name_ptr == NULL)
                {
                    /* set name_ptr */
                    name_ptr = syscall_item->syscall.name;

                    /* set initial length */
                    min_length = strlen(name_ptr);
                }
            }

            func_cnt ++;

            if (*prefix != 0)
            {
                length = str_common(name_ptr, syscall_item->syscall.name);
                if (length < min_length)
                    min_length = length;
            }

            rt_kprintf("[l] %s\n", syscall_item->syscall.name);
        }
        syscall_item = syscall_item->next;
    }

    /* checks in system variable */
    {
        struct finsh_sysvar *index;
        for (index = _sysvar_table_begin;
                index < _sysvar_table_end;
                FINSH_NEXT_SYSVAR(index))
        {
            if (str_is_prefix(prefix, index->name) == 0)
            {
                if (var_cnt == 0)
                {
                    rt_kprintf("--variable:\n");

                    if (*prefix != 0 && name_ptr == NULL)
                    {
                        /* set name_ptr */
                        name_ptr = index->name;

                        /* set initial length */
                        min_length = strlen(name_ptr);

                    }
                }

                var_cnt ++;

                if (*prefix != 0)
                {
                    length = str_common(name_ptr, index->name);
                    if (length < min_length)
                        min_length = length;
                }

#ifdef FINSH_USING_DESCRIPTION
                rt_kprintf("%-16s -- %s\n", index->name, index->desc);
#else
                rt_kprintf("%s\n", index->name);
#endif
            }
        }
    }

    /* checks in dynamic system variable */
    sysvar_item = global_sysvar_list;
    while (sysvar_item != NULL)
    {
        if (str_is_prefix(prefix, sysvar_item->sysvar.name) == 0)
        {
            if (var_cnt == 0)
            {
                rt_kprintf("--variable:\n");
                if (*prefix != 0 && name_ptr == NULL)
                {
                    /* set name_ptr */
                    name_ptr = sysvar_item->sysvar.name;

                    /* set initial length */
                    min_length = strlen(name_ptr);
                }
            }

            var_cnt ++;

            if (*prefix != 0)
            {
                length = str_common(name_ptr, sysvar_item->sysvar.name);
                if (length < min_length)
                    min_length = length;
            }

            rt_kprintf("[v] %s\n", sysvar_item->sysvar.name);
        }
        sysvar_item = sysvar_item->next;
    }

    /* only one matched */
    if (name_ptr != NULL)
    {
        rt_strncpy(prefix, name_ptr, min_length);
    }
}
#endif

#if defined(FINSH_USING_SYMTAB) && !defined(FINSH_USING_MSH_ONLY)
static int dummy = 0;
FINSH_VAR_EXPORT(dummy, finsh_type_int, dummy variable for finsh)
#endif

#endif /* RT_USING_FINSH */

