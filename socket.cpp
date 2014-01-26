
#include "socket.h"

Socket::Socket()
{
  isBound = false;
}

Socket::~Socket()
{
  closesocket(sock_);
}

int Socket::bindSocket(unsigned short port)
{
  port_ = port;
  if ((sock_ = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
  {
    exit(1);
  }
  memset(&sockAddr_, 0, sizeof(sockAddr_));
  sockAddr_.sin_family = AF_INET;
  sockAddr_.sin_addr.s_addr = htonl(INADDR_ANY);
  sockAddr_.sin_port = htons(port_);

  return bind(sock_, (struct sockaddr *) &sockAddr_, sizeof(sockAddr_));
}

int Socket::recvMessage(struct sockaddr_in* fromAddr, char* message)
{
  int size = sizeof(*fromAddr);
  return recvfrom(sock_, message, 1000, 0, (struct sockaddr*)fromAddr, &size);
}

int Socket::sendMessage(struct sockaddr_in* to, std::string message)
{
  return sendto(sock_, message.c_str(), strlen(message.c_str()), 0, (struct sockaddr*)to, sizeof(struct sockaddr));
}

void Socket::close()
{
  closesocket(sock_);
}

