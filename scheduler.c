/*
 * scheduler.c
 *
 *  Created on: Jun 8, 2013
 *      Author: samuel
 */

#include <stdio.h>
#include <stdlib.h>
#include <libvirt/libvirt.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "numax_getinfo.h"
#include "scheduler.h"

int default_scheduler(virDomainPtr domPtr_sche_lst[], int domPtr_sche_lst_nxt)
{
        int i = -1;
        int nodes_nr = nodes_nr = get_numa_node_nr();
        int node_dist_matrix[MAX_NODES_NR][MAX_NODES_NR];
        memset(node_dist_matrix, 0, sizeof node_dist_matrix);

        /*
         *	Get the Node Distance Matrix.
         */
        if(get_node_distance_matrix(node_dist_matrix, nodes_nr) < 0){
                perror("failed to get the node distance matrix");
                return ERROR;
        }

        for(i = 0; i < domPtr_sche_lst_nxt; i++){
                int dom_id;
                if((dom_id = virDomainGetID(domPtr_sche_lst[i])) < 0){
                        perror("failed to execute virDomainGetInfo");
                        return ERROR;
                }

                /*
                 *      Get domain's memory distribution:
                 *              execute cmd_lines and save & tail the memory information into mem_distri.
                 *              Store each domain memory information in integer array "dom_mem_alloc";
                 */

                char dmesg_cmd[MAX_CHARS_ONE_LINE];
                int dom_mem_alloc[MAX_NODES_NR];

                system("xm debug-keys u");
                sprintf(dmesg_cmd, "xm dmesg | tail -n %d > mem_distri", (nodes_nr + 1) * domPtr_sche_lst_nxt);
                system(dmesg_cmd);

                memset(dom_mem_alloc, 0, sizeof dom_mem_alloc);
                if(parse_domain_mem_alloc(dom_id, dom_mem_alloc) < -1){
                        perror("failed to parser the dom_mem_alloc");
                        return ERROR;
                }

                /*
                 *      Get one domain's all VCPU pointer for Binding.
                 *      store the VCPU pointer in vcpuinfo_p, nr_vpus besides.
                 */
                virVcpuInfo vcpuinfo_lst[MAX_VCPU_IN_DOMAINS];
                int nr_vcpus = MAX_VCPU_IN_DOMAINS;
                unsigned char *cpumaps;
                int cpumap_len = get_pcpus_nr()/BITS_IN_ONE_BYTES;
                int j;

                cpumaps = calloc(nr_vcpus, cpumap_len);

                nr_vcpus = virDomainGetVcpus(domPtr_sche_lst[i], vcpuinfo_lst, nr_vcpus, cpumaps, cpumap_len);
                if (nr_vcpus < 0){
                        perror("failed to virDomainGetVcpus");
                        exit(ERROR);
                }
                free(cpumaps);
                for(j = 0; j < nr_vcpus; j++){
                        /*
                         *      calculate best node.
                         */
                        int k;
                        int min_cost = INF;
                        int best_node = -1;
                        for(k = 0; k < nodes_nr; k++){
                                int cost_for_this_node = calc_cost(k, dom_mem_alloc, node_dist_matrix, nodes_nr);
                                if(cost_for_this_node < min_cost){
                                        best_node = k;
                                        min_cost = cost_for_this_node;
                                }
                        }

                        /*
                         *      bind VCPU to PCPU.
                         */
                        for(k = 0; k < nr_vcpus; k++){
                                /*
                                 *      Generate the cpumap and cpumap_len
                                 */
                                unsigned char *best_node_cpumap;
                                //TODO if the map_len can't divide by 8.something goes wrong. must be xiangshangquzheng.
                                int best_node_cpumap_len = get_pcpus_nr() / BITS_IN_ONE_BYTES;
                                if((best_node_cpumap = (unsigned char*)malloc(best_node_cpumap_len)) == NULL){
                                        perror("failed to mallc the best_node_cpumap");
                                        return ERROR;
                                }
                                if (get_node_pcpumap(best_node_cpumap, best_node_cpumap_len, best_node) < 0){
                                        perror("failed to get_node_pcpumap");
                                        return ERROR;
                                }

                                int isSucc = virDomainPinVcpu(domPtr_sche_lst[i], vcpuinfo_lst[k].number, best_node_cpumap, best_node_cpumap_len);
                                if(isSucc < 0){
                                        perror("failed to bind the VCPU.");
                                        return ERROR;
                                }
                                free(best_node_cpumap);
                        }
                        //DEBUG print success info
                        printf("dom %d: %d VCPU has been binded to Node.\n",i , k);
                }
                //DEBUG print success info
                printf("dom %d 's ALL VCPU has been binded to PCPU.\n", i);
        }
        return SUCCESS;
}

//TODO the 2 dimensional array's second dimensional number must be assign. temporally assign it to 2
int calc_cost(int which_node_to_cal, int dom_mem_alloc[], int node_dis_matrix[][2], int nodes_nr)
{
        int i;
        int sum_cost = 0;
        for(i = 0; i < nodes_nr; i++){
                sum_cost += dom_mem_alloc[i] * node_dis_matrix[which_node_to_cal][i];
        }
        return sum_cost;
}

int google_scheduler(virDomainPtr domPtr_sche_lst[], int domPtr_sche_lst_nxt)
{
        //TODO version 2: google scheduler.
        return -1;
}
