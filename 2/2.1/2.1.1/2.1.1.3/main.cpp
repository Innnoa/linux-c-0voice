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
#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
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

typedef ssize_t (*rcallback)(int fd);

struct conn_item
	{
		int fd;
		char buffer[conn_buff];
		ssize_t idx;
		rcallback accept_callback;
		rcallback recv_callback;
		rcallback send_callback;
	} connlist[conn_num];

int epfd;

ssize_t accept_cb(int sockfd);

ssize_t recv_cb(int connfd);

ssize_t send_cb(int connfd);

void set_event(const int fd, const int event, const int flag)
	{
		if (flag)
			{
				epoll_event ev{};
				ev.events = event;
				ev.data.fd = fd;
				epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
			} else
			{
				epoll_event ev{};
				ev.events = event;
				ev.data.fd = fd;
				epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
			}
	}

//listenfd
//EPOLLIN
ssize_t accept_cb(const int sockfd)
	{
		sockaddr_in clientaddr{};
		socklen_t len = sizeof(clientaddr);
		const int clientfd = accept(sockfd, reinterpret_cast<sockaddr *>(&clientaddr), &len);
		if (clientfd < 0)
			{
				return -1;
			}
		set_event(clientfd,EPOLLIN, 1);
		connlist[clientfd].fd = clientfd;
		memset(connlist[clientfd].buffer, 0, conn_buff);
		connlist[clientfd].idx = 0;
		connlist[clientfd].recv_callback = recv_cb;
		connlist[clientfd].send_callback = send_cb;
		return clientfd;
	}

//clientfd
//EPOLLIN
ssize_t recv_cb(const int connfd)
	{
		char *buffer = connlist[connfd].buffer;
		const ssize_t idx = connlist[connfd].idx;
		const ssize_t count = recv(connfd, buffer + idx, conn_buff - idx, 0);
		if (count == 0)
			{
				std::cout << "out\n";
				epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, nullptr);
				close(connfd);
				return -1;
			}
		connlist[connfd].idx += count;
		//设置事件
		set_event(connfd,EPOLLOUT, 0);
		return count;
	}

//EPOLLOUT
ssize_t send_cb(const int connfd)
	{
		const char *buffer = connlist[connfd].buffer;
		const ssize_t idx = connlist[connfd].idx;
		const ssize_t count = send(connfd, buffer, idx, 0);
		//恢复事件
		set_event(connfd,EPOLLIN, 0);
		return count;
	}

int main()
	{
		const int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		sockaddr_in serveraddr{};
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
		serveraddr.sin_port = htons(2048);
		if (-1 == bind(sockfd, reinterpret_cast<const sockaddr *>(&serveraddr), sizeof(sockaddr)))
			{
				perror("bind");
				return -1;
			}
		listen(sockfd, 10);

		connlist[sockfd].fd = sockfd;
		connlist[sockfd].accept_callback = accept_cb;

		epfd = epoll_create(1);
		set_event(sockfd,EPOLLIN, 1);
		epoll_event events[1024] = {};
		std::cout << "loop\n";
		while (true)
			{
				const int nready = epoll_wait(epfd, events, 1024, -1);
				for (auto i = 0; i < nready; ++i)
					{
						if (const int connfd = events[i].data.fd; sockfd == connfd)
							{
								const ssize_t clientfd = connlist[sockfd].accept_callback(sockfd);
								std::cout << "clientfd: " << clientfd << "\n";
							} else if (events[i].events & EPOLLIN)
							{
								const ssize_t count = connlist[connfd].recv_callback(connfd);
								std::cout << "recv<--buffer: " << connlist[connfd].buffer << " count: " << count <<
										"\n";
							} else if (events[i].events & EPOLLOUT)
							{
								const ssize_t count = connlist[connfd].send_callback(connfd);
								std::cout << "send-->buffer: " << connlist[connfd].buffer << " count: " << count <<
										"\n";
							}
					}
			}
	}
