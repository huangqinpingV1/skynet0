#ifndef SKYNET_IMP_H
#define SKYNET_IMP_H
struct skynet_config {
	int thread; //线程数
	int mqueue_size;//队列大小
	char* logger;//日志相关
	const char* module_path;//模块路径
};

//启动服务
void skynet_start(struct skynet_config * config);
#endif
