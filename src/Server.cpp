#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <thread>
#include <vector>

using namespace std;

vector<string> processRESPCommand(string &buffer);
string processArray(vector<string> &command);

void handleRequest(int clientSocket)
{

  string readBuffer;

  while (true)
  {
    char buffer[1024];

    memset(buffer, 0, sizeof(buffer));

    int recv_bytes = recv(clientSocket, buffer, 1024, 0);

    if (recv_bytes == 0)
    {
      std::cout << "Client disconnected\n";
      close(clientSocket);
      return;
    }
  // for (int i = 0; i < 40; i++) {
  //   cout<<"buffer["<<i<<"]: "<<buffer[i]<<"\n";
  // }
    readBuffer=buffer;

    cout<<"Data received: "<<readBuffer<<endl;
    Cout<<"Length of data: "<<readBuffer.size()<<endl;

   vector<string> command = processRESPCommand(readBuffer);
    cout<<command[0]<<"\n";
    const char *message = processArray(command).c_str();
    send(clientSocket, message, strlen(message), 0);
  }
}

int main(int argc, char **argv)
{
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // You can use print statements as follows for debugging, they'll be visible when running tests.
  std::cout << "Logs from your program will appear here!\n";

  // test1();

  // Uncomment this block to pass the first stage

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0)
  {
    std::cerr << "Failed to create server socket\n";
    return 1;
  }

  // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
  {
    std::cerr << "setsockopt failed\n";
    return 1;
  }
  //
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(6379);
  //
  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
  {
    std::cerr << "Failed to bind to port 6379\n";
    return 1;
  }
  //
  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0)
  {
    std::cerr << "listen failed\n";
    return 1;
  }

  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);

  std::cout << "Waiting for a client to connect...\n";

  while (true)
  {

    int clientSocket = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
    std::cout << "Client connected\n";
    thread t(handleRequest, clientSocket);

    t.detach();
  }

  close(server_fd);

  return 0;
}

vector<string> processRESPCommand(string &buffer)
{
string local_buffer=buffer;
  cout<<local_buffer<<"\n";
  int position = 0;
  vector<string> command;
  vector<string> noValidCommand={"NVC"};

  int sizeOfbuffer = buffer.size();

  if (sizeOfbuffer < 1)
  {
    return noValidCommand;
  }

  if (buffer[position] != '*')
  {
    return {"nvc1"};
  }

  position++;
  int commandLength = stoi(&buffer[position]);

  position++;


  if (buffer[position] != '\r' && buffer[position+1] != '\n')
  {

    return {"nvc2"};
  }

  position += 2;

  for (int i = 0; i < commandLength; i++)
  {

    if (buffer[position] != '$')
    {
      return {"nvc3"};
    }

    position++;
    int tokenLength = stoi(&buffer[position]);

    position++;

    if (buffer[position] != '\r' && buffer[position+1] != '\n')
    {
      return {"nvc4"};
    }

    position += 2;

    command.push_back(buffer.substr(position, tokenLength));
    position += tokenLength;


    if (buffer[position] != '\r' && buffer[position+1] != '\n')
    {
      return command;
    }

    position += 2;


  }

  cout<<"Command1: "<<command[0]<<endl;
  return command;
 
}

string processArray(vector<string> &command) {

    if (command[0]=="PING") {
      return "$4\r\nPONG\r\n";
    }

  if (command[0]=="ECHO") {

    return "$"+to_string(command[1].size())+"\r\n"+command[1]+"\r\n";;
  }

}
