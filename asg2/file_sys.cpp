// $Id: file_sys.cpp,v 1.7 2019-07-09 14:05:44-07 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

#include "debug.h"
#include "file_sys.h"
#include "commands.h"

int inode::next_inode_nr {1};

struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}

void inode_state::setRoot(inode_ptr that){
   root = that;
}

inode_ptr inode_state::getRoot(){return root;}

void inode_state::setCwd(inode_ptr that){
   cwd = that;
}

inode_ptr inode_state::getCwd(){return cwd;}

void inode_state::set_prompt(string that){
   prompt_ = that;
}

void inode::setContents(base_file_ptr that){
   contents = that;
}

base_file_ptr inode::getContents(){
   return contents;
}

inode_state::inode_state() {
   root = make_shared<inode>(file_type::DIRECTORY_TYPE);
   cwd = root;
   shared_ptr <directory> baseD = dynamic_pointer_cast<directory> (root->getContents());
   baseD->addent(".", root);
   baseD->addent("..", root);
   //DEBUGF ('i', "root = " << root << ", cwd = " << cwd << ", prompt =
   //    \"" << prompt() << "\"");
}

inode_state::~inode_state(){
  cwd = nullptr;
  shared_ptr <directory> baseD = dynamic_pointer_cast<directory> (root->getContents());
  baseD->clearDir();
  root = nullptr;
}
const string& inode_state::prompt() const {return prompt_;}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
   }
//DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

bool inode::getType()const{
   return isDir;
   }

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}


file_error::file_error (const string& what):
            runtime_error (what) {
}

void base_file::setFilename(const string& name){
  filename = name;
}

string base_file::getFname(){
  return filename;
}

// const wordvec& base_file::readfile() const {throw file_error ("is a "
//    + error_file_type());
// }

// void base_file::writefile (const wordvec&) {throw file_error ("is a "
//    + error_file_type());
// }

// void base_file::remove (const string&) {throw file_error ("is a " +
//    error_file_type());
// }

// inode_ptr base_file::mkdir (const string&) {throw file_error ("is a "
//    + error_file_type());
// }

// inode_ptr base_file::mkfile (const string&) {throw file_error ("is a
//    " + error_file_type());
// }


// size_t plain_file::size() const {size_t size {0}; DEBUGF ('i', "size
//    = " << size); return size;
// }
size_t plain_file::getsize(){
  return size;
}

void plain_file::setSize(size_t nsize){
  size = nsize;
}

const wordvec& plain_file::readF() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writeF (const wordvec& words) {
   DEBUGF ('i', words);
   data = words;
}

inode_ptr plain_file::mkfile (const string&) {
   throw file_error ("is a plain file");
}

void plain_file::remove (const string&) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkdir (const string&) {
   throw file_error ("is a plain file");
}

size_t directory::getsize(){
   //DEBUGF ('i', "size = " << size);
   return dirents.size();
}

int directory::numF(){
   return dirents.size() - 2;
}

void directory::setSize(size_t nsize){
  size = nsize;
}

const wordvec& directory::readF() const{
   throw file_error ("is a directory");
}

void directory::writeF(const wordvec&){
   throw file_error ("is a directory");
}
void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
   dirents.erase(filename);
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   if(getF(dirname) == inode_ptr()){
      dirents.at(dirname) = inode_ptr();
   }
   return nullptr;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   dirents.at(filename) = inode_ptr();
   return nullptr;
}

inode_ptr directory::getF (const string& filename) {
  try{
    return dirents.at(filename);
  }
  catch(const std::out_of_range& ex){
    return inode_ptr();
  }
}

void directory::addent (const string& filename, const inode_ptr& file){
   dirents[filename] = file;
}

bool directory::fexists(const string& filename) {
   try {
      dirents.at(filename);
      return true;
   }
   catch(std::out_of_range) {
      return false;
   }
}

void directory::createF (const string& filename, const wordvec& newdata){
   inode_ptr newF = make_shared<inode>(file_type::PLAIN_TYPE);
   size_t size = 0;
   unsigned int itor = 0;
   while(itor < newdata.size()){
      size += newdata[itor].length();
      if(itor < newdata.size() -1){++size;}
      ++itor;
   }
   addent(filename,newF);
   shared_ptr <plain_file> currfile = dynamic_pointer_cast<plain_file> (newF->getContents());
   currfile->setSize(size);
   currfile->setFilename(getFname() + "/" + filename);
   currfile->writeF(newdata);
}

void directory::createDir(const string& filename){
   inode_ptr newF =  make_shared<inode>(file_type::DIRECTORY_TYPE);
   addent(filename, newF);
   shared_ptr <directory> currfile = dynamic_pointer_cast<directory> (newF->getContents());
   currfile->setFilename(getFname() + "/" + filename);
   currfile->dirents["."] = newF;
   currfile->dirents[".."] = dirents["."];
}

string directory::printEnt(){
  string str1 = "";
  std::map<const string,inode_ptr>::iterator itor= dirents.begin();
  while(itor != dirents.end()){
    if(itor->second->getType() == true){
      shared_ptr <directory> item = dynamic_pointer_cast<directory>(itor->second->getContents());
      str1.append(to_string(itor->second->get_inode_nr()));
      str1.append(" ");
      str1.append(to_string(item->getsize()));
      str1.append(" ");
      str1.append(itor->first);
      str1.append("\n");
    }
    else{
      shared_ptr <directory> item = dynamic_pointer_cast<directory>(itor->second->getContents());
      str1.append(to_string(itor->second->get_inode_nr()));
      str1.append(" ");
      str1.append(to_string(item->getsize()));
      str1.append(" ");
      str1.append(itor->first);
      str1.append("\n");
    }
    ++itor;
  }
  return str1;
}

string directory::printEntR(string content, vector<int> *visited){
  string outstr = content;
  bool found = false;
  if(getFname() == ""){
    outstr += "/:\n";
  }else{
    outstr = outstr + getFname() + ":\n";
  }
  outstr = outstr + printEnt();
  std::map<const string, inode_ptr>::iterator itor = dirents.begin();
  visited->push_back(itor->second->get_inode_nr());
  itor++;
  visited->push_back(itor->second->get_inode_nr());
  itor++;
  while(itor != dirents.end()){
    unsigned int inside = 0;
    while(inside < visited->size()){
      if(visited->at(inside) == itor->second->get_inode_nr()){
        found = true;
        break;
      }
      ++inside;
    }
    if(!found){
      visited->push_back(itor->second->get_inode_nr());
      if((itor->first != ".") && (itor->first != "..") && (itor->second->getType())){
      shared_ptr <directory> direct = dynamic_pointer_cast<directory>(itor->second->getContents());
      outstr += direct->printEntR(outstr, visited);
      }
    }
    ++itor;
    found = false;
  }
  return outstr;
}

void directory::clearDir(){
for(std::map<const string, inode_ptr>::iterator fileItor = dirents.begin(); fileItor!=dirents.end();++fileItor){
    if((fileItor->first != ".") && (fileItor->first != "..") && (fileItor->second->getContents())){
   shared_ptr<directory>dir= dynamic_pointer_cast<directory> (fileItor->second->getContents());
      dir->clearDir();
    }
  }
  dirents.clear();
}
