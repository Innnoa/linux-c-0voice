#include <array>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
using namespace std;
constexpr auto conn_buff = 1024; //buff的大小
constexpr auto conn_num = 1024; // 结构体conn_item对象的数量

typedef ssize_t (*rcallback) ( int fd ); //声明rcallback方法指针,可以视为一种接口,接收符合该接口的方法

struct conn_item { //声明一个结构体,存储以事件触发的对象
	int fd; //fd标识符

	char rbuffer[conn_buff]; //read_buffer 存储接收的内容
	ssize_t rlen; //rlen 存储接收的长度
	rcallback read_callback; //党的的一个recv方法

	char wbuffer[conn_buff]; //write_buffer 存储发送的内容
	ssize_t wlen; //wlen 存储发送的长度
	rcallback write_callback; //单独的一个write方法
} connlist[conn_num]; //对象名

int epfd; // epoll的fd

ssize_t accept_cb ( int sockfd ); //accept的回调函数

ssize_t recv_cb ( int connfd ); //recv的回调函数

ssize_t send_cb ( int connfd ); //send的回调函数


void set_event ( const int fd, const int event, const int flag ) { //设置事件函数,统一调用单发
	if (flag) { // 如果没创建过
		epoll_event ev {}; // epoll的事件初始化
		ev.events = event; // 赋值events
		ev.data.fd = fd; //赋值data.fd
		epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev); //epoll的特有函数,设置该事件内容
	} else { //如果创建过
		epoll_event ev {}; // 同理
		ev.events = event; // 同理
		ev.data.fd = fd; //同理
		epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev); // 同理 事件不同
	}
}

//listenfd
//EPOLLIN
ssize_t accept_cb ( const int sockfd ) { //监听函数 ,用于服务端接收客户端的连接请求
	sockaddr_in clientaddr {}; // sock编程的内置结构体
	socklen_t len = sizeof(clientaddr); // 整形len 获取clientaddr的长度
	const int clientfd = accept(sockfd, reinterpret_cast <sockaddr*>(&clientaddr), &len);
	//初始化一个客户端fd
	if (clientfd < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) return -1; // 可重试
		perror("accept failed");
		return -1;
	}
	set_event(clientfd,EPOLLIN, 1); //设置事件
	connlist[clientfd].fd = clientfd; //设置fd

	memset(connlist[clientfd].rbuffer, 0, conn_buff); //重置接收buffer内容
	connlist[clientfd].rlen = 0; //同理
	connlist[clientfd].read_callback = recv_cb; //设置回调函数

	memset(connlist[clientfd].wbuffer, 0, conn_buff); //重置发送buffer内容
	connlist[clientfd].wlen = 0; //同理
	connlist[clientfd].write_callback = send_cb; //设置回调函数

	return clientfd;
}

typedef conn_item connection_t; // 声明一个类型别名 , 其符合http的请求

void http_requset ( const connection_t* conn ) { // http请求
}

void http_response ( connection_t* conn ) { // 对客户端的相应 , 发送内容
	// 使用html文本直接输入

	conn->wlen = sprintf(conn->wbuffer, "HTTP/1.1 200 OK\r\n"
											 "Accept-Ranges: bytes\r\n"
											 "Content-Length: 106\r\n"
											 "Content-Type: text/html\r\n"
											 "Date: Sat, 06 Aug 2023 13:16:46 GMT\r\n\r\n"
											 "<html><head><title>0voice.king</title></head><body><h1>Hello! I just played for a month</h1></body></html>");

	//使用文件的方法传递html文件

	// const int filefd = open("/home/innno/code/0voice/2/2.1/2.1.1/2.1.1.3/index.html",O_RDONLY);
	// struct stat stat_buf {};
	// fstat(filefd, &stat_buf);
	// conn->wlen = sprintf(conn->wbuffer, "HTTP/1.1 200 OK\r\n"
	// 										 "Accept-Ranges: bytes\r\n"
	// 										 "Content-Length: %ld\r\n"
	// 										 "Content-Type: text/html\r\n"
	// 										 "Date: Sat, 06 Aug 2023 13:16:46 GMT\r\n\r\n", stat_buf.st_size);
	// const ssize_t count = read(filefd, conn->wbuffer + conn->wlen, conn_buff - conn->wlen);
	// conn->wlen += count;
}

