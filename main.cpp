#include <iostream>
#include <string>
#include <sstream>
#include "socket.h"
#include <list>
#include <map>
#include <process.h>
#include <winsock2.h>
#include <windows.h>

using namespace std;
char RPG[4]  = {'R', 'P', 'G', '_'};
char RPGC[4] = {'R', 'P', 'G', 'C'};
char RPGD[4] = {'R', 'P', 'G', 'D'};

struct Client
{
  struct sockaddr_in addr;
  string name;
};

int main()
{
  char messageBuffer[1000];
  struct sockaddr_in fromAddr;
  list<Client> clients;
  int numClients = 0;
  WSADATA wsaData;
  Socket sock;
  struct sockaddr_in tmp;
  if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
  {
    cout << "WSAStartup() failed\n";
    exit(1);
  }
  if(sock.bindSocket(12345) < 0)
  {
    cout << "Bind failed, Error code: " << WSAGetLastError();
    exit(1);
  }

  cout << "Server Active" << endl;
  cout << sizeof(fromAddr);

  while (true)
  {
    sock.recvMessage(&fromAddr, messageBuffer);
    //cout << "Got: " << messageBuffer << endl;
    if(!memcmp(messageBuffer, RPG, 4) || !memcmp(messageBuffer, RPGC, 4))
    {
      if(!memcmp(messageBuffer, RPGC, 4))
      {
        Client tmpC;
        stringstream ss;
        memcpy(&tmpC, &fromAddr, sizeof(fromAddr));
        ss << messageBuffer + 4;
        tmpC.name = ss.str();
        clients.push_back(tmpC);
        cout << "Added: " << inet_ntoa(fromAddr.sin_addr) << ", " << messageBuffer + 4 << " to list\n";
        ss.str(string());
        char addr[16];
        memcpy(&addr, &fromAddr, sizeof(fromAddr));
        ss << addr << tmpC.name;
        list<Client>::iterator it;
        for(it = clients.begin(); it != clients.end(); it++)
        {
          memset(&tmp, 0, sizeof(tmp));
          tmp.sin_family = it->addr.sin_family;
          tmp.sin_addr = it->addr.sin_addr;
          tmp.sin_port = it->addr.sin_port;
          if(sock.sendMessage(&tmp, ss.str()) < 0)
          {
            cout << "Send error: " << WSAGetLastError();
          }
        }
      }
      else
      {
        list<Client>::iterator it;
        stringstream ss;
        ss.str(string());
        ss << "CIM";
        bool isFound = false;
        for(it = clients.begin(); it != clients.end(); it++)
        {
          if(!memcmp(&(it->addr), &fromAddr, sizeof(fromAddr)))
          {
            ss << it->name << ": ";
            isFound = true;
          }
        }
        if(!isFound)
          ss << inet_ntoa(fromAddr.sin_addr) << ":" << ntohs(fromAddr.sin_port) << ": ";
        ss << messageBuffer + 3;
        //cout << "Sent: " << ss.str() << endl;
        for(it = clients.begin(); it != clients.end(); it++)
        {
          memset(&tmp, 0, sizeof(tmp));
          tmp.sin_family = it->addr.sin_family;
          tmp.sin_addr = it->addr.sin_addr;
          tmp.sin_port = it->addr.sin_port;
          if(sock.sendMessage(&tmp, ss.str()) < 0)
          {
            cout << "Send error: " << WSAGetLastError();
          }
        }
      }
    }
    memset(messageBuffer, '\0', 1000);
  }

  sock.close();
  return 0;
}
