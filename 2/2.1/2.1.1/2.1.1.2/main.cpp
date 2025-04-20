#include <sys/socket.h>
#include <stdexcept>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/poll.h>

//c++ select:

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
//
//     // 移动构造函数  
//     socket(socket &&other) noexcept : fd(other.fd) {
//       other.fd = -1;
//     }
//
//     // 禁止拷贝  
//     socket(const socket &) = delete;
//
//     socket &operator=(const socket &) = delete;
//
//     ~socket() {
//       if (fd != -1)
//         {
//           close(fd); //断开连接  
//         }
//     }
//
//
//     void bind(const sockaddr_in &address) const {
//       if (::bind(fd, reinterpret_cast<const sockaddr *>(&address), sizeof(address)) == -1)
//         {
//           throw std::runtime_error(std::string("bind() failed: ") + strerror(errno));
//         }
//     }
//
//     void listen(const int backlog = 10) const {
//       if (::listen(fd, backlog) == -1)
//         {
//           throw std::runtime_error(std::string("listen() failed: ") + strerror(errno));
//         }
//       std::cout << "Server listening on port " << PORT << "\n";
//     }
//
//     int accept(sockaddr_in &address) const {
//       socklen_t len = sizeof(address);
//       const int client_fd = ::accept(fd, reinterpret_cast<sockaddr *>(&address), &len);
//       if (client_fd == -1)
//         {
//           throw std::runtime_error(std::string("accept() failed: ") + strerror(errno));
//         }
//       return client_fd;
//     }
//
//     static constexpr uint16_t PORT = 2048;
//     int fd = -1;
//   };
// }
//
// [[noreturn]] void select_io(const stq::socket &socket) {
//   fd_set rfds, rset;
//   FD_ZERO(&rfds);
//   FD_SET(socket.fd, &rfds);
//   int maxfd = socket.fd;
//   std::cout << "loop\n";
//   while (true)
//     {
//       rset = rfds;
//       select(maxfd + 1, &rset, nullptr, nullptr, nullptr);
//       if (FD_ISSET(socket.fd, &rset))
//         {
//           sockaddr_in clientaddr{};
//           socklen_t len = sizeof(clientaddr);
//           int clientfd = accept(socket.fd, reinterpret_cast<sockaddr *>(&clientaddr), &len);
//           std::cout << "sockfd: " << clientfd << "\n";
//           FD_SET(clientfd, &rfds);
//           maxfd = clientfd;
//         }
//       for (int i{socket.fd + 1}; i <= maxfd; ++i)
//         {
//           if (FD_ISSET(i, &rset))
//             {
//               char buffer[120]{};
//               const long int count = recv(i, buffer, 120, 0);
//               if (std::string(buffer, count) == "out" || count == 0)
//                 {
//                   std::cout << "out\n";
//                   FD_CLR(i, &rfds);
//                   close(i);
//                   break;
//                 }
//               send(i, buffer, count, 0);
//               std::cout << " " << "clientfd: " << i << " " << "count: " << count << "\n";
//             }
//         }
//     }
// }
//
// int main() {
//   try
//     {
//       const stq::socket socket(AF_INET, SOCK_STREAM, 0);
//
//       sockaddr_in server_addr{};
//       server_addr.sin_family = AF_INET;
//       server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//       server_addr.sin_port = htons(stq::socket::PORT);
//       socket.bind(server_addr);
//       socket.listen();
//
//       while (true)
//         {
//           select_io(socket); // 持续接受新连接  
//         }
//     } catch (const std::exception &e)
//     {
//       std::cerr << "Fatal error: " << e.what() << "\n";
//       return EXIT_FAILURE;
//     }
// }

//c select:

// int main() {
//   const int sockfd = socket(AF_INET,SOCK_STREAM, 0);
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
//   fd_set rfds, rset;
//   FD_ZERO(&rfds);
//   FD_SET(sockfd, &rfds);
//   int maxfd = sockfd;
//   std::cout << "loop\n";
//   while (true)
//     {
//       rset = rfds;
//       select(maxfd + 1, &rset, nullptr, nullptr, nullptr);
//       if (FD_ISSET(sockfd, &rset))
//         {
//           sockaddr_in clientaddr{};
//           socklen_t len = sizeof(clientaddr);
//           int clientfd = accept(sockfd, reinterpret_cast<sockaddr *>(&clientaddr), &len);
//           std::cout << "sockfd: " << clientfd << "\n";
//           FD_SET(clientfd, &rfds);
//           maxfd = clientfd;
//         }
//
//       for (int i = sockfd + 1; i <= maxfd; ++i)
//         {
//           if (FD_ISSET(i, &rset))
//             {
//               char buffer[120]{};
//               const long int count = recv(i, buffer, 120, 0);
//               if (std::string(buffer, count) == "out" || count == 0)
//                 {
//                   std::cout << "out\n";
//                   FD_CLR(i, &rfds);
//                   close(i);
//                   break;
//                 }
//               send(i, buffer, count, 0);
//
//               std::cout << " " << "clientfd: " << i << " " << "count: " << count << "\n";
//             }
//         }
//     }
// }

