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
  regex comment_regex {R"(^\s*(#.*)?$)"}; //comment
  regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
  regex key_regex {R"(^\s*([^=]+?)\s*$)"};
  regex key_eq_regex {R"(^\s*([^=]+?)\s*=\s*$)"};
  regex eq_value_regex {R"(^\s*=\s*(.*?)\s*$)"};
  regex eq_regex {R"(\s*=\s*$)"};
  str_str_map::iterator itor;
  //str_str_pair::iterator newPair;
  smatch result;

  // '#'
  if (regex_search (line, result, comment_regex)) {
    cout << "Comment" << endl;
    //continue;
  }
  // "key=value"
  if (regex_search (line, result, key_value_regex)) {
    itor = map.find(result[1]);
    str_str_pair::iterator newPair(result[1],result[2]);
    map.insert(newPair);
    cout << result[1] << " = " << result[2] << endl;
  }else if(regex_search (line, result, key_regex){
    itor = map.find(result[1]);
    if(itor = map.begin()){
      cout << result[1] << ": key not found" << endl;
    }
  })
    cout << result[1] << "=" << result[2] << endl;
  //need "=" , "key =", "key", "= value"
}

int main (int argc, char** argv) {
  // sys_info::execname (argv[0]);
  // scan_options (argc, argv);

  str_str_map test;

  // for (char** argp = &argv[optind]; argp != &argv[argc]; ++argp) {
  //   str_str_pair pair (*argp, to_string<int> (argp - argv));
  //   cout << "Before insert: " << pair << endl;
  //   test.insert (pair);
  // }

  // for (str_str_map::iterator itor = test.begin();
  //     itor != test.end(); ++itor) {
  //   cout << "During iteration: " << *itor << endl;
  // }

  // str_str_map::iterator itor = test.begin();
  // test.erase (itor);

  // cout << "EXIT_SUCCESS" << endl;
  // return EXIT_SUCCESS;
 for (;;) {
    string line;
    getline (cin, line);
    if (cin.eof()) break;
    cout << endl << "input: \"" << line << "\"" << endl;
    parse(line,test);

 }
   return 0;
}

