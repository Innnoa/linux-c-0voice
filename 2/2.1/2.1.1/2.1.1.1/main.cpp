#include <sys/socket.h>
#include <stdexcept>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <pthread.h>

// c++:

namespace stq {
  class socket {
  public:
    explicit socket(const int domain, const int type, const int protocol = 0) {
      fd = ::socket(domain, type, protocol);
      if (fd == -1)
        {
          throw std::runtime_error(std::string("socket() failed: ") + strerror(errno));
        }
    }

    // 移动构造函数
    socket(socket &&other) noexcept : fd(other.fd) {
      other.fd = -1;
    }

    // 禁止拷贝
    socket(const socket &) = delete;

    socket &operator=(const socket &) = delete;

    ~socket() {
      if (fd != -1)
        {
          close(fd); //断开连接
        }
    }


    void bind(const sockaddr_in &address) const {
      if (::bind(fd, reinterpret_cast<const sockaddr *>(&address), sizeof(address)) == -1)
        {
          throw std::runtime_error(std::string("bind() failed: ") + strerror(errno));
        }
    }

    void listen(int backlog = 10) const {
      if (::listen(fd, backlog) == -1)
        {
          throw std::runtime_error(std::string("listen() failed: ") + strerror(errno));
        }
      std::cout << "Server listening on port " << PORT << "\n";
    }

    int accept(sockaddr_in &address) const {
      socklen_t len = sizeof(address);
      const int client_fd = ::accept(fd, reinterpret_cast<sockaddr *>(&address), &len);
      if (client_fd == -1)
        {
          throw std::runtime_error(std::string("accept() failed: ") + strerror(errno));
        }
      return client_fd;
    }

    static constexpr uint16_t PORT = 2048;

  private:
    int fd = -1;
  };
}

static void *client_thread(void *arg) {
  const int client_fd = *static_cast<int *>(arg);
  delete static_cast<int *>(arg); // 释放动态内存

  try
    {
      while (true)
        {
          char buffer[120];
          const ssize_t count = recv(client_fd, buffer, sizeof(buffer), 0);
          if (count <= 0)
            {
              if (count == 0)
                {
                  std::cout << "Client " << client_fd << " disconnected\n";
                } else
                {
                  perror("recv error");
                }
              break;
            }

          // 处理退出指令
          if (std::string(buffer, count) == "out")
            {
              std::cout << "Client " << client_fd << " requested to disconnect\n";
              break;
            }

          // 回显数据
          if (send(client_fd, buffer, count, 0) == -1)
            {
              throw std::runtime_error("send failed");
            }

          std::cout << "Client " << client_fd
              << " | Received: " << count << " bytes" << "str:" << std::string(buffer, count) << "\n";
        }
    } catch (const std::exception &e)
    {
      std::cerr << "Client " << client_fd << " error: " << e.what() << "\n";
    }

  close(client_fd);
  return nullptr;
}

void thread_run(const stq::socket &socket) {
  try
    {
      sockaddr_in clientaddr{};
      const int client_fd = socket.accept(clientaddr); // 阻塞但立即返回新fd

      // 动态分配fd副本
      const auto client_fd_ptr = new int(client_fd);

      pthread_t thid;
      if (pthread_create(&thid, nullptr, client_thread, client_fd_ptr) != 0)
        {
          std::cerr << "Failed to create thread\n";
          delete client_fd_ptr;
          close(client_fd);
          return;
        }
      pthread_detach(thid); // 分离线程
    } catch (const std::exception &e)
    {
      std::cerr << "Accept error: " << e.what() << "\n";
    }
}


int main() {
  try
    {
      const stq::socket socket(AF_INET, SOCK_STREAM, 0);

      sockaddr_in server_addr{};
      server_addr.sin_family = AF_INET;
      server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
      server_addr.sin_port = htons(stq::socket::PORT);

      socket.bind(server_addr);
      socket.listen();

      while (true)
        {
          thread_run(socket); // 持续接受新连接
        }
    } catch (const std::exception &e)
    {
      std::cerr << "Fatal error: " << e.what() << "\n";
      return EXIT_FAILURE;
    }
}

// c:

// void *client_thread(void *arg) {
//   const int clientfd = *static_cast<int *>(arg);
//   while (true)
//     {
//       char buffer[120]{};
//       //getchar();//这里的getchar设置了缓冲区,使接收到的字符一直存放到缓冲区中,使其造成了一种
//       //错觉认为recv会反复调用,直到停止接收其实,其只调用了一次,不过这一次会把缓冲区中所有的字符全部接收
//       const long int count = recv(clientfd, buffer, 120, 0);
//       if (std::string(buffer, count) == "out" || count == 0)
//         {
//           std::cout << "out\n";
//           break;
//         }
//       send(clientfd, buffer, count, 0);
//
//       std::cout << " " << "clientfd: " << clientfd << " " << "count: " << count << "\n";
//     }
//   //getchar();
//   close(clientfd);
//   return nullptr;
// }
//
// int main() {
//   int sockfd = socket(AF_INET,SOCK_STREAM, 0);
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
// #if false
//   //单线程
//   sockaddr_in clientaddr{};
//   socklen_t len = sizeof(clientaddr);
//   int clientfd = accept(sockfd, reinterpret_cast<sockaddr *>(&clientaddr), &len);
//
//   std::cout << "accept\n";
//
// #if 0
//   //只接收一次:
//   char buffer[120]{};
//   //getchar();//这里的getchar设置了缓冲区,使接收到的字符一直存放到缓冲区中,使其造成了一种错觉,认为recv会反复调用,直到停止接收
//   //其实,其只调用了一次,不过这一次会把缓冲区中所有的字符全部接收
//   long int count = recv(clientfd, buffer, 120, 0);
//
//   send(clientfd, buffer, count, 0);
//
//   std::cout << "sockfd: " << sockfd << " " << "clientfd: " << clientfd << " " << "count: " << count << "\n";
//
// #else
//   //接收多次:
//   while (true)
//     {
//       char buffer[120]{};
//       //getchar();//这里的getchar设置了缓冲区,使接收到的字符一直存放到缓冲区中,使其造成了一种错觉,认为recv会反复调用,直到停止接收
//       //其实,其只调用了一次,不过这一次会把缓冲区中所有的字符全部接收
//       long int count = recv(clientfd, buffer, 120, 0);
//       if (std::string(buffer,count)=="out")
//         {
//           std::cout<<"out\n";
//           break;
//         }
//       send(clientfd, buffer, count, 0);
//
//       std::cout << "sockfd: " << sockfd << " " << "clientfd: " << clientfd << " " << "count: " << count << "\n";
//     }
//   getchar();
//   close(clientfd);
//
// #endif
//   return 0;
// #else
//   //多线程,多次调用
//   while (true)
//     {
//       sockaddr_in clientaddr{};
//       socklen_t len = sizeof(clientaddr);
//       int clientfd = accept(sockfd, reinterpret_cast<sockaddr *>(&clientaddr), &len);
//       pthread_t thid;
//       std::cout << "clientfd: " << clientfd << "\n";
//       pthread_create(&thid, nullptr, client_thread, &clientfd);
//     }
//
// #endif
// }
