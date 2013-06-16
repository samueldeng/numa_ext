/*
 * numa_text.c
 *
 *  Created on: Jun 5, 2013
 *      Author: samuel
 *
 *  Compile with: gcc -g -Wall ex1.c -o ex -lvirt
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libvirt/libvirt.h>
#include <string.h>
#include "numa_ext.h"
#include "scheduler.h"
#include "selector.h"
#include "numax_getinfo.h"

int main(int argc, char *argv[])
{

	int i;
	virConnectPtr conn;
	virDomainPtr domPtr_sche_lst[MAX_DOMS_NR];
	int domPtr_sche_lst_nxt = 0;

	if ((conn = virConnectOpen("xen:///"))== NULL){
		perror("failed to execute virConnectOpen function");
		return ERROR;
	}
	memset(domPtr_sche_lst, 0, sizeof domPtr_sche_lst);

	while(1){
		if(default_selector(conn, domPtr_sche_lst, &domPtr_sche_lst_nxt) == ERROR){
			perror("failed to execute the default_selector");
			return ERROR;
		}
		if(default_scheduler(domPtr_sche_lst, domPtr_sche_lst_nxt) == ERROR){
			perror("failed to execute the default_scheduler");
			return ERROR;
		}
		for(i = 0; i < domPtr_sche_lst_nxt; i++)
			free(domPtr_sche_lst[i]);
		break;
	}
	virConnectClose(conn);
	return SUCCESS;
}
