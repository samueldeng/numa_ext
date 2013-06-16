/*
 * selector.c
 *
 *  Created on: Jun 7, 2013
 *      Author: samuel
 */

#include <stdio.h>
#include <stdlib.h>
#include <libvirt/libvirt.h>
#include "numax_getinfo.h"


//TODO modify the parameter
#define MEM_THRESHOLD	200*1024	//count in KBytes.
//TODO modify the parameter.
#define CPU_THRESHOLD 0.0 		//count in %
#define MAX_DOMS_NR 128
#define ERROR -1
#define SUCCESS 0

int default_selector(virConnectPtr conn, virDomainPtr domsPtr_sche_lst[], int *domPtr_sche_lst_nxt)
{
	int doms_actvie_nxt = 0;
	virDomainPtr domsPtr_active_lst[MAX_DOMS_NR];
	virDomainInfo domain_info;

	int i = 0;
	int *dom_active_id_lst;
	double dom_cpu_uti = 0.0;

	/*
	 * 	get the active domain, store the domian_ptr in doms_active_Ptr_lst[i];
	 */
	doms_actvie_nxt = virConnectNumOfDomains(conn);
	if(doms_actvie_nxt < 0){
		perror("failed to execute virConnectNumOfDomains");
		return ERROR;
	}
	dom_active_id_lst = malloc(sizeof(int) * doms_actvie_nxt);
	doms_actvie_nxt = virConnectListDomains(conn, dom_active_id_lst, doms_actvie_nxt);
	if(doms_actvie_nxt < 0){
		perror("failed to execute virConnectListDomains");
		return ERROR;
	}

	for(i = 0; i < doms_actvie_nxt; i++){
		domsPtr_active_lst[i] = virDomainLookupByID(conn, dom_active_id_lst[i]);
		if(domsPtr_active_lst[i] == NULL){
			perror("failed to execute virDomainLookupByID");
			return ERROR;
		}

	}
	free(dom_active_id_lst);
	dom_active_id_lst = NULL;

	/*
	 * 	query every domains's used memory
	 * 	set domsPtr_active_lst[i] = NULL,
	 * 	if the domain's mem is less than the MEM_THREASHOLD.
	 */

	for(i = 0; i < doms_actvie_nxt; i++){
		if(virDomainGetInfo(domsPtr_active_lst[i], &domain_info) < 0){
			perror("failed to virDomainGetInfo");
			return ERROR;
		}

		//DEBUG print every domain memory.
		printf("dom_mem_max : %d\n", domain_info.maxMem);
		printf("dom_mem_used : %d\n", domain_info.memory);

		if(domain_info.memory < MEM_THRESHOLD){
			free(domsPtr_active_lst[i]);
			domsPtr_active_lst[i] = NULL;
		}
	}

	/*
	 * 	query every domain CPU total utilization.
	 * 	populate domsPtr_sche_lst
	 */
	*domPtr_sche_lst_nxt = 0;
	for(i = 0; i < doms_actvie_nxt; i++){
		if(domsPtr_active_lst[i] != NULL){
			dom_cpu_uti = domain_get_cpu_util(conn, domsPtr_active_lst[i]);
			//DEBUG print every domain cpu_utili.
			printf("dom_cpu_utilization : %f\n",dom_cpu_uti);
			if(dom_cpu_uti > CPU_THRESHOLD)
				domsPtr_sche_lst[(*domPtr_sche_lst_nxt)++] = domsPtr_active_lst[i];
			else
				continue;
		}
	}
	return SUCCESS;
}