// c++ poll:

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
//
//     // 移动构造函数  
//     socket(socket &&other) noexcept : fd(other.fd) {
//       other.fd = -1;
//     }
//
//     // 禁止拷贝  
//     socket(const socket &) = delete;
//
//     socket &operator=(const socket &) = delete;
//
//     ~socket() {
//       if (fd != -1)
//         {
//           close(fd); //断开连接  
//         }
//     }
//
//
//     void bind(const sockaddr_in &address) const {
//       if (::bind(fd, reinterpret_cast<const sockaddr *>(&address), sizeof(address)) == -1)
//         {
//           throw std::runtime_error(std::string("bind() failed: ") + strerror(errno));
//         }
//     }
//
//     void listen(const int backlog = 10) const {
//       if (::listen(fd, backlog) == -1)
//         {
//           throw std::runtime_error(std::string("listen() failed: ") + strerror(errno));
//         }
//       std::cout << "Server listening on port " << PORT << "\n";
//     }
//
//     int accept(sockaddr_in &address) const {
//       socklen_t len = sizeof(address);
//       const int client_fd = ::accept(fd, reinterpret_cast<sockaddr *>(&address), &len);
//       if (client_fd == -1)
//         {
//           throw std::runtime_error(std::string("accept() failed: ") + strerror(errno));
//         }
//       return client_fd;
//     }
//
//     static constexpr uint16_t PORT = 2048;
//     int fd = -1;
//   };
// }
//
// [[noreturn]] void poll_io(const stq::socket &socket) {
//   pollfd fds[1024] = {};
//   fds[socket.fd].fd = socket.fd;
//   fds[socket.fd].events = POLLIN;
//   int maxfd = socket.fd;
//   std::cout << "loop\n";
//   while (true)
//     {
//       poll(fds,maxfd+1,-1);
//       if (fds[socket.fd].revents & POLLIN)
//         {
//           sockaddr_in clientaddr{};
//           socklen_t len = sizeof(clientaddr);
//           int clientfd = accept(socket.fd, reinterpret_cast<sockaddr *>(&clientaddr), &len);
//           
//           std::cout << "sockfd: " << clientfd << "\n";
//           
//           fds[clientfd].fd = clientfd;
//           fds[clientfd].events = POLLIN;
//           maxfd = clientfd;
//         }
//       for (int i{socket.fd + 1}; i <= maxfd; ++i)
//         {
//           if (fds[i].revents & POLLIN)
//             {
//               char buffer[120]{};
//               const long int count = recv(i, buffer, 120, 0);
//               if (std::string(buffer, count) == "out" || count == 0)
//                 {
//                   std::cout << "out\n";
//
//                   fds[i].fd = -1;
//                   fds[i].events = 0;
//                   
//                   close(i);
//                   break;
//                 }
//               send(i, buffer, count, 0);
//               std::cout << " " << "clientfd: " << i << " " << "count: " << count << "buffer: " << buffer << "\n";
//             }
//         }
//     }
// }
//
// int main() {
//   try
//     {
//       const stq::socket socket(AF_INET, SOCK_STREAM, 0);
//
//       sockaddr_in server_addr{};
//       server_addr.sin_family = AF_INET;
//       server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//       server_addr.sin_port = htons(stq::socket::PORT);
//       socket.bind(server_addr);
//       socket.listen();
//
//       while (true)
//         {
//           poll_io(socket); // 持续接受新连接  
//         }
//     } catch (const std::exception &e)
//     {
//       std::cerr << "Fatal error: " << e.what() << "\n";
//       return EXIT_FAILURE;
//     }
// }

// c poll:

// int main() {
//   const int sockfd = socket(AF_INET,SOCK_STREAM, 0);
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
//   pollfd fds[1024] = {};
//   fds[sockfd].fd = sockfd;
//   fds[sockfd].events = POLLIN;
//
//   int maxfd = sockfd;
//   std::cout << "loop\n";
//   while (true)
//     {
//       poll(fds, maxfd + 1, -1);
//       if (fds[sockfd].revents & POLLIN)
//         {
//           sockaddr_in clientaddr{};
//           socklen_t len = sizeof(clientaddr);
//           int clientfd = accept(sockfd, reinterpret_cast<sockaddr *>(&clientaddr), &len);
//           std::cout << "sockfd: " << clientfd << "\n";
//
//           fds[clientfd].fd = clientfd;
//           fds[clientfd].events = POLLIN;
//           maxfd = clientfd;
//         }
//
//       for (int i = sockfd + 1; i <= maxfd; ++i)
//         {
//           if (fds[i].revents & POLLIN)
//             {
//               char buffer[120]{};
//               const long int count = recv(i, buffer, 120, 0);
//               if (std::string(buffer, count) == "out" || count == 0)
//                 {
//                   std::cout << "out\n";
//
//                   fds[i].fd = -1;
//                   fds[i].events = 0;
//
//                   close(i);
//                   continue;
//                 }
//               send(i, buffer, count, 0);
//               std::cout << " " << "clientfd: " << i << " " << "count: " << count << "buffer: " << buffer << "\n";
//             }
//         }
//     }
// }

