/* RT-Thread config file */

#ifndef __RTTHREAD_CFG_H__
#define __RTTHREAD_CFG_H__

// <<< Use Configuration Wizard in Context Menu >>>
// <h>RT-Thread Kernel Basic Configuration
// <o>Maximal level of thread priority <8-256>
// <i>Default: 32
#define RT_THREAD_PRIORITY_MAX 12
// <o>OS tick per second 
// <64-10000>
// <i>Default: 1000 (1ms)
#define RT_TICK_PER_SECOND 100
// <o>Alignment size for CPU architecture data access
// <i>Default: 4
// <4=> 4bytes  
#define RT_ALIGN_SIZE 4
// <o>the max length of object name<2-16>
// <i>Default: 8
#define RT_NAME_MAX 8
// <e>Using RT-Thread components initialization
// <i>Using RT-Thread components initialization
#define RT_USING_COMPONENTS_INIT 1
#if RT_USING_COMPONENTS_INIT == 0
#undef RT_USING_COMPONENTS_INIT
#endif 
// </e>
// <e>Using user main
// <i>Using user main
#define RT_USING_USER_MAIN 1
#if RT_USING_USER_MAIN == 0
#undef RT_USING_USER_MAIN
#endif 
// <o>the size of main thread<4-4096:4>
// <i>Default: 512
#define RT_MAIN_THREAD_STACK_SIZE 2048

// <o>the priority of main thread 
#define RT_MAIN_THREAD_PRIORITY 3
// </e>
// </h>




// <h>Debug Configuration
// <e>enable kernel debug configuration
// <i>Default: enable kernel debug configuration 
#define RT_DEBUG 0
#if RT_DEBUG == 0
#undef RT_DEBUG
#endif
// </e>
// <e>enable components initialization debug configuration
// <i>Default: 0
#define RT_DEBUG_INIT 0
#if RT_DEBUG_INIT == 0
#undef RT_DEBUG_INIT
#endif
// </e>
// <e>thread stack over flow detect
// <i> Diable Thread stack over flow detect
#define RT_USING_OVERFLOW_CHECK 0
#if RT_USING_OVERFLOW_CHECK == 0
#undef RT_USING_OVERFLOW_CHECK
#endif
// </e>
// </h>




// <h>Hook Configuration
// <e>using hook
// <i>using hook
#define RT_USING_HOOK 1
#if RT_USING_HOOK == 0
#undef RT_USING_HOOK
#else
//   <o>  idle tasl stack size 
//   <128-1024:4>
#define IDLE_THREAD_STACK_SIZE	384
#endif
// </e>
// <e>using idle hook
// <i>using idle hook
#define RT_USING_IDLE_HOOK 1
#if RT_USING_IDLE_HOOK == 0
#undef RT_USING_IDLE_HOOK
#endif
// </e>
// </h>



// <h>Software timers Configuration
// <e> Enables user timers
// <i> Enables user timers
#define RT_USING_TIMER_SOFT 1
#if RT_USING_TIMER_SOFT == 0
#undef RT_USING_TIMER_SOFT
#endif
// <o>The priority level of timer thread <0-31>
// <i>Default: 4
#define RT_TIMER_THREAD_PRIO 5
// <o>The stack size of timer thread <0-8192>
// <i>Default: 512
#define RT_TIMER_THREAD_STACK_SIZE 512
// <o>The soft-timer tick per second <0-1000>
// <i>Default: 100
#define RT_TIMER_TICK_PER_SECOND RT_TICK_PER_SECOND
// </e>
// </h>



// <h>IPC(Inter-process communication) Configuration
// <e>Using Semaphore
// <i>Using Semaphore
#define RT_USING_SEMAPHORE 1
#if RT_USING_SEMAPHORE == 0
#undef RT_USING_SEMAPHORE
#endif 
// </e>
// <e>Using Mutex
// <i>Using Mutex
#define RT_USING_MUTEX 1
#if RT_USING_MUTEX == 0
#undef RT_USING_MUTEX
#endif 
// </e>
// <e>Using Event
// <i>Using Event
#define RT_USING_EVENT 0
#if RT_USING_EVENT == 0
#undef RT_USING_EVENT
#endif 
// </e>
// <e>Using MailBox
// <i>Using MailBox
#define RT_USING_MAILBOX 1
#if RT_USING_MAILBOX == 0
#undef RT_USING_MAILBOX
#endif 
// </e>
// <e>Using Message Queue
// <i>Using Message Queue
#define RT_USING_MESSAGEQUEUE 1
#if RT_USING_MESSAGEQUEUE == 0
#undef RT_USING_MESSAGEQUEUE
#endif 
// </e>
// </h>





// <h>Memory Management Configuration
// <e>Using Memory Pool Management
// <i>Using Memory Pool Management
#define RT_USING_MEMPOOL 1
#if RT_USING_MEMPOOL == 0
#undef RT_USING_MEMPOOL
#endif 
// </e>
// <e>Dynamic Heap Management
// <i>Dynamic Heap Management
#define RT_USING_HEAP 1
#if RT_USING_HEAP == 0
#undef RT_USING_HEAP
#endif 
// </e>
// <e>using small memory
// <i>using small memory
#define RT_USING_SMALL_MEM 1
#if RT_USING_SMALL_MEM == 0
#undef RT_USING_SMALL_MEM
#endif 
// </e>
// <e>using tiny size of memory
// <i>using tiny size of memory
#define RT_USING_TINY_SIZE 0
#if RT_USING_TINY_SIZE == 0
#undef RT_USING_TINY_SIZE
#endif 
// </e>
// </h>


