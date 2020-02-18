// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <regex>
#include <fstream>
#include <unistd.h>
#include <assert>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

void scan_options (int argc, char** argv) {
  opterr = 0;
  for (;;) {
    int option = getopt (argc, argv, "@:");
    if (option == EOF) break;
    switch (option) {
      case '@':
        debugflags::setflags (optarg);
        break;
      default:
        complain() << "-" << char (optopt) << ": invalid option" << endl;
        break;
    }
  }
}

void parse(string line, str_str_map& map){
  regex comment_regex {R"(^\s*(#.*)?$)"};
  regex whtspce_regex = {R"(^\s*$)"};
  regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
  regex key_regex {R"(^\s*([^=]+?)\s*$)"};
  regex key_eq_regex {R"(^\s*([^=]+?)\s*=\s*$)"};
  regex eq_value_regex {R"(^\s*=\s*(.*?)\s*$)"};
  regex eq_regex {R"(\s*=\s*$)"};
  str_str_map::iterator itor;
  smatch result;

  // '#'
  if (regex_search (line, result, comment_regex) ||
  regex_search(line, result, whtspce_regex)){
    return;
  }
  // "key=value"
  if (regex_search(line, result, key_value_regex)) {
    itor = map.find(result[1]);
    str_str_pair::iterator newPair(result[1],result[2]);
    map.insert(newPair);
    cout << result[1] << "=" << result[2] << endl;
  }else if(regex_search(line, result, key_regex)){
    itor = map.find(result[1]);
    if(itor->next == map.end()){
      cout << result[1] << ": key not found" << endl;
    }else{
      cout << result[1] << "=" << result[2] << endl;
    }
  }else if(regex_search(line, result, key_eq_regex)){
    for(auto itor = map.begin();itor != map.end();++itor){
      if(result[1] == itor->first){
        map.erase(itor);
        break;
      }
    }
  }else if(regex_search(line, result, eq_value_regex)){
    for(auto itor = map.begin();itor != map.end();++itor){
      if(result[2] == itor->second){
        cout << itor->first << " = " << itor->second
        << endl;
      }
    }
  }else{
    cerr << "Parsing error" << endl;
  }
}

int main (int argc, char** argv) {
  sys_info::execname (argv[0]);
  scan_options (argc, argv);
  str_str_map test;
  string line = "";
  string pname = string(argv[0]);
  int count{0};

  for(int itor = 1; itor < argc; ++itor){
    if(argv[itor] == std::string("-")){
      while(getline(cin,line)){
      if(cin.eof()){break;}
      ++count;
      cout << argv[itor] << ": " << count << ": " 
      << line << endl;
      parse(line,test);
    }
    }else{
      ifstream fstream(argv[itor]);
      if(fstream.fail()){
        cerr << pname << ": " << argv[itor]
        << ": No such file or directory" << endl;
      }else{
        while(getline(fstream,line)){
          ++count;
          cout << argv[itor] << ": " << count << ": " 
          << line << endl;
          parse(line,test);
      }
    }
  }

  // for (char** argp = &argv[optind]; argp != &argv[argc]; ++argp) {
  //   str_str_pair pair (*argp, to_string<int> (argp - argv));
  //   cout << "Before insert: " << pair << endl;
  //   test.insert (pair);
  // }

  return EXIT_SUCCESS;
}

