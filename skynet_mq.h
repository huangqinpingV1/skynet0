#ifndef SKYNET_MESSAGE_QUEUE_H
#define SKYNET_MESSAGE_QUEUE_H

#include <stdlib.h>

//定义队列中节点的数据结构
struct skynet_message{
	int source;
	int destination;
	void* data;
	size_t sz;
};

struct message_queue;

//创建消息队列
struct message_queue* skynet_mq_create(int cap);
//消息队列初始化
void skynet_mq_init(int cap);
//释放消息队列
void skynet_mq_release(struct message_queue *q);
//消息出栈
int skynet_mq_pop(struct skynet_message * message);
//消息入栈
int skynet_mq_push(struct skynet_message * message);

int skynet_mq_leave(struct message_queue *q,struct skynet_message *message);

void skynet_mq_enter(struct message_queue *q,struct skynet_message *message);
#define
