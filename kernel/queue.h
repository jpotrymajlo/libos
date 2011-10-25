
#ifndef __QUEUE_H__
#define __QUEUE_H__

extern UINT32  queue_init     (queue_t* pCont);
extern void    queue_init_node(node_t* pNode);
extern UINT32  queue_push     (queue_t* lpHead,node_t* tsk_dscr);
extern UINT32  queue_add      (queue_t* lpHead,node_t* tsk_dscr);
extern UINT32  queue_insert   (queue_t* pCont,node_prior_t* pNode);
extern void*   queue_pop      (queue_t* pHead);
extern UINT32  queue_remove   (queue_t* pHead,node_t* tsk_dscr);




#endif

