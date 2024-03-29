#ifndef _LINUX_KTHREAD_H
#define _LINUX_KTHREAD_H
/* Simple interface for creating and stopping kernel threads without mess. */
#include <linux/err.h>
#include <linux/sched.h>
#include <lwk/kthread.h>

struct task_struct *kthread_create(int (*threadfn)(void *data),
				   void *data,
				   const char namefmt[], ...)
	__attribute__((format(printf, 3, 4)));

/**
 * kthread_run - create and wake a thread.
 * @threadfn: the function to run until signal_pending(current).
 * @data: data ptr for @threadfn.
 * @namefmt: printf-style name for the thread.
 *
 * Description: Convenient wrapper for kthread_create() followed by
 * wake_up_process().  Returns the kthread or ERR_PTR(-ENOMEM).
 */
#define kthread_run(threadfn, data, namefmt, ...)			   \
({									   \
	struct task_struct *__k						   \
		= kthread_create(threadfn, data, namefmt, ## __VA_ARGS__); \
	if (!IS_ERR(__k))						   \
		wake_up_process(__k);					   \
	__k;								   \
})

static inline int
kthread_stop(struct task_struct *k)
{
	panic("In kthread_stop()... needs to be implemented.");
}

static inline int
kthread_should_stop(void)
{
	panic("In kthread_should_stop()... needs to be implemented.");
}

#endif /* _LINUX_KTHREAD_H */
