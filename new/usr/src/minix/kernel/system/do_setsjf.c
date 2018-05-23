#include "kernel/system.h"
#include <stdio.h>
#include <minix/endpoint.h>
#include <minix/config.h>
#include "kernel/clock.h"

/*===========================================================================*
 *				do_setsjf  				     *
 *===========================================================================*/
int do_setsjf(struct proc * caller, message * m_ptr)
{

	struct proc *p;
	int proc_nr, new_priority = SJF_Q,
		new_quantum = SJF_QUANTUM, cpu = -1;
	//TODO switch to generic message type
	//lesser TODO switch to custom message
	int expected_time = m_ptr->m_lsys_krn_schedule.cpu;

	if (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr)) {
		printf("endpoint not ok\n");
		return EINVAL;
	}

	p = proc_addr(proc_nr);
	printf("kernel call from pid=%d\n", proc_nr);

	/* lesser TODO: Only the process itself should be able to modify its SJF status - not specified in task */
	//if (caller != p)
	//	return(EPERM);

	if (expected_time < SJF_MIN_EXP_TIME || SJF_MAX_EXP_TIME < expected_time) 
		return EINVAL;

	if (expected_time == SJF_RESET_POLICY) { // restore scheduling policy to non-SJF default
		if (p->p_priority != SJF_Q) 
			return EPERM;  // process is already scheduled with default method
		if (p->p_scheduler) {  // SCHED manages this process
			new_priority = USER_Q;
		}
		else {  // no scheduler - managed by kernel
			new_priority = TASK_Q;
		}
		new_quantum = USER_QUANTUM;
	}

	else { // switch to scheduling with SJF
		if (p->p_priority == SJF_Q) 
			return EPERM;  // process already scheduled with SJF method
	}

	// TODO: set priority and quantum basing on expected_time
	return sched_proc(p, new_priority, new_quantum, cpu);
}