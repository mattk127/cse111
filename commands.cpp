// $Id: commands.cpp,v 1.18 2019-10-08 13:55:31-07 - - $
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstddef>

#include "commands.h"
#include "debug.h"

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"rmr"   , fn_rmr   },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type> So:
   // iterator.first is key_type (string) So: iterator.second is
   // mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int status = exec::status();
   cout << exec::execname() << ": exit(" << status << ")" << endl;
   return status;
}

void fn_cat (inode_state& state, const wordvec& words){
  DEBUGF ('c', state);
  DEBUGF ('c', words);
  if(words.size() == 1){
    throw command_error ("cat: No file specified");
  }else if(words.size() > 1){
    shared_ptr <directory> contentct = dynamic_pointer_cast <directory> (state.getCwd()->getContents());
    unsigned int itor = 1;
    while(itor < words.size()){
      inode_ptr filecontent = contentct->getF(words[itor]);
      if(filecontent == inode_ptr()){
        cout << "cat: " << words[itor] << " no such file exists.\n";
      }else{
        shared_ptr <plain_file> file = dynamic_pointer_cast <plain_file> (filecontent->getContents());
        cout << file->readF() << endl;
      }
      ++itor;
    }
  }else{
    cout << "cat: need a file to cat.\n";
  }

}

void navigate(bool& successful, shared_ptr <directory> current_dir, const wordvec& dirnames){
  inode_ptr next_file = dynamic_pointer_cast <inode> (current_dir);
  successful = true;
  for(unsigned int itor = 0; itor < dirnames.size(); itor++){
    next_file=dynamic_pointer_cast<inode>(current_dir->getF(dirnames[itor]));
      if(next_file == inode_ptr()){
          successful= false;
          break;
      }
      if(next_file->getType()) { // file specified is a directory
      current_dir = dynamic_pointer_cast <directory>(current_dir->getF(dirnames[itor])->getContents());
      }
      else{
          successful = false;
          break;
      }
  }
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() == 1){
     state.setCwd(state.getRoot());
   }else if(words.size() == 2){
     shared_ptr <directory> currDir = dynamic_pointer_cast <directory> (state.getCwd()->getContents());
     inode_ptr nextF = dynamic_pointer_cast <inode> (currDir);
     wordvec dirName = split (words[1], "/");
     bool success = true;
     unsigned int itorCD = 0;
     while(itorCD < dirName.size()){
       nextF = dynamic_pointer_cast <inode> (currDir->getF(dirName[itorCD]));
       if(nextF == inode_ptr()){
         success = false;
         break;
       }
       if(nextF->getType()){
         currDir = dynamic_pointer_cast<directory> (currDir->getF(dirName[itorCD])->getContents());
       }else{
         success = false;
         break;
       }
       itorCD++;
     }
     if(success){state.setCwd(nextF);}
     else{
       cout << "cd: cannot navigate to dir...\n";
     }
     if(words[1] == "/"){
       state.setCwd(state.getRoot());
     }
   }
   else{
     cout << "cd: invalid argument given\n";
   }
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   shared_ptr <directory> allFiles = dynamic_pointer_cast <directory> (state.getCwd()->getContents());
   allFiles->clearDir();
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words){
  if(words.size() <= 2){
    shared_ptr <directory> contentl = dynamic_pointer_cast <directory> (state.getCwd()->getContents());
    cout << contentl->printEnt();
  }
  else{
    throw command_error("ls: invalid number of args");
  }
   DEBUGF ('c', state); 
   DEBUGF ('c', words);
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   shared_ptr <directory> contentls = dynamic_pointer_cast<directory> (state.getCwd()->getContents());
   for(unsigned int itor = 1; itor < words.size();itor++){
     wordvec splitted = split(words[itor], "/");
     wordvec::iterator vecItor = splitted.begin();
     contentls = dynamic_pointer_cast<directory> (state.getCwd()->getContents());
     unsigned int sizeD = 0;
     inode_ptr currfile;
     if(words[itor] == "/"){
       shared_ptr <directory> contentls = dynamic_pointer_cast <directory> (state.getRoot()->getContents());
       vector <int> *visited = new vector<int> (0);
       string outStr = contentls->printEntR("", visited);
       cout << outStr.substr(outStr.rfind("/:"),outStr.length()-1);
       delete(visited);
     }else{
       while(sizeD < splitted.size()){
         if(contentls->fexists(*vecItor)){
           currfile = contentls->getF(*vecItor);
         }else{
           throw command_error("fn_lsr: directory does not exist " + *vecItor);
         }
         if(!currfile->getType()){
           throw command_error("fn_lsr: expected a directory");
         }
         else{
           vecItor++;
           sizeD++;
           contentls = dynamic_pointer_cast<directory> (currfile->getContents());
         }
       }
     }
     sizeD = 0;
   }
   if(words.size() == 0){
     throw command_error("fs_lsr: invalid number of args");
   }
}

