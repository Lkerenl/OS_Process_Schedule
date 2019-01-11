//
//  queue.h
//  OS_Process_Schedule
//
//  Created by kerenl L on 2019/1/10.
//  Copyright © 2019 kerenl L. All rights reserved.
//

#ifndef queue_h
#define queue_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

typedef struct PSW {
    size_t arrived_time;    //到达时间
    size_t estimated_time;  //估计运行时间
    size_t start_time;      //开始时间
    size_t finished_time;   //完成时间
    
    size_t remain_time;     //剩余时间
    size_t pause_time;    //暂停时间
    char   level;           //优先级
    
    size_t turnaround_time; //周转时间
    
    char   name;            //名称
} process;

typedef struct _Node {
    struct _Node * next;
    process * psw;
} node;

typedef node * pnode;

typedef struct _Que {
    struct _Node * first;
    struct _Node ** last;
} head;

typedef head * phead;


#define queue_empty(h) ((h)->first == NULL)
#define queue_first(h) ((h)->first)
#define queue_next(e) ((e)->next)
#define for_each(var, h) for ( (var) = queue_first(h); (var); (var) = queue_next(var))
#define for_each_safe(var, save, h) \
for ((var) = queue_first(h); (var)&&((save)=queue_next(var),1); (var)=(save) )

#define queue_insert_head(h,e) do { \
queue_next(e) = queue_first(h);    \
queue_first(h) = (e);   \
}while(0)

#define queue_insert_tail(h,e) do {\
queue_next(e) = NULL; \
*(h)->last = (e);\
(h)->last = &queue_next(e);\
}while(0)

#define queue_insert_after(h,t,e) do {                      \
if (((e)->next = (t)->next) == NULL)                          \
    (h)->last = &(e)->next;                       \
queue_next(t) = (e);                                        \
}while(0)

#define queue_remove_head(h) do {                           \
if ((queue_first(h) = queue_next(queue_first(h))) == NULL)    \
    (h)->last = &queue_first(h);                        \
}while(0)

#define queue_remove_after(h,e) do {\
    if ((queue_next(e) = queue_next(queue_next(e))) == NULL)\
        (h)->last = &queue_next(e);\
}while(0)

#define queue_remove(h,e) do {\
    if (queue_first(h) == (e)){\
        queue_remove_head(h);\
    }else{\
        pnode tmp = queue_first(h);\
        while(queue_next(tmp)!=(e))\
            tmp = queue_next(tmp);\
        queue_remove_after((h),(tmp));\
    }\
    queue_next(e) = NULL;\
}while(0)

phead init_queue(void);
pnode init_node(void);
void clear_queue(phead);
phead init_process_stats(char * filename);

phead copy_queue(const phead);
void echo_process(process * );
void update_process_stats(process *, process * , size_t);


#endif /* queue_h */