void set_sendback ( const int connfd ) {  // 发送给客户端的内容 , 封装为函数 ,用于发送不同类型的内容
	//1.send内容为recv内容
	//memcpy(connlist[connfd].wbuffer, connlist[connfd].rbuffer, connlist[connfd].rlen);
	//connlist[connfd].wlen = connlist[connfd].rlen;
	//2.设置为http内容
	//http_request(&connlist[fd]);
	http_response(&connlist[connfd]);
}

//clientfd
//EPOLLIN
ssize_t recv_cb ( const int connfd ) { //接收事件的函数
	char* buffer = connlist[connfd].rbuffer; // 接收buffer的副本
	const ssize_t idx = connlist[connfd].rlen; // 同理
	const ssize_t count = recv(connfd, buffer + idx, conn_buff - idx, 0);
	// count 为 recv函数的返回值
	if (count == 0) {
		std::cout << "out\n";
		epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, nullptr);
		close(connfd);
		return -1;
	}
	connlist[connfd].rlen += count; //因为接受了新的内容 , 所以rlen的长度会变化
	//设置send内容
	set_sendback(connfd); // 设置 wbuffer内容
	//设置事件
	set_event(connfd,EPOLLOUT, 0); //设置事件为发送给客户端内容
	return count;
}

//EPOLLOUT
ssize_t send_cb ( const int connfd ) { // 发送事件函数
	conn_item &conn = connlist[connfd];
	ssize_t total_sent = 0;
	while (conn.wlen > 0) {
		const ssize_t count = send(connfd, conn.wbuffer + total_sent, conn.wlen, 0); //同理
		if (count < 0) {
			if (errno == EAGAIN) break;
			return -1;
		}
		total_sent += count;
		conn.wlen -= count;
	}
	if (conn.wlen == 0) {
		set_event(connfd, EPOLLIN, 0);
	}
	return total_sent;
}

class socket_cl {
public:
	socket_cl ( const int domain, const int type, const int protocol ) : sockfd_(socket(domain, type, protocol)) {
		cout << "construction finish \n";
		constexpr int opt = 1;
		setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	}

	[[nodiscard]] auto get_sockfd ( ) const -> int {
		return sockfd_;
	}

	auto bind_sockfd ( auto &serveraddr ) const -> int {
		if (-1 == bind(sockfd_, reinterpret_cast <const struct sockaddr*>(&serveraddr), sizeof(serveraddr))) {
			perror(" cant bind");
			return -1;
		}
		return 1;
	}

	auto listen_sockfd ( const int n ) const -> void {
		listen(sockfd_, n);
	}

private:
	int sockfd_ = 0;
};

int main ( ) {
	const auto socket_ob = socket_cl(AF_INET,SOCK_STREAM, 0); //设置socket

	sockaddr_in serveraddr {}; //sockaddr_in 结构体
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(2048); //开放在2048端口

	socket_ob.bind_sockfd(serveraddr);
	socket_ob.listen_sockfd(10);

	connlist[socket_ob.get_sockfd()].fd = socket_ob.get_sockfd(); //设置初始fd
	connlist[socket_ob.get_sockfd()].read_callback = accept_cb; //设置初始recv ,先accept一下

	epfd = epoll_create(1); // epoll 的特有函数

	set_event(socket_ob.get_sockfd(),EPOLLIN, 1); //设置事件为接收客户端发送的信息
	epoll_event events[1024] = {}; // epoll_event 的对象数组
	std::cout << "loop\n";

	while (true) { //持续循环
		const int nready = epoll_wait(epfd, events, 1024, -1); // epoll的特有函数
		for (auto i = 0 ; i < nready ; ++i) {
			const int connfd = events[i].data.fd; // 提取fd
			if (events[i].events & EPOLLIN) { //若event为EPOLLIN
				const ssize_t count = connlist[connfd].read_callback(connfd); //调用recv函数
				std::cout << "recv<--buffer: " << connlist[connfd].rbuffer << " count: " << count <<
						"\n";
			} else if (events[i].events & EPOLLOUT) { //若event为EPOLLOUT
				const ssize_t count = connlist[connfd].write_callback(connfd); //调用send函数
				std::cout << "send-->buffer: " << connlist[connfd].wbuffer << " count: " << count <<
						"\n";
			}
		}
	}
}