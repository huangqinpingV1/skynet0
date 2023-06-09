#include "skyney_mq.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//定义消息队列
struct message_queue{
	int cap;//消息队列容量
	int head;//队列中消息头的下标
	int tail;//队列中消息尾的下标
	int lock;
	struct skynet_message *queue;
};
static struct message_queue*  Q  = NULL;

//创建一个长度cap的队列
struct message_queue* skynet_mq_create(int cap){
	struct message_queue *q = malloc(sizeof(*q));
	q->cap = cap;
	q->head = 0;
	q->tail = 0;//尾部消息在数组中的下标
	q->lock = 0;//队列锁

	q->queue = malloc(sizeof(struct skynet_message)*cap);

	return q;
}

//队列初始化
void 
skynet_mq_init(int cap) {
	Q = skynet_mq_create(cap);
}

//释放队列
void
skynet_mq_release(struct message_queue *q) {
	free(q->queue);
	free(q);
}

//给队列锁
static inline void
_lock_queue(struct message_queue *q) {
	while (__sync_lock_test_and_set(&q->lock,1)) {}
}

//队列解锁
static inline void
_unlock_queue(struct message_queue *q) {
	__sync_lock_release(&q->lock);
}


//一条消息入队列
void skynet_mq_enter(struct message_queue *q, struct skynet_message *message){
	//加锁互斥
	_lock_queue(q);
	//加入的message直接放到队列的尾部
	q->queue[q->tail]  = *message;
	//q->tail+1,如果过队列已满，则q->tail==q->head ==0,
	if(++ q->tail >= q->cap){
		q->tail =  0;
	}
	
	//队列已满，扩展为2*cap容量的新队列
	if(q->head == q->tail){
		struct message_queue *new_queue = malloc(sizeof(struct skynet_message)*q->cap*2);
		int i;
		//复制消息
		for(i =  0;i<q->cap;i++){
			new_queue[i]= q->queue[(q->head+i) % q->cap];
		}
		q->head = 0;
		q->tail  = q->cap;
		q->cap* =2;
		//释放旧的消息数组	
		free(q->queue);
		q->queue = new_queue;
	}
	//操作完成，队列解锁
	_unlock_queue(q);

}

void skynet_mq_push(struct skynet_message *message){
	skynet_mq_enter(Q,message);
}

//取出一个消息队列,返回该消息的destination字段和 message结构体
int skynet_mq_leave(struct message_queue *q,struct skynet_message *message){
	int ret = -1;
	_lock_queue(q);
	//队列不空
	if(q->head != q->tail){
		//获取数组队列头部消息
		*message  = q->queue[q->head];
		ret = message->destination;
		//队列头下标q->head+1,如果到队列尾部了则队列头从0开始
		if(++ q->head >= q->cap){
			q->head =  0;
			printf("skynet_mq_leave  取完时p->tail =%d\n",p->tail);
		}
	}
	_unlock_queue(q);
	return ret;
}

int skynet_mq_op(struct skynet_message *message){
	return skynet_mq_leave(Q,message);
}
