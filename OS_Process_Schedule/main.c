//
//  main.c
//  OS_Process_Schedule
//
//  Created by kerenl L on 2019/1/11.
//  Copyright © 2019 kerenl L. All rights reserved.
//

#include <stdio.h>
#include "queue.h"

void FCFS(const phead);
void SJF(const phead);
void HP(const phead);
void HPS(const phead);

int main(int argc,char * argv[],char * env[])
{
//    if (argc < 2)
//    {
//        fprintf(stderr,"Usage:<filename> [datafile]");
//        exit(1);
//    }
//    phead data = init_process_stats(argv[1]);

    phead data = init_process_stats("/Users/lkerenl/process");

    printf("\n\n先来先服务:\n");
    FCFS(data);
    printf("\n\n最短运行时间:\n");
    SJF(data);
    printf("\n\n优先级(优先级小优先):\n");
    HP(data);
    printf("\n\n优先级(抢占式):\n");
    HPS(data);

    return 0;
}


void FCFS(const phead s)
{
    phead arrived_list = copy_queue(s);
    
    phead finished_list = init_queue();
    
    pnode running = NULL;
    pnode n = NULL;
    for_each_safe(running, n, arrived_list)
    {
        update_process_stats(running->psw,n?n->psw:(void*)n,0);
        queue_remove(arrived_list, running);
        queue_insert_tail(finished_list, running);
    }
    printf("name level arrived estimated start finshed turnaround\n");
    for_each(running,finished_list)
        echo_process(running->psw);
    
    clear_queue(arrived_list);
    clear_queue(finished_list);
    free(arrived_list);
    free(finished_list);
}


void SJF(const phead s)
{
    phead arrived_list = copy_queue(s);
    phead finished_list = init_queue();
    phead wait_list = init_queue();
    
    pnode running = queue_first(arrived_list);
    queue_remove_head(arrived_list);
    queue_insert_tail(wait_list, running);

    size_t safe_time = 0;
    pnode tsave = NULL;
    for_each_safe(running, tsave, wait_list)
    {
        safe_time += running->psw->estimated_time;
        pnode n = NULL;
        pnode next = NULL;
        for_each_safe(n, next, arrived_list)
        {
            if (n->psw->arrived_time > safe_time)
                break;

            pnode a = NULL;
            pnode b = NULL;

            for_each_safe(a, b, wait_list)
            {
                if (b == NULL || b->psw->estimated_time > n->psw->estimated_time)
                {
                    queue_remove(arrived_list, n);
                    queue_insert_after(wait_list, a, n);
                    break;
                }
            }
        }
        
        tsave = queue_next(running);
        update_process_stats(running->psw, tsave?tsave->psw:(void*)n, 0);
        queue_remove(wait_list, running);
        queue_insert_tail(finished_list, running);
    }

    printf("name level arrived estimated start finshed turnaround\n");
    for_each(running, finished_list)
        echo_process(running->psw);

    clear_queue(arrived_list);
    clear_queue(finished_list);
    clear_queue(wait_list);
    free(arrived_list);
    free(finished_list);
    free(wait_list);
}

void HP(const phead s)
{
    phead arrived_list = copy_queue(s);
    phead finished_list = init_queue();
    phead wait_list = init_queue();
    
    pnode running = queue_first(arrived_list);
    queue_remove_head(arrived_list);
    queue_insert_tail(wait_list, running);
    
    size_t safe_time = 0;
    pnode tsave = NULL;
    for_each_safe(running, tsave, wait_list)
    {
        safe_time += running->psw->estimated_time;
        pnode n = NULL;
        pnode next = NULL;
        for_each_safe(n, next, arrived_list)
        {
            if (n->psw->arrived_time > safe_time)
                break;
            
            pnode a = NULL;
            pnode b = NULL;
            
            for_each_safe(a, b, wait_list)
            {
                if (b == NULL || b->psw->level > n->psw->level)
                {
                    queue_remove(arrived_list, n);
                    queue_insert_after(wait_list, a, n);
                    break;
                }
            }
        }

        tsave = queue_next(running);
        update_process_stats(running->psw, tsave?tsave->psw:(void*)n, 0);
        queue_remove(wait_list, running);
        queue_insert_tail(finished_list, running);
    }
    
    printf("name level arrived estimated start finshed turnaround\n");
    for_each(running, finished_list)
    echo_process(running->psw);
    
    clear_queue(arrived_list);
    clear_queue(finished_list);
    clear_queue(wait_list);
    free(arrived_list);
    free(finished_list);
    free(wait_list);
}


void HPS(const phead s)
{
    phead arrived_list = copy_queue(s);
    phead finished_list = init_queue();
    phead wait_list = init_queue();
    
    pnode running = queue_first(arrived_list);
    queue_remove_head(arrived_list);
    queue_insert_tail(wait_list, running);
    
    size_t safe_time = 0;
    pnode tsave = NULL;
    for_each_safe(running, tsave, wait_list)
    {
        safe_time += running->psw->estimated_time;
        pnode n = NULL;
        // TODO: 抢占式优先级算法

        pnode next = NULL;
        for_each_safe(n, next, arrived_list)
        {
            if (n->psw->arrived_time > safe_time)
                break;
            
            pnode a = NULL;
            pnode b = NULL;
            if(safe_time > n->psw->arrived_time)
                safe_time = n->psw->arrived_time;
            for_each_safe(a, b, wait_list)
            {
                if (b == NULL || b->psw->level > n->psw->level)
                {
                    queue_remove(arrived_list, n);
                    queue_insert_after(wait_list, a, n);
                    break;
                }
            }
        }
        
        tsave = queue_next(running);
        queue_remove(wait_list, running);
        update_process_stats(running->psw, tsave?tsave->psw:(void*)n, safe_time);
        if (running->psw->remain_time == 0) {
            queue_insert_tail(finished_list, running);
        } else {
            pnode a = NULL;
            pnode b = NULL;
            
            for_each_safe(a, b, wait_list)
            {
                if (b == NULL || b->psw->level > running->psw->level)
                {
                    queue_insert_after(wait_list, a, running);
                    break;
                }
            }
        }

    }
    
    printf("name level arrived estimated start finshed turnaround\n");
    for_each(running, finished_list)
    echo_process(running->psw);
    
    clear_queue(arrived_list);
    clear_queue(finished_list);
    clear_queue(wait_list);
    free(arrived_list);
    free(finished_list);
    free(wait_list);
}