// <h>Finsh Configuration
// <e>Using finsh shell
// <i>Using finsh shell
#define RT_USING_FINSH 1
#if RT_USING_FINSH == 0
#undef RT_USING_FINSH
#endif

// <e>Using msh support c-style and msh 
// <i>Using msh support c-style and msh
#define FINSH_USING_MSH 1
#if FINSH_USING_MSH == 0
#undef FINSH_USING_MSH
#endif
// </e>

// <e>Using msh only supper msh
// <i>Using msh only supper msh
#define FINSH_USING_MSH_ONLY 0
#if FINSH_USING_MSH_ONLY == 0
#undef FINSH_USING_MSH_ONLY
#endif
// </e>

// <o>the priority of finsh thread <1-7>
// <i>the priority of finsh thread
// <i>Default: 6
#define __FINSH_THREAD_PRIORITY 5
#define FINSH_THREAD_PRIORITY (RT_THREAD_PRIORITY_MAX / 8 * __FINSH_THREAD_PRIORITY + 1)
// <o>the stack of finsh thread <1-4096>
// <i>the stack of finsh thread
// <i>Default: 4096 (4096Byte)
#define FINSH_THREAD_STACK_SIZE 2048

// <e>Using hestory in finsh shell
// <i>Using hestory in finsh shell
#define FINSH_USING_HISTORY 1
#if FINSH_USING_HISTORY == 0
#undef FINSH_USING_HISTORY
#endif
// <o>the history lines of finsh thread <1-32>
// <i>the history lines of finsh thread
// <i>Default: 5
#define FINSH_HISTORY_LINES 5
// </e>

// <e>Using symbol table in finsh shell
// <i>Using symbol table in finsh shell
#define FINSH_USING_SYMTAB 1
#if FINSH_USING_SYMTAB == 0
#undef FINSH_USING_SYMTAB
#endif

// <e>Using description in finsh shell
// <i>Using description table in finsh shell 
#define FINSH_USING_DESCRIPTION 1
#if FINSH_USING_DESCRIPTION == 0
#undef FINSH_USING_DESCRIPTION
#endif
// </e>
// </e>

// <e>Using POSIX(Portable Operating System Interface of UNIX) in finsh shell
// <i>Using POSIX(Portable Operating System Interface of UNIX) in finsh shell 
#define RT_USING_POSIX 0
#if RT_USING_POSIX == 0
#undef RT_USING_POSIX
#endif
// </e>

// <o>the cmd size of finsh thread <1-80>
// <i>the cmd size of finsh thread
#define FINSH_CMD_SIZE 80

// </e>
// </h>


// <h>Kernel Device Object
// <e>RT_USING_DEVICE
// <i>Using kernel device
#define RT_USING_DEVICE 1
#if RT_USING_DEVICE == 0
#undef RT_USING_DEVICE
#endif 

// <h>Console Configuration
// <e>Using console
// <i>Using console
#define RT_USING_CONSOLE 1
#if RT_USING_CONSOLE == 0
#undef RT_USING_CONSOLE
#endif 
// <o>the buffer size of console <1-1024>
// <i>the buffer size of console
// <i>Default: 128 (128Byte)
#define RT_CONSOLEBUF_SIZE 128
// <s>The device name for console
// <i>The device name for console
// <i>Default: uart1
#define RT_CONSOLE_DEVICE_NAME "rtt"
// </e>

// <e>RT_USING_SERIAL
// <i>Using serial device driver
#define RT_USING_SERIAL 1
#if RT_USING_SERIAL == 0
#undef RT_USING_SERIAL
#endif 
// </e>

// <e>RT_USING_PIN
// <i>Using GPIO device driver
#define RT_USING_PIN 0
#if RT_USING_PIN == 0
#undef RT_USING_PIN
#endif 
// </e>
// </h>

// </e>
// </h>

// <h>Portable Operating System Interface of UNIX
// <e>Using POSIX(Portable Operating System Interface of UNIX)
// <i>Using POSIX(Portable Operating System Interface of UNIX) 
#define RT_USING_POSIX 0
#if RT_USING_POSIX==0
#undef RT_USING_POSIX
#endif
// </e>

// <e>Using libc
// <i>Using libc
#define RT_USING_LIBC 1
#if RT_USING_LIBC==0
#undef RT_USING_LIBC
#endif
// </e>
// </h>



// <h>[mynewt nimble config]
// <e>nimble Controller enable
// <i>nimble Controller enable 
#define NIMBLE_CTLR 1
#if NIMBLE_CTLR==0
#undef NIMBLE_CTLR
#endif

// <h>mynewt nimble role config
// <e>peropheral Controller configuretion
// <i>peropheral Controller configuretion 
#define NIMBT_ROLE_PERIPHERAL 1
#if NIMBT_ROLE_PERIPHERAL==0
#undef NIMBT_ROLE_PERIPHERAL
#endif
// </e>

// <e>peropheral broadcaster configuretion
// <i>peropheral broadcaster configuretion 
#define NIMBT_ROLE_BROADCASTER 1
#if NIMBT_ROLE_BROADCASTER==0
#undef NIMBT_ROLE_BROADCASTER
#endif
// </e>

// <o>--nimble debug level choose--
// <i>nimble debug level choose
// <0=> MODLOG_LEVEL_ERROR   
// <1=> MODLOG_LEVEL_WARING  
// <2=> MODLOG_LEVEL_INFO    
// <3=> MODLOG_LEVEL_LOG     
// <4=> MODLOG_LEVEL_RAW     
// <5=> MODLOG_LEVEL_CRITICAL
#define NIMBLE_DEBUG_LEVEL 3
// </h>

// </e>
// </h>

// <<< end of configuration section >>>

#endif
