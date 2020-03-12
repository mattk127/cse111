// $Id: shape.cpp,v 1.2 2019-02-28 15:24:20-08 - - $
#define _USE_MATH_DEFINES
#include <typeinfo>
#include <unordered_map>
#include <math.h>
using namespace std;

#include "shape.h"
#include "util.h"
#include "graphics.h"

static unordered_map<void*,string> fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

static unordered_map<string,void*> fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font_, const string& textdata_):
      glut_bitmap_font(glut_bitmap_font_), textdata(textdata_) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}


polygon::polygon (const vertex_list& vertices_): vertices(vertices_) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height):
            polygon({{width,height},{width, 0}, {0,0}, {0,height}}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

triangle::triangle (const vertex_list& vertices): polygon(vertices) {
  DEBUGF ('c', this);
}
diamond::diamond (const GLfloat width, const GLfloat height): 
    polygon({{0,0}, {width/2, height/2}, 
                    {width, 0}, {width/2, 0 - height/2}}) {
    DEBUGF ('c', this);
}
equilateral::equilateral(const GLfloat width): 
    triangle({{0, 0}, {width/2 ,width},
            {width,0}}){
  DEBUGF ('c', this);
 }
void text::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   auto temp = reinterpret_cast<const GLubyte*>(textdata.c_str());
   glColor3ubv(color.ubvec);
   glRasterPos2f(center.xpos, center.ypos);
   glutBitmapString(glut_bitmap_font, temp);
}
void text::showNum(const vertex& center,
 const rgbcolor& color, const size_t& number) const {
   if(number <= 9) {
      auto text = reinterpret_cast<const GLubyte*> (to_string(number).c_str());
      glColor3ubv (color.ubvec);
      glRasterPos2f (center.xpos, center.ypos);
      glutBitmapString (GLUT_BITMAP_8_BY_13, text);
    }
}
void text::draw_border(const vertex& center,
 const rgbcolor& color, const GLfloat& width, const size_t& number) const{
   auto temp = reinterpret_cast<const GLubyte*>(textdata.c_str());
   size_t ex = glutBitmapLength (glut_bitmap_font, temp);
   size_t why = glutBitmapHeight(glut_bitmap_font);
   glColor3ubv(color.ubvec);
   glLineWidth(width);
   glBegin(GL_LINE_LOOP);
   glVertex2f(center.xpos - width, center.ypos - width);
   glVertex2f(center.xpos - width, center.ypos + width + height);
   glVertex2f(center.xpos + width + length, center.ypos +width + height);
   glVertex2f(center.xpos + width + length, center.ypos - width);
   glEnd();
   vertex count = {(center.xpos + (length/2)), (center.ypos + (height/2))};
   text::showNum(count,color,number);
}
void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   int ayy = 60;
   const GLfloat theta = 2 *M_PI / ayy;
   glBegin(GL_POLYGON);
   glEnable (GL_LINE_SMOOTH);
   glColor3ubv(color.ubvec);
   for (GLfloat buf = 0; point < 2.0 * M_PI; buf += theta) {
      glVertex2f (dimension.xpos/2 * cos(point) + center.xpos,
                  dimension.ypos/2 * sin(point) + center.ypos);
   }
   glEnd();
}
void ellipse::showNum(const vertex& center, 
const rgbcolor& color, const size_t& number) const {
   if(number <= 9) {
      auto text = reinterpret_cast<const GLubyte*> (to_string(number).c_str());
      glColor3ubv (color.ubvec);
      glRasterPos2f (center.xpos, center.ypos);
      glutBitmapString (GLUT_BITMAP_8_BY_13, text);
      glEnd();
    }
}
void ellipse::draw_border (const vertex& center,
 const rgbcolor& color, const float width) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   const GLfloat theta = 2.0 * M_PI / 64;
   showNum(center,color,number);
   glLineWidth(width);
   glEnable (GL_LINE_SMOOTH);
   glBegin (GL_LINE_LOOP);
   glColor3ubv(color.ubvec);
   for (GLfloat point = 0; point < 2.0 * M_PI; point += theta) {
      glVertex2f (((dimension.xpos + width)/2 * 
                  (cos(point) + center.xpos)),
                  ((dimension.ypos + width)/2 * 
                  (sin(point) + center.ypos)));
   }
   glEnd();
}

void polygon::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin (GL_POLYGON);
   glColor3ubv (color.ubvec);
   for(auto temp: vertices) {
       glVertex2f (temp.xpos + center.xpos,
                   temp.ypos + center.ypos);
   }
   glEnd();
}

void polygon::showNum(const vertex& center,
 const rgbcolor& color, const size_t& number) const {
   if(number <= 9) {
      auto text = reinterpret_cast<const GLubyte*> (to_string(number).c_str());
      glColor3ubv (color.ubvec);
      glRasterPos2f (center.xpos, center.ypos);
      glutBitmapString (GLUT_BITMAP_8_BY_13, text);
      glEnd();
    }
}

void polygon::draw_border (const vertex& center, const rgbcolor& color,
 const GLfloat& width, const size_t& number) const {
   glLineWidth(width);
   glBegin (GL_LINE_LOOP);
   glColor3ubv (color.ubvec);
   float ex = 0;
   float why = 0;
   int count = 0;
   for(auto temp: vertices) {
      glVertex2f (temp.xpos + center.xpos, 
      temp.ypos + center.ypos);
      ex += temp.xpos + center.xpos;
      why += temp.ypos + center.ypos;
      count += 1;
   }
   if(count > 1) {
      ex = ex/ count;
      why = why / count;
   }
   glEnd();
   vertex result = {ex, why};
   showNum(result,color,number);
}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}

