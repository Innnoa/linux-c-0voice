/**
 * ClassName: ${NAME}
 * package: ${PACKAGE_NAME}
 * Description:
 * @Author: innno
 * @Create: 2024/2/27 - 21:54
 * @Version: v1.0
 */
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
using namespace std;

// c: io

// constexpr int conn_buff = 1024;
// constexpr int conn_num = 1024;
//
// struct conn_item {
//   int fd;
//   char buffer[conn_buff];
//   ssize_t idx;
// } connlist[conn_num];
//
// int main() {
//   const int sockfd = socket(AF_INET, SOCK_STREAM, 0);
//   sockaddr_in serveraddr{};
//   serveraddr.sin_family = AF_INET;
//   serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
//   serveraddr.sin_port = htons(2048);
//   if (-1 == bind(sockfd, reinterpret_cast<const sockaddr *>(&serveraddr), sizeof(sockaddr)))
//     {
//       perror("bind");
//       return -1;
//     }
//   listen(sockfd, 10);
//
//   int epfd = epoll_create(1);
//
//   epoll_event ev{};
//   ev.events = EPOLLIN;
//   ev.data.fd = sockfd;
//
//   epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
//
//   epoll_event events[1024] = {};
//
//   std::cout << "loop\n";
//   while (true)
//     {
//       int nready = epoll_wait(epfd, events, 1024, -1);
//       for (int i = 0; i < nready; ++i)
//         {
//           int connfd = events[i].data.fd;
//           if (sockfd == connfd)
//             {
//               sockaddr_in clientaddr{};
//               socklen_t len = sizeof(clientaddr);
//               int clientfd = accept(sockfd, reinterpret_cast<sockaddr *>(&clientaddr), &len);
//               ev.events = EPOLLIN | EPOLLET; //ET
//               //ev.events = EPOLLIN; //LT
//               ev.data.fd = clientfd;
//               epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);
//
//               connlist[clientfd].fd = clientfd;
//               memset(connlist[clientfd].buffer, 0, conn_buff);
//               connlist[clientfd].idx = 0;
//
//               std::cout << "sockfd: " << clientfd << "\n";
//             } else if (events[i].events & EPOLLIN)
//             {
//               char *buffer = connlist[connfd].buffer;
//               ssize_t idx = connlist[connfd].idx;
//               const ssize_t count = recv(connfd, buffer + idx, conn_buff - idx, 0);
//               //send(i, buffer, count, 0);
//               if (count == 0)
//                 {
//                   std::cout << "out\n";
//                   epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, nullptr);
//                   close(i);
//                   continue;
//                 }
//               connlist[connfd].idx += count;
//               send(connfd, buffer + idx, count, 0);
//               std::cout << " " << "clientfd: " << connfd << " " << "count: " << count << "buffer: " << buffer + idx <<
//                   "\n";
//             }
//         }
//     }
// }

// c++: io

