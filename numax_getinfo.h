/*
 * numax_getinfo.h
 *
 *  Created on: Jun 8, 2013
 *      Author: samuel
 */

#ifndef NUMAX_GETINFO_H_
#define NUMAX_GETINFO_H_



#endif /* NUMAX_GETINFO_H_ */

#define SLEEP_INTERVAL  1
#define SUCCESS 0
#define ERROR -1
#define MAX_CHARS_ONE_LINE 80
#define MAX_LEN_FILE_NAME 64
#define MAX_FILE_BUFF_SIZE 64

/*
 *
 */
void die_with_erro(char *mes);

/*
 *
 */
double domain_get_cpu_util(virConnectPtr conn, virDomainPtr domain_p);

/*
 *
 */
int get_numa_node_nr();

/*
 *
 */
int parse_domain_mem_alloc(int dom_id, int dom_mem_alloc[]);

/*
 *
 */
int get_pcpus_nr();

/*
 *
 */
int get_node_pcpumap(unsigned char* node_cpumap, int node_cpumap_len, int node_nr);

/*
 *
 */
int get_node_distance_matrix(int node_dist_matrix[][2], int nodes_nr);
