#ifndef _LINUX_TIMER_H
#define _LINUX_TIMER_H

#include <linux/list.h>
#include <linux/ktime.h>
#include <linux/stddef.h>
#include <linux/debugobjects.h>
#include <lwk/timer.h>

struct tvec_base;

#define timer_list timer

#define TIMER_INITIALIZER(_function, _expires, _data) {		\
		/*.link = LIST_HEAD_INIT(.link), */	 \
		.function = (_function),			\
		.expires = (_expires),				\
		.data = (_data),				\
	}

#define DEFINE_TIMER(_name, _function, _expires, _data)		\
	struct timer_list _name =				\
		TIMER_INITIALIZER(_function, _expires, _data)

void init_timer(struct timer_list *timer);
void init_timer_deferrable(struct timer_list *timer);

#ifdef CONFIG_DEBUG_OBJECTS_TIMERS
extern void init_timer_on_stack(struct timer_list *timer);
extern void destroy_timer_on_stack(struct timer_list *timer);
#else
static inline void destroy_timer_on_stack(struct timer_list *timer) { }
static inline void init_timer_on_stack(struct timer_list *timer)
{
	init_timer(timer);
}
#endif

static inline void setup_timer(struct timer_list * timer,
				void (*function)(unsigned long),
				unsigned long data)
{
	timer->function = function;
	timer->data = data;
	init_timer(timer);
}

static inline void setup_timer_on_stack(struct timer_list *timer,
					void (*function)(unsigned long),
					unsigned long data)
{
	timer->function = function;
	timer->data = data;
	init_timer_on_stack(timer);
}

/**
 * timer_pending - is a timer pending?
 * @timer: the timer in question
 *
 * timer_pending will tell whether a given timer is currently pending,
 * or not. Callers must ensure serialization wrt. other operations done
 * to this timer, eg. interrupt contexts, or other CPUs on SMP.
 *
 * return value: 1 if the timer is pending, 0 if not.
 */
static inline int timer_pending(const struct timer_list * timer)
{
	return !list_empty(&timer->link);
}

extern void add_timer_on(struct timer_list *timer, int cpu);
extern int del_timer(struct timer_list * timer);
extern int __mod_timer(struct timer_list *timer, unsigned long expires);
extern int mod_timer(struct timer_list *timer, unsigned long expires);

/*
 * The jiffies value which is added to now, when there is no timer
 * in the timer wheel:
 */
#define NEXT_TIMER_MAX_DELTA	((1UL << 30) - 1)

/*
 * Return when the next timer-wheel timeout occurs (in absolute jiffies),
 * locks the timer base:
 */
extern unsigned long next_timer_interrupt(void);
/*
 * Return when the next timer-wheel timeout occurs (in absolute jiffies),
 * locks the timer base and does the comparison against the given
 * jiffie.
 */
extern unsigned long get_next_timer_interrupt(unsigned long now);

/*
 * Timer-statistics info:
 */
#ifdef CONFIG_TIMER_STATS

#define TIMER_STATS_FLAG_DEFERRABLE	0x1

extern void init_timer_stats(void);

extern void timer_stats_update_stats(void *timer, pid_t pid, void *startf,
				     void *timerf, char *comm,
				     unsigned int timer_flag);

extern void __timer_stats_timer_set_start_info(struct timer_list *timer,
					       void *addr);

static inline void timer_stats_timer_set_start_info(struct timer_list *timer)
{
	__timer_stats_timer_set_start_info(timer, __builtin_return_address(0));
}

static inline void timer_stats_timer_clear_start_info(struct timer_list *timer)
{
	timer->start_site = NULL;
}
#else
static inline void init_timer_stats(void)
{
}

static inline void timer_stats_timer_set_start_info(struct timer_list *timer)
{
}

static inline void timer_stats_timer_clear_start_info(struct timer_list *timer)
{
}
#endif

/**
 * add_timer - start a timer
 * @timer: the timer to be added
 *
 * The kernel will do a ->function(->data) callback from the
 * timer interrupt at the ->expires point in the future. The
 * current time is 'jiffies'.
 *
 * The timer's ->expires, ->function (and if the handler uses it, ->data)
 * fields must be set prior calling this function.
 *
 * Timers with an ->expires field in the past will be executed in the next
 * timer tick.
 */
static inline void add_timer(struct timer_list *timer)
{
	BUG_ON(timer_pending(timer));
	__mod_timer(timer, timer->expires);
}

#ifdef CONFIG_SMP
  extern int try_to_del_timer_sync(struct timer_list *timer);
  extern int del_timer_sync(struct timer_list *timer);
#else
# define try_to_del_timer_sync(t)	del_timer(t)
# define del_timer_sync(t)		del_timer(t)
#endif

#define del_singleshot_timer_sync(t) del_timer_sync(t)

extern void init_timers(void);
extern void run_local_timers(void);
struct hrtimer;
extern enum hrtimer_restart it_real_fn(struct hrtimer *);

unsigned long __round_jiffies_relative(unsigned long j, int cpu);
unsigned long round_jiffies_relative(unsigned long j);


/**
 * This is supposed to round the input jiffies value to a multiple
 * of one second.  Linux does a whole bunch of stuff to add some
 * skew for each CPU and what not.  The LWK doesn't bother and
 * assumes the caller passed in something fairly close to being
 * a multiple of a second.
 */
static inline unsigned long
round_jiffies(unsigned long j)
{
	return j;
}

#endif