// constexpr int conn_buff = 128;
// constexpr int conn_num = 1024;
//
// struct conn_item {
//   int fd;
//   char buffer[conn_buff];
//   ssize_t idx;
// } connlist[conn_num];
//
// namespace stq {
//   class socket {
//   public:
//     explicit socket(const int domain, const int type, const int protocol = 0) {
//       fd = ::socket(domain, type, protocol);
//       if (fd == -1)
//         {
//           throw std::runtime_error(std::string("socket() failed: ") + strerror(errno));
//         }
//     }
//     // 移动构造函数
// socket(socket &&other) noexcept : fd(other.fd) {
//       other.fd = -1;
//     }
//     // 禁止拷贝
// socket(const socket &) = delete;
//     socket &operator=(const socket &) = delete;
//     ~socket() {
//       if (fd != -1)
//         {
//           close(fd); //断开连接
// }
//     }
//     void bind(const sockaddr_in &address) const {
//       if (::bind(fd, reinterpret_cast<const sockaddr *>(&address), sizeof(address)) == -1)
//         {
//           throw std::runtime_error(std::string("bind() failed: ") + strerror(errno));
//         }
//     }
//     void listen(const int backlog = 10) const {
//       if (::listen(fd, backlog) == -1)
//         {
//           throw std::runtime_error(std::string("listen() failed: ") + strerror(errno));
//         }
//       std::cout << "Server listening on port " << PORT << "\n";
//     }
//     int accept(sockaddr_in &address) const {
//       socklen_t len = sizeof(address);
//       const int client_fd = ::accept(fd, reinterpret_cast<sockaddr *>(&address), &len);
//       if (client_fd == -1)
//         {
//           throw std::runtime_error(std::string("accept() failed: ") + strerror(errno));
//         }
//       return client_fd;
//     }
//     static constexpr uint16_t PORT = 2048;
//     int fd = -1;
//   };
// }
// [[noreturn]] void epoll_io(const stq::socket &socket) {
//   int epfd = epoll_create(1);
//   epoll_event ev{};
//   ev.events = EPOLLIN;
//   ev.data.fd = socket.fd;
//   epoll_ctl(epfd,EPOLL_CTL_ADD, socket.fd, &ev);
//   epoll_event events[1024] = {};
//   std::cout << "loop\n";
//   while (true)
//     {
//       int nready = epoll_wait(epfd, events, 1024, -1);
//       for (int i = 0; i < nready; ++i)
//         {
//           int connfd = events[i].data.fd;
//           if (socket.fd == connfd)
//             {
//               sockaddr_in clientaddr{};
//               socklen_t len = sizeof(clientaddr);
//               int clientfd = accept(socket.fd, reinterpret_cast<sockaddr *>(&clientaddr), &len);
//               //ev.events = EPOLLIN | EPOLLET; //ET
//               ev.events = EPOLLIN; //LT
//               ev.data.fd = clientfd;
//               epoll_ctl(epfd,EPOLL_CTL_ADD, clientfd, &ev);
//               connlist[clientfd].fd = clientfd;
//               memset(connlist[clientfd].buffer, 0, conn_buff);
//               connlist[clientfd].idx = 0;
//               std::cout << "sockfd: " << clientfd << "\n";
//             } else if (events[i].events & EPOLLIN)
//             {
//               char *buffer = connlist[connfd].buffer;
//               ssize_t idx = connlist[connfd].idx;
//               const ssize_t count = recv(connfd, buffer + idx, conn_buff - idx, 0);
//               //send(i, buffer, count, 0);
//               if (count == 0)
//                 {
//                   std::cout << "out\n";
//                   epoll_ctl(epfd,EPOLL_CTL_DEL, connfd, nullptr);
//                   close(i);
//                   continue;
//                 }
//               connlist[connfd].idx += count;
//               send(connfd, buffer + idx, count, 0);
//               std::cout << " " << "clientfd: " << connfd << " " << "count: " << count << "buffer: " << buffer + idx <<
//                   "\n";
//             }
//         }
//     }
// }
//
// int main() {
//   try
//     {
//       const stq::socket socket(AF_INET, SOCK_STREAM, 0);
//       sockaddr_in server_addr{};
//       server_addr.sin_family = AF_INET;
//       server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//       server_addr.sin_port = htons(stq::socket::PORT);
//       socket.bind(server_addr);
//       socket.listen();
//       while (true)
//         {
//           epoll_io(socket); // 持续接受新连接
// }
//     } catch (const std::exception &e)
//     {
//       std::cerr << "Fatal error: " << e.what() << "\n";
//       return EXIT_FAILURE;
//     }
// }

// c: event

constexpr auto conn_buff = 1024;
constexpr auto conn_num = 1024;

typedef ssize_t (*rcallback) ( int fd );

struct conn_item {
	int fd;
	char rbuffer[conn_buff];
	ssize_t rlen;
	char wbuffer[conn_buff];
	ssize_t wlen;

	char resource[conn_buff];

	union {
		rcallback accept_callback;
		rcallback recv_callback;
	} reve_t;

	rcallback send_callback;
} connlist[conn_num];

int epfd;

ssize_t accept_cb ( int sockfd );

ssize_t recv_cb ( int connfd );

ssize_t send_cb ( int connfd );


void set_event ( const int fd, const int event, const int flag ) {
	if (flag) {
		epoll_event ev {};
		ev.events = event;
		ev.data.fd = fd;
		epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
	} else {
		epoll_event ev {};
		ev.events = event;
		ev.data.fd = fd;
		epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
	}
}