void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() <= 1){
     throw command_error("make: no filename specified");
   }else{
     shared_ptr <directory> content = dynamic_pointer_cast <directory> (state.getCwd()->getContents());
     wordvec splitted = split(words[1], "/");
     wordvec::iterator vecItor = splitted.begin();
     unsigned int size = 0;
     if(splitted.size() < 2){
       if(content->getF(words[1]) == inode_ptr()){
         content->createF(words[1],wordvec(words.begin() + 2, words.end()));
       }else{
         throw command_error("make: file already exists");
       }
     }else{
       while(size < (splitted.size()-1)){
         inode_ptr currfile;
         if(content->fexists(*vecItor)){
           currfile = content->getF(*vecItor);
         }else{
           throw command_error("make: directory does not exist");
         }
         if(currfile->getType()){
           vecItor++;
           size++;
           content = dynamic_pointer_cast<directory> (currfile->getContents());
         }else{
           throw command_error("make: expected a directory");
         }
       }
       if(content->getF(*vecItor) == inode_ptr()){
         content->createF(*vecItor,wordvec(words.begin()+2, words.end()));
       }else{
         cout << "make " << words[1] << " file already exists.\n";
       }
     }
   }
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
  if((words.size() == 1)){
    throw command_error("mkdir: invalid number of args");
  }
  wordvec splitted = split(words[1], "/");
  wordvec::iterator vecItor = splitted.begin();
  shared_ptr <directory> contentdir = dynamic_pointer_cast <directory> (state.getCwd()->getContents());
     
  unsigned int sizeL = 0;
   if(words.size() == 2){
     if(splitted.size()<2){
       if(contentdir->fexists(words[1])){
         throw command_error("mkdir: " + words[1] + " already exists.");
       }else{
         contentdir->createDir(words[1]);
       }
     }else{
       while(sizeL < (splitted.size()-1)){
         inode_ptr currfile;
         if(contentdir->fexists(*vecItor)){
           currfile = contentdir->getF(*vecItor);
         }else{
           throw command_error("make: directory" + *vecItor + " does not exist");
         }
         if(currfile->getType()){
           vecItor++;
           sizeL++;
           contentdir= dynamic_pointer_cast<directory> (currfile->getContents());
         }else{
           throw command_error("make: expected a directory");
         }
       }
       if(!contentdir->fexists(*vecItor)){
         contentdir->createDir(*vecItor);
       }else{
         throw command_error("make: file already exists.");
       }
     }
   }else{
     throw command_error("mkdir: invalid number of args.");
   }
}

void fn_prompt (inode_state& state, const wordvec& words){
   string promptStr = "";
   if(words.size() >= 2){
     unsigned int itorP = 0;
     while(itorP < words.size()){
       promptStr += words[itorP];
       if(itorP < words.size() -1){
         promptStr += " ";
       }
       itorP++;
     }
     state.set_prompt(promptStr);
   }else{
     cout << "prompt: wrong number of args passed\n";
   }
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}
string getName(shared_ptr<directory> curr){
  string outstr = curr->getFname();
  if(outstr.size() == 0){
    outstr = "/";
  }
  return outstr;
}

void fn_pwd (inode_state& state, const wordvec& words){
  DEBUGF ('c', state);
   DEBUGF ('c', words);
  if(words.size()==1){ 
    shared_ptr <directory> curr = dynamic_pointer_cast <directory> (state.getCwd()->getContents());
    string outStr = getName(curr);
    cout << outStr << endl;
  }
    else throw command_error("pwd: invalid number of args");
}


void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   shared_ptr <directory> curr = dynamic_pointer_cast <directory> (state.getCwd()->getContents());
    string outStr = getName(curr);
    if(words.size() == 2){
      auto currelement = curr->getF(words[1]);
      auto currDir = dynamic_pointer_cast<directory> (currelement->getContents());
      if(currDir->numF()==0 || !currelement->getType()){
        if(currelement->getType()){
          currDir->clearDir();
        }
        curr->remove(words[1]);
      }else{
        cout << "rm: " << words[1] << " is not an empty directory\n";
      }
    }
}

void fn_rmr (inode_state& state, const wordvec& words){
  DEBUGF ('c', state);
  DEBUGF ('c', words);
  shared_ptr<directory> contentrmr = dynamic_pointer_cast<directory>(state.getCwd()->getContents());
  for(unsigned int itor = 1; itor < words.size(); itor++){
    wordvec splitted = split(words[itor], "/");
    wordvec::iterator vecItor = splitted.begin();
    contentrmr=dynamic_pointer_cast<directory>(state.getCwd()->getContents());
    unsigned int size = 0;
    inode_ptr file;
  if(splitted.size() <= 2 ){
    if(contentrmr->fexists(*splitted.begin())) {
      shared_ptr<directory> oldDir = contentrmr;
      contentrmr = dynamic_pointer_cast<directory>(contentrmr->getF(words[itor])->getContents());
      contentrmr->clearDir();
      oldDir->remove(*splitted.begin());
    }else {
        cout << "rmr: " + words[itor] + " does not exists.\n";
    }
  }else{
    while(size < splitted.size()-1){
      if(contentrmr->fexists(*vecItor)){
        file = contentrmr->getF(*vecItor);
      }else{
        throw command_error("rmr: directory " + *vecItor+ " does not exist.");
      }
      if(file->getType()){
        vecItor++;
        size++;
        contentrmr = dynamic_pointer_cast<directory> (file->getContents());
      }else{
        throw command_error("rmr: expected a directory and not a file");
      }
    }
    if(contentrmr->fexists(*vecItor)) {
      shared_ptr<directory> oldDir = contentrmr;
      contentrmr = dynamic_pointer_cast<directory>(contentrmr->getF(words[itor])->getContents());
        contentrmr->clearDir();
        oldDir->remove(*vecItor);
      }
      else {
        cout << "rmr: " + *vecItor + " does not exist.\n";
      }
    }
  }
}
