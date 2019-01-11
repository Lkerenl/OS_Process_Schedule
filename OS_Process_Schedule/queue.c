//
//  queue.c
//  OS_Process_Schedule
//
//  Created by kerenl L on 2019/1/10.
//  Copyright © 2019 kerenl L. All rights reserved.
//

#include "queue.h"

phead init_queue(void)
{
    phead new_queue = calloc(1, sizeof(head));
    new_queue->first = NULL;
    new_queue->last = &new_queue->first;
    return new_queue;
}

pnode init_node(void)
{
    pnode new_node = calloc(1,sizeof(node));
    process * psw = calloc(1, sizeof(process));
    new_node->psw = psw;
    return new_node;
}

phead copy_queue(const phead src)
{
    phead dest = init_queue();
    pnode tmp = NULL;
    for_each(tmp, src)
    {
        pnode new = init_node();
        memcpy(new->psw, tmp->psw, sizeof(process));
        queue_insert_tail(dest, new);
    }
    return dest;
}


void echo_process(process * t)
{
    //name level arrived estimated start finshed turnaround
    printf("%c\t%2lu\t%5lu\t%5lu\t%5lu\t%5lu\t%5lu\n",
           t->name,
           (unsigned long)t->level,
           t->arrived_time,
           t->estimated_time,
           t->start_time,
           t->finished_time,
           t->turnaround_time);
}


phead init_process_stats(char * filename)
{
    phead t = init_queue();
    
    FILE * fp = fopen(filename, "r");
    if(fp == NULL)
    {
        fprintf(stderr, "%s\n",strerror(errno));
        exit(-1);
    }
    char buf[20] = {0};
    
    puts("input data:\nname\tlevel\tarrived\testimated");
    
    while(fgets(buf, sizeof(buf), fp))
    {
        pnode new_node = init_node();
        char *save = NULL;
        
        new_node->psw->name = strtok_r(buf, " ", &save)[0];
        new_node->psw->arrived_time = atol(strtok_r(NULL, " ", &save));
        new_node->psw->estimated_time = atol(strtok_r(NULL, " ", &save));
        new_node->psw->level = atol(strtok_r(NULL, " ", &save));
        new_node->psw->remain_time = new_node->psw->estimated_time;
        
        printf("%c\t%2lu\t%5lu\t%5lu\n",
               new_node->psw->name,
               (unsigned long)new_node->psw->level,
               new_node->psw->arrived_time,
               new_node->psw->estimated_time);
        
        queue_insert_tail(t, new_node);
        
        memset(buf, 0, sizeof(buf));
    }
    
    fclose(fp);
    
    return t;
}

void clear_queue(phead t)
{
    pnode a = NULL;
    pnode b = NULL;
    for_each_safe(a, b, t)
    {
        queue_remove_head(t);
        free(a->psw);
        free(a);
    }
}

void update_process_stats(process * run,process * wait,size_t mode)
{
    if (mode)
    {
        //TODO: 抢占式进程状态更新
        if (wait != NULL)
        {
            if (wait->arrived_time != 0)
                wait->start_time = wait->start_time?wait->start_time:mode;
            wait->pause_time = mode;
        }

        run->remain_time -= mode - run->pause_time;
        run->pause_time = mode;
        if(run->remain_time == 0)
        {
            run->finished_time = mode;
            run->turnaround_time = mode -  - run->arrived_time;
        }
        

    }
    else
    {
        run->finished_time = run->start_time + run->estimated_time;
        run->turnaround_time = run->finished_time - run->arrived_time;
        run->remain_time -= run->estimated_time;
        
        if(wait != NULL)
            wait->start_time = run->finished_time;
    }
}

