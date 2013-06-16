/*
 * scheduler.h
 *
 *  Created on: Jun 8, 2013
 *      Author: samuel
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_



#endif /* SCHEDULER_H_ */
#include <limits.h>


#define SUCCESS 0
#define ERROR -1

#define MAX_NODES_NR 16
#define INF INT_MAX
#define BITS_IN_ONE_BYTES 8
#define MAX_VCPU_IN_DOMAINS 8


/*
 *
 */
int default_scheduler(virDomainPtr domPtr_sche_lst[], int domPtr_sche_lst_nxt);

/*
 *
 */
int calc_cost(int which_node_to_cal, int dom_mem_alloc[], int node_dis_matrix[][2], int nodes_nr);