//listenfd
//EPOLLIN
ssize_t accept_cb ( const int sockfd ) {
	sockaddr_in clientaddr {};
	socklen_t len = sizeof(clientaddr);
	const int clientfd = accept(sockfd, reinterpret_cast <sockaddr*>(&clientaddr), &len);
	if (clientfd < 0) {
		return -1;
	}
	set_event(clientfd,EPOLLIN, 1);
	connlist[clientfd].fd = clientfd;
	memset(connlist[clientfd].rbuffer, 0, conn_buff);
	connlist[clientfd].rlen = 0;
	connlist[clientfd].reve_t.recv_callback = recv_cb;
	memset(connlist[clientfd].wbuffer, 0, conn_buff);
	connlist[clientfd].wlen = 0;
	connlist[clientfd].send_callback = send_cb;
	return clientfd;
}

typedef conn_item connection_t;

void http_requset ( const connection_t* conn ) {
}

void http_response ( connection_t* conn ) {
	// 使用html文本直接输入

	// conn->wlen = sprintf(conn->wbuffer, "HTTP/1.1 200 OK\r\n"
	// 										 "Accept-Ranges: bytes\r\n"
	// 										 "Content-Length: 78\r\n"
	// 										 "Content-Type: text/html\r\n"
	// 										 "Date: Sat, 06 Aug 2023 13:16:46 GMT\r\n\r\n"
	// 										 "<html><head><title>0voice.king</title></head><body><h1>King</h1></body></html>");

	//使用文件的方法传递html文件
	const int filefd = open("/home/innno/code/0voice/2/2.1/2.1.1/2.1.1.3/index.html",O_RDONLY);
	struct stat stat_buf {};
	fstat(filefd, &stat_buf);
	conn->wlen = sprintf(conn->wbuffer, "HTTP/1.1 200 OK\r\n"
											 "Accept-Ranges: bytes\r\n"
											 "Content-Length: %ld\r\n"
											 "Content-Type: text/html\r\n"
											 "Date: Sat, 06 Aug 2023 13:16:46 GMT\r\n\r\n", stat_buf.st_size);
	const ssize_t count = read(filefd, conn->wbuffer + conn->wlen, conn_buff - conn->wlen);
	conn->wlen += count;
}

void set_sendback ( const int connfd ) {
	//1.send内容为recv内容
	//memcpy(connlist[connfd].wbuffer, connlist[connfd].rbuffer, connlist[connfd].rlen);
	//connlist[connfd].wlen = connlist[connfd].rlen;
	//2.设置为http内容
	//http_request(&connlist[fd]);
	http_response(&connlist[connfd]);
}

//clientfd
//EPOLLIN
ssize_t recv_cb ( const int connfd ) {
	char* buffer = connlist[connfd].rbuffer;
	const ssize_t idx = connlist[connfd].rlen;
	const ssize_t count = recv(connfd, buffer + idx, conn_buff - idx, 0);
	if (count == 0) {
		std::cout << "out\n";
		epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, nullptr);
		close(connfd);
		return -1;
	}
	connlist[connfd].rlen += count;
	//设置send内容
	set_sendback(connfd);
	//设置事件
	set_event(connfd,EPOLLOUT, 0);
	return count;
}

//EPOLLOUT
ssize_t send_cb ( const int connfd ) {
	const char* buffer = connlist[connfd].wbuffer;
	const ssize_t idx = connlist[connfd].wlen;
	const ssize_t count = send(connfd, buffer, idx, 0);
	//恢复事件
	set_event(connfd,EPOLLIN, 0);
	return count;
}

int main ( ) {
	const int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in serveraddr {};
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(2048);
	if (-1 == bind(sockfd, reinterpret_cast <const sockaddr*>(&serveraddr), sizeof(sockaddr))) {
		perror("bind");
		return -1;
	}
	listen(sockfd, 10);

	connlist[sockfd].fd = sockfd;
	connlist[sockfd].reve_t.accept_callback = accept_cb;

	epfd = epoll_create(1);
	set_event(sockfd,EPOLLIN, 1);
	epoll_event events[1024] = {};
	std::cout << "loop\n";
	while (true) {
		const int nready = epoll_wait(epfd, events, 1024, -1);
		for (auto i = 0 ; i < nready ; ++i) {
			const int connfd = events[i].data.fd;
			if (events[i].events & EPOLLIN) {
				const ssize_t count = connlist[connfd].reve_t.recv_callback(connfd);
				std::cout << "recv<--buffer: " << connlist[connfd].rbuffer << " count: " << count <<
						"\n";
			} else if (events[i].events & EPOLLOUT) {
				const ssize_t count = connlist[connfd].send_callback(connfd);
				std::cout << "send-->buffer: " << connlist[connfd].wbuffer << " count: " << count <<
						"\n";
			}
		}
	}
}
