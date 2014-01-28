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
char RPGS[4] = {'R', 'P', 'G', 'S'};

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
        //char addr[16];
        list<Client>::iterator it;
        for(it = clients.begin(); it != clients.end(); it++)
        {
          ss.str(string());
          //memcpy(&addr, &(it->addr), sizeof(struct sockaddr_in));
          ss << "RPGS" << ":" << inet_ntoa(it->addr.sin_addr) << ":" << ntohs(it->addr.sin_port) << ":" << it->name;
          cout << ss.str() << endl;
          if(sock.sendMessage(&fromAddr, ss.str()) < 0)
          {
            cout << "Send error: " << WSAGetLastError();
          }
        }


        cout << "Added: " << inet_ntoa(fromAddr.sin_addr) << ", " << messageBuffer + 4 << " to list\n";
        ss.str(string());
        ss << "RPGS" << ":" << inet_ntoa(tmpC.addr.sin_addr) << ":" << ntohs(tmpC.addr.sin_port) << tmpC.name;

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

        clients.push_back(tmpC);
      }
    }
    memset(messageBuffer, '\0', 1000);
  }

  sock.close();
  return 0;
}
