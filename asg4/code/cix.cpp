// $Id: cix.cpp,v 1.9 2019-04-05 15:04:28-07 - - $
//Matthew Klein and Andrew Oceguera
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstring>
#include <unordered_map>

using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream outlog (cout);
struct cix_exit: public exception {};

unordered_map<string,cix_command> command_map {
  {"exit", cix_command::EXIT},
  {"help", cix_command::HELP},
  {"ls"  , cix_command::LS  },
  {"get"  , cix_command::GET},
  {"put"  , cix_command::PUT  },
  {"rm"  , cix_command::RM  },
};

// static const char help[] = R"||(
// exit         - Exit the program.  Equivalent to EOF.
// get filename - Copy remote file to local host.
// help         - Print help summary.
// ls           - List names of files on remote server.
// put filename - Copy local file to remote host.
// rm filename  - Remove file from remote server.
// )||";

void cix_help() {
  static const vector<string> help = {
    "exit         - Exit the program.  Equivalent to EOF.",
    "get filename - Copy remote file to local host.",
    "help         - Print help summary.",
    "ls           - List names of files on remote server.",
    "put filename - Copy local file to remote host.",
    "rm filename  - Remove file from remote server.",
  };
  for(const auto& line: help) cout << line << endl;
}

void cix_ls (client_socket& server) {
  cix_header header;
  header.command = cix_command::LS;
  outlog << "sending header " << header << endl;
  send_packet (server, &header, sizeof header);
  recv_packet (server, &header, sizeof header);
  outlog << "received header " << header << endl;
  if (header.command != cix_command::LSOUT) {
    outlog << "sent LS, server did not return LSOUT" << endl;
    outlog << "server returned " << header << endl;
  }else {
    char buffer[header.nbytes + 1];
    recv_packet (server, buffer, header.nbytes);
    outlog << "received " << header.nbytes << " bytes" << endl;
    buffer[header.nbytes] = '\0';
    cout << buffer;
  }
}

void cix_get(client_socket& server, string filename){
  cix_header header;
  header.command = cix_command::GET;
  filename.copy(header.filename, filename.size());
  outlog << "sending header " << header << endl;
  send_packet(server, &header, sizeof header);
  recv_packet(server, &header, sizeof header);
  outlog << "received header " << header << endl;

  if (header.command != cix_command::FILEOUT) {
    outlog << "Error: No such file: " << header.filename << endl;
    return;
  }else{
    char buffer[header.nbytes + 1];
    recv_packet(server, buffer, header.nbytes);
    outlog << "received " << header.nbytes << " bytes" << endl;
    buffer[header.nbytes] = '\0';
    ofstream outfile (filename, ofstream::out);
    outfile.write(buffer, sizeof buffer);
    outfile.close();
  }

}

void cix_rm(client_socket& server, string filename){
  cix_header header;
  header.command = cix_command::RM;
  filename.copy (header.filename, filename.size());
  header.nbytes = 0;

  outlog << "sending header " << header << endl;
  send_packet (server, &header, sizeof header);
  recv_packet (server, &header, sizeof header);
  outlog << "received header " << header << endl;

  if (header.command != cix_command::ACK) {
    outlog << "Error: No such file: " << header.filename << endl;
    return;
  }
  else{ 
    outlog << header.filename << ": file succesfully removed." 
    << endl;
  }
}

void cix_put (client_socket& server, string& filename) {
  cix_header header;
  header.command = cix_command::PUT;
  filename.copy (header.filename, filename.size());

  string getFile;
  char buffer;

  ifstream file (filename);

  if (file.fail()) {
    outlog << "Error: No such file: " << header.filename << endl;
  return;
  }else{
    while(file.good()){
      file.get(buffer);
      getFile.push_back(buffer);
    }
  }

  
  file.close();
  header.nbytes = getFile.size();

  outlog << "sending header " << header << endl;
  send_packet (server, &header, sizeof header);
  send_packet (server, getFile.c_str(), getFile.size());
  outlog << "sent " << getFile.size() << " bytes" << endl;
  recv_packet (server, &header, sizeof header);
  outlog << "received header " << header << endl;

  if (header.command != cix_command::ACK) {
    outlog << "sent CIX_PUT, server did not return CIX_ACK" << endl;
    outlog << "server returned " << header << endl;
    outlog << "put: " << filename << ": " << strerror(header.nbytes) 
    << endl;
  }else{
    outlog << "put: " << filename << ": OK" << endl;
  }
}


void usage() {
  cerr << "Usage: " << outlog.execname() << " [host] [port]" << endl;
  throw cix_exit();
}

int main (int argc, char** argv) {
  outlog.execname (basename (argv[0]));
  outlog << "starting" << endl;
  vector<string> args (&argv[1], &argv[argc]);
  string host;
  in_port_t port;

  if (args.size() > 2) usage();
  if (args.size() == 1) {
    host = get_cix_server_host(args, 1);
    port = get_cix_server_port(args, 0);
  }else{
    host = get_cix_server_host (args, 0);
    port = get_cix_server_port (args, 1);
  }
  
  outlog << to_string (hostinfo()) << endl;
  
  try {
    outlog << "connecting to " << host << " port " << port << endl;
    client_socket server (host, port);
    outlog << "connected to " << to_string (server) << endl;
    for (;;) {
      string line;
      getline (cin, line);
      if (cin.eof()) throw cix_exit();
      outlog << "command: " << line << endl;
      size_t fst_pos_space = line.find(" ");
      string cmd_str = line.substr(0, fst_pos_space);
      string args1 = (fst_pos_space != string::npos?
      line.substr(fst_pos_space+1,
      line.size()): "");
      const auto& itor = command_map.find (cmd_str);
      cix_command cmd = itor == command_map.end()
      ? cix_command::ERROR : itor->second;
      switch (cmd) {
        case cix_command::EXIT:
          throw cix_exit();
          break;
        case cix_command::HELP:
          cix_help();
          break;
        case cix_command::LS:
          cix_ls (server);
          break;
        case cix_command::GET:
          cix_get (server, args1);
          break;
        case cix_command::RM:
          cix_rm (server, args1);
          break;
        case cix_command::PUT:
          cix_put (server, args1);
          break;
        default:
          outlog << cmd_str << ": invalid command" << endl;
          break;
      }
    }
  }catch (socket_error& error) {
    outlog << error.what() << endl;
  }catch (cix_exit& error) {
    outlog << "caught cix_exit" << endl;
  }
  outlog << "finishing" << endl;
  return 0;
}
