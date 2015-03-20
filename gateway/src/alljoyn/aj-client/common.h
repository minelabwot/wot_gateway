#ifndef _COMMON_H_
#define _COMMON_H_

#define MAX_RES_NUM		8
#define	CUR_RES_NUM		3

//为了简单起见，除去资源单位与资源类型域，因为网关收到了也没用，反而增加了解析数据结构的复杂性

struct res_info {
	// 发现个问题：若把unit写在最后面，客户端产生了异常，所以就改到了最前面
	char* unit;			// 资源单位
	uint8_t __resid;	// 资源内部编号
	uint8_t type;		// 资源类型(将它以第二参数进行封装)
	
};

struct dev_info {
	char* mac;					// 设备名称
	uint8_t rescnt;				// 资源数目
	uint8_t res[MAX_RES_NUM];	// 资源数组
};

struct data_info {
	char* mac;
	uint8_t __resid;
	double val;
};

extern void get_mac(char macstr[]);

#endif