// c++ epoll

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
//
//     // 移动构造函数  
//     socket(socket &&other) noexcept : fd(other.fd) {
//       other.fd = -1;
//     }
//
//     // 禁止拷贝  
//     socket(const socket &) = delete;
//
//     socket &operator=(const socket &) = delete;
//
//     ~socket() {
//       if (fd != -1)
//         {
//           close(fd); //断开连接  
//         }
//     }
//
//
//     void bind(const sockaddr_in &address) const {
//       if (::bind(fd, reinterpret_cast<const sockaddr *>(&address), sizeof(address)) == -1)
//         {
//           throw std::runtime_error(std::string("bind() failed: ") + strerror(errno));
//         }
//     }
//
//     void listen(const int backlog = 10) const {
//       if (::listen(fd, backlog) == -1)
//         {
//           throw std::runtime_error(std::string("listen() failed: ") + strerror(errno));
//         }
//       std::cout << "Server listening on port " << PORT << "\n";
//     }
//
//     int accept(sockaddr_in &address) const {
//       socklen_t len = sizeof(address);
//       const int client_fd = ::accept(fd, reinterpret_cast<sockaddr *>(&address), &len);
//       if (client_fd == -1)
//         {
//           throw std::runtime_error(std::string("accept() failed: ") + strerror(errno));
//         }
//       return client_fd;
//     }
//
//     static constexpr uint16_t PORT = 2048;
//     int fd = -1;
//   };
// }
//
// [[noreturn]] void epoll_io(const stq::socket &socket) {
//   int epfd = epoll_create(1);
//
//   epoll_event ev{};
//   ev.events = EPOLLIN;
//   ev.data.fd = socket.fd;
//
//   epoll_ctl(epfd,EPOLL_CTL_ADD, socket.fd, &ev);
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
//           if (socket.fd == connfd)
//             {
//               sockaddr_in clientaddr{};
//               socklen_t len = sizeof(clientaddr);
//               int clientfd = accept(socket.fd, reinterpret_cast<sockaddr *>(&clientaddr), &len);
//               ev.events = EPOLLIN | EPOLLET;
//               ev.data.fd = clientfd;
//               epoll_ctl(epfd,EPOLL_CTL_ADD, clientfd, &ev);
//
//               std::cout << "clientfd: " << clientfd << "\n";
//             } else if (events[i].events & EPOLLIN)
//             {
//               char buffer[50] = {};
//               ssize_t count = recv(connfd, buffer, 50, 0);
//               send(i, buffer, count, 0);
//               if (std::string(buffer, count) == "out" || count == 0)
//                 {
//                   std::cout << "out\n";
//
//                   epoll_ctl(epfd,EPOLL_CTL_DEL, connfd, nullptr);
//
//                   close(i);
//                   continue;
//                 }
//               send(connfd, buffer, count, 0);
//               std::cout << " " << "clientfd: " << connfd << " " << "count: " << count << "buffer: " << buffer << "\n";
//             }
//         }
//     }
// }
//
// int main() {
//   try
//     {
//       const stq::socket socket(AF_INET, SOCK_STREAM, 0);
//
//       sockaddr_in server_addr{};
//       server_addr.sin_family = AF_INET;
//       server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//       server_addr.sin_port = htons(stq::socket::PORT);
//       socket.bind(server_addr);
//       socket.listen();
//
//       while (true)
//         {
//           epoll_io(socket); // 持续接受新连接  
//         }
//     } catch (const std::exception &e)
//     {
//       std::cerr << "Fatal error: " << e.what() << "\n";
//       return EXIT_FAILURE;
//     }
// }

// c epoll

// int main() {
//   const int sockfd = socket(AF_INET,SOCK_STREAM, 0);
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
//   epoll_ctl(epfd,EPOLL_CTL_ADD, sockfd, &ev);
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
//               ev.events = EPOLLIN | EPOLLET;
//               ev.data.fd = clientfd;
//               epoll_ctl(epfd,EPOLL_CTL_ADD, clientfd, &ev);
//
//               std::cout << "sockfd: " << clientfd << "\n";
//             } else if (events[i].events & EPOLLIN)
//             {
//               char buffer[50] = {};
//               ssize_t count = recv(connfd, buffer, 50, 0);
//               send(i, buffer, count, 0);
//               if (std::string(buffer, count) == "out" || count == 0)
//                 {
//                   std::cout << "out\n";
//
//                   epoll_ctl(epfd,EPOLL_CTL_DEL, connfd, nullptr);
//
//                   close(i);
//                   continue;
//                 }
//               send(connfd, buffer, count, 0);
//               std::cout << " " << "clientfd: " << connfd << " " << "count: " << count << "buffer: " << buffer << "\n";
//             }
//         }
//     }
// }
