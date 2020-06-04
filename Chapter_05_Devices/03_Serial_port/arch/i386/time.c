/*! Timer in arch layer; use 'arch_timer_t' device defined in configuration */

#include "time.h"
#include <kernel/kprint.h>

#include <types/time.h>

extern arch_timer_t TIMER;
static arch_timer_t *timer = &TIMER;

static timespec_t clock;	/* system time starting from 0:00 at power on */
static int steps_until_handler = 0; /*Will overflow in around 4 days for signed int!*/

static void (*alarm_handler) (); /* kernel function - call when alarm given by
				    kernel ('delay') expires */

static void arch_timer_handler (); /* whenever timer expires call this */

void arch_enable_timer_interrupt ()	{ timer->enable_interrupt ();	}
void arch_disable_timer_interrupt ()	{ timer->disable_interrupt ();	}

void arch_get_min_interval ( timespec_t *time )
{
	*time = timer->min_interval;
}

/*Somewhat bad, but I don't think I'm supposed to touch the timer driver*/
static void arch_get_interrupt_interval ( timespec_t *time ) {
	time->tv_sec = 0;
	time->tv_nsec = 200000;
}

/*! Initialize timer 'arch' subsystem: timer device, subsystem data */
void arch_timer_init ()
{
	/* kod postavljanja alarma treshold nije bitan,
	a timer se puni vrijednosti za 200us */

	timespec_t interrupt_interval;
	arch_get_interrupt_interval(&interrupt_interval);

	clock.tv_sec = clock.tv_nsec = 0;
	alarm_handler = NULL;

	timer->init ();

	//kprintf("setting interrupt interval to %d, %d\n", interrupt_interval.tv_sec, interrupt_interval.tv_nsec);

	timer->set_interval ( &interrupt_interval );
	timer->register_interrupt ( arch_timer_handler );
	timer->enable_interrupt ();

	
	return;
}

/*!
 * Set next timer activation
 * \param time Time of next activation
 * \param alarm_func Function to call upon timer expiration
 */
void arch_timer_set ( timespec_t *time, void *alarm_func )
{
	/* Vise ne dirati timer, cak niti get_interval_remainder */

	/* Which function to call */
	alarm_handler = alarm_func;

	/*Compute how many interrupts until the function needs to be called
	 - easier than fiddling with times
	 - add 1 if offset from 200us alignment is more than 100us, for a bit better resolution!
	 */
	steps_until_handler = 5000*(time->tv_sec); //Each second is 5000 200us increments

						   //microseconds, rounded down, divided by number of 200us increments, rounded down	
	steps_until_handler += (time->tv_nsec / 1000) / 200; //Truncate to lower number of microseconds

	int remaining_nanoseconds = (time->tv_nsec) % (200000);
	//More than 100 us remaining
	if(remaining_nanoseconds >= 100000) {
		steps_until_handler++;
	}

	//kprintf("Arrived at %d 200us increments for %d seconds and %d nanoseconds!\n", steps_until_handler, time->tv_sec, time->tv_nsec);
}

/*!
 * Get 'current' system time
 * \param time Store address for current time
 */
void arch_get_time ( timespec_t *time )
{
	/* best we can do, since we cannot access remaining time
	from the timer */
	*time = clock;
}

/*!
 * Set 'current' system time
 * \param time Time to set as current
 * NOTE: changing clock may have unpredicted behavior on timers!
 */
void arch_set_time ( timespec_t *time )
{
	void (*k_handler) ();

	clock = *time;
	
	/* let kernel handle time shift problems */
	if ( alarm_handler )
	{
		k_handler = alarm_handler;
		alarm_handler = NULL; /* reset kernel callback function */
		k_handler ();
	}
}

/*!
 * Registered 'arch' handler for timer interrupts;
 * update system time and forward interrupt to kernel if its timer is expired
 */
static void arch_timer_handler ()
{
	static int steps = 0;

	steps++;

	//kprintf("%d\n", steps);
	void (*k_handler) ();

	/* advance the clock in any case */
	timespec_t interrupt_interval;
	arch_get_interrupt_interval(&interrupt_interval);
	time_add(&clock, &interrupt_interval);

	//kprintf("should print every 200us....\n");

	/*if there is a timer registered, reduce it's steps*/
	if(alarm_handler) {
		--steps_until_handler;

		/* if done reducing */
		if(steps_until_handler <= 0) {
			k_handler = alarm_handler;
			alarm_handler = NULL; /* reset kernel callback function */
			k_handler (); /* forward interrupt to kernel */
		}
	}
}
