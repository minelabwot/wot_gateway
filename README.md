# wot_gateway
this is the code of minelab-wot

这是基于microwrt的异构网络实现。以microwrt为网关，以microduino节点（基于zigbee通信）和microwrt节点（基于alljoyn通信）。

该套系统实现了基于异构网络的节点即插即用和基于web of sense的物联网资源url化。具体实现了以下功能：
1、能够实现节点资源的即插即用。插上后网关、服务器即可识别，拔出后网关和服务器即可删除相关信息；
2、能够实现节点资源的web化。可以将节点的资源内容全部转换成URL，只要调用URL，即可实现节点信息的查询和节点的控制。

详细使用方法在“gateway/doc/”下，附带两个使用说明，一个是详细的配置过程，一个是傻瓜式的使用方法。可以随意使用。


minelabmakes
