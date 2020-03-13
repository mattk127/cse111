// $Id: interp.cpp,v 1.3 2019-03-19 16:18:22-07 - - $

#include <memory>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

unordered_map<string,interpreter::interpreterfn>
interpreter::interp_map {
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
   {"moveBy" , &interpreter::do_moveBy },
   {"border" , &interpreter::do_border },
};

unordered_map<string,interpreter::factoryfn>
interpreter::factory_map {
   {"text"     , &interpreter::make_text     },
   {"ellipse"  , &interpreter::make_ellipse  },
   {"circle"   , &interpreter::make_circle   },
   {"polygon"  , &interpreter::make_polygon  },
   {"rectangle", &interpreter::make_rectangle},
   {"square"   , &interpreter::make_square   },
   {"triangle" , &interpreter::make_triangle   },
   {"equilateral" , &interpreter::make_equilateral},
   {"diamond"   , &interpreter::make_diamond },
};

interpreter::shape_map interpreter::objmap;

interpreter::~interpreter() {
  for (const auto& itor: objmap) {
    cout << "objmap[" << itor.first << "] = " 
    << *itor.second << endl;
  }
}

void interpreter::interpret (const parameters& params) {
  DEBUGF ('i', params);
  param begin = params.cbegin();
  string command = *begin;
  auto itor = interp_map.find (command);
  if (itor == interp_map.end()) throw runtime_error ("syntax error");
  interpreterfn func = itor->second;
  func (++begin, params.cend());
}

void interpreter::do_define (param begin, param end) {
  DEBUGF ('f', range (begin, end));
  string name = *begin;
  objmap.emplace (name, make_shape (++begin, end));
}


void interpreter::do_draw (param begin, param end) {
  DEBUGF ('f', range (begin, end));
  if (end - begin != 4) throw runtime_error ("syntax error");
  string name = begin[1];
  shape_map::const_iterator itor = objmap.find (name);
  if (itor == objmap.end()) {
    throw runtime_error (name + ": no such shape");
  }
  vertex where {from_string<GLfloat> (begin[2]), 
    from_string<GLfloat> (begin[3])};
  rgbcolor color {begin[0]};
  window::push_back (object (itor->second, where, color));
}
void interpreter::do_moveBy(param begin, param end){
  int length = 0;
  auto tempBegin = begin;
  while(tempBegin != end){
    tempBegin++;
    length++;
  }
  if(length != 1){
    throw runtime_error ("do_moveBy: wrong number of args");
  }
  window::moveBy = stof(*begin);
}

void interpreter::do_border(param begin, param end){
  int length1 = 0;
  auto tempBegin1 = begin;
  while(tempBegin1 != end){
    tempBegin1++;
    length1++;
  }
  if(length1 != 2){
    throw runtime_error ("do_border: wrong number of args");
  }
  rgbcolor color {begin[0]};
  GLfloat width = from_string<GLfloat> (begin[1]);
  window::borderColor = color;
  window::borderWidth = width;
}

shape_ptr interpreter::make_shape (param begin, param end) {
  DEBUGF ('f', range (begin, end));
  string type = begin[0];
  begin++;
  auto itor = factory_map.find(type);
  if (itor == factory_map.end()) {
    throw runtime_error (type + ": no such shape");
  }
  factoryfn func = itor->second;
  return func (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
  DEBUGF ('f', range (begin, end));
  string plain_text = "";
  auto itor = fontcode.find(begin[0]);
  begin++;
  if(itor == fontcode.end()){
    throw runtime_error (begin[0] + ": no such font");
  }
  while(begin != end){
    plain_text += begin[0];
    plain_text += " ";
    ++begin;
  }
  return make_shared<text> (itor->second, plain_text);
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<ellipse> (GLfloat(stof(begin[0])),
    GLfloat(stof(begin[1])));
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<circle> (GLfloat(stof(begin[0])));
}

shape_ptr interpreter::make_polygon (param begin, param end) {
  DEBUGF ('f', range (begin, end));
  vertex_list verts;
  auto tempBegin2 = begin;
  int length2;
  while(tempBegin2 != end){
    ++tempBegin2;
    ++length2;
  }
  if((length2 % 2) != 0){
    throw runtime_error("Wrong number of args");
  }
  while(begin != end){
    verts.push_back({GLfloat(stof(begin[0])),
      GLfloat(stof(begin[1]))});
      ++begin;
      ++begin;
  }
  return make_shared<polygon> (verts);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
  DEBUGF ('f', range (begin, end));
  if(begin == end){
    throw runtime_error("Wrong number of args");
  }
  return make_shared<rectangle> (GLfloat(stof(begin[0])), 
    GLfloat(stof(begin[1])));
}

shape_ptr interpreter::make_square (param begin, param end) {
  DEBUGF ('f', range (begin, end));
  if(begin == end){
    throw runtime_error("Wrong number of args");
  }
  return make_shared<square> (GLfloat(stof(begin[0])));
}

shape_ptr interpreter::make_triangle (param begin, param end){
  vertex_list verts;
  auto tempBegin3 = begin;
  int length3;
  while(tempBegin3 != end){
    ++tempBegin3;
    ++length3;
  }
  if((length3 % 2) != 0){
    throw runtime_error("Wrong number of args");
  }
  while(begin != end){
    verts.push_back({GLfloat(stof(begin[0])), 
    GLfloat(stof(begin[1]))});
    ++begin;
    ++begin;
  }
  return make_shared<triangle> (verts);
}

shape_ptr interpreter::make_equilateral (param begin, param end){
  DEBUGF('f',range(begin,end));
  return make_shared<equilateral> (GLfloat(stof(begin[0])));
}

shape_ptr interpreter::make_diamond(param begin, param end){
  DEBUGF('f',range(begin,end));
  if(begin == end){
    throw runtime_error("wrong number of args");
  }
  return make_shared<diamond> (GLfloat(stof(begin[0])),
  GLfloat(stof(begin[1])));
}
