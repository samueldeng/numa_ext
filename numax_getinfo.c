/*
 * numax_getinfo.c
 *
 *  Created on: Jun 8, 2013
 *      Author: samuel
 */


#include <stdio.h>
#include <stdlib.h>
#include <libvirt/libvirt.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include "numax_getinfo.h"


void die_with_erro(char *mes)
{
        perror(mes);
        exit(ERROR);
}

double domain_get_cpu_util(virConnectPtr conn, virDomainPtr domain_p) {
        virDomainInfo info;
        double cpu_time_fir; 		//cnt in ns
        double cpu_time_sec;		//cnt in ns
        struct timeval sys_time_fir;	//cnt in us
        struct timeval sys_time_sec;	//cnt in us
        double cpu_time_elap;		//cnt in us
        double sys_time_elap;		//cnt in us

        /*
         *	first snapshot of CPU time and current_system_time;
         */
        if ((virDomainGetInfo(domain_p, &info)) < 0)
                die_with_erro("failed virDomainGetInfo to get the CPU time");
        cpu_time_fir = (double)info.cpuTime;

        if (gettimeofday(&sys_time_fir, NULL) == -1)
                die_with_erro("failed to gettimeofday.");

        if(sleep(SLEEP_INTERVAL) != 0)
                die_with_erro("failed to sleep for a while");

        /*
         *	second snapshot of CPU time and current_system_time;
         */
        if ((virDomainGetInfo(domain_p, &info)) < 0)
                die_with_erro("failed virDomainGetInfo to get the CPU time");
        cpu_time_sec = (double)info.cpuTime;

        if (gettimeofday(&sys_time_sec, NULL) == -1)
                die_with_erro("failed to gettimeofday.");

        /*
         *	calculate the CPU utilization.
         */
        cpu_time_elap = (cpu_time_sec - cpu_time_fir) / 1000;
        sys_time_elap = 1000000 * (sys_time_sec.tv_sec - sys_time_fir.tv_sec) +
                        (sys_time_sec.tv_usec - sys_time_fir.tv_usec);
        return cpu_time_elap / (double)sys_time_elap;
}

int get_numa_node_nr()
{
        //TODO finish this function.
        return 2;
}

int get_pcpus_nr()
{
        return 8;
        /*	This Version was implemented by analyze  from xm info -n

		int i;
		FILE *numa_info_file_p;
		char results_cmd[MAX_CHARS_ONE_LINE];

		memset(results_cmd, 0, sizeof results_cmd);

		if(system("xm info -n | head -n 5 | tail -n 1 > numa_info") < 0){
			perror("failed to execute system() to get xm info");
			return ERROR;
		}
		if((numa_info_file_p = fopen("node_args.info", 'r')) < 0){
			perror("failed to fopen the file.");
			return -1;
		}

		if(fgets(results_cmd, MAX_CHARS_ONE_LINE, numa_info_file_p) == NULL){
			perror("failed to fgets");
			return ERROR;
		}


         * 	Enumerate the results_cmd to find the first digit character.

		char *num_p;
		for(i = 0; i < MAX_CHARS_ONE_LINE; i++){
			if(results_cmd[i] >'0' && results_cmd[i] < '9'){
				num_p = &results_cmd[i];
			}
		}
		int pcpu_nr = atoi(num_p);
		fclose(numa_info_file_p);
		return pcpu_nr;
         */

        /*	This version was implemented by virNodeGetInfo()

         */


}

int parse_domain_mem_alloc(int dom_id, int dom_mem_alloc[])
{
        int i;
        FILE *file_p;
        char cmd[MAX_CHARS_ONE_LINE];
        int nodes_nr = get_numa_node_nr();
        char file_name[MAX_LEN_FILE_NAME];
        char buff[MAX_FILE_BUFF_SIZE];
        /*
         *      'sed' to grep the domain memory information
         *      store the into mem_info_DomX
         */
        sprintf(cmd, "cat mem_distri | sed -n '/Domain %d /,+%dp' | tail -n %d > mem_info_Dom%d", dom_id, nodes_nr, , dom_id);
        if(system(cmd) < 0){
                perror("failed to system the 'sed' to grep domainX");
                return ERROR;
        }

        sprintf(file_name, "mem_info_Dom%d", dom_id);
        if((file_p = fopen(file_name, "r")) == NULL){
                perror("failed to open file 'mem_distri.info'");
                return ERROR;
        }
        for(i = 0; i < nodes_nr; i++){
                fgets(buff, MAX_FILE_BUFF_SIZE, file_p);
                strtok(buff, ":");
                char *num_p = strtok(NULL, ":");
                dom_mem_alloc[i] = atoi(num_p);
        }
        fclose(file_p);

        sprintf(cmd, "rm mem_info_Dom%d", dom_id);
        if(system(cmd) < 0){
                perror("failed to system the 'rm' to remove temperate file");
                return ERROR;
        }
        return SUCCESS;

}

int get_node_distance_matrix(int node_dist_matrix[][2], int nodes_nr)
{
        //TODO get_node_distance: Now only support nodes_nr = 2;
        if(nodes_nr != 2){
                perror("you are not running on the 184, and fix this bug soon.");
                return ERROR;
        }
        node_dist_matrix[0][0] = 10;node_dist_matrix[0][1] = 20;
        node_dist_matrix[1][0] = 20;node_dist_matrix[1][1] = 10;
        return SUCCESS;
}

int get_node_pcpumap(unsigned char* node_cpumap, int node_cpumap_len, int node_nr)
{
        //TODO get_node_pcpumap;
        if(node_cpumap_len != 1){
                perror("you are not running on the 184 and fix this bug soon");
                return ERROR;
        }
        if(node_nr == 0){
                *node_cpumap = 0b00001111;
        }else if(node_nr == 1){
                *node_cpumap = 0b11110000;
        }else{
                return ERROR;
        }
        return SUCCESS;
}
