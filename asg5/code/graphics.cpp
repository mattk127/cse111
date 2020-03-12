// $Id: graphics.cpp,v 1.6 2019-05-15 18:02:12-07 - - $

#include <iostream>
using namespace std;

#include <GL/freeglut.h>

#include "graphics.h"
#include "util.h"

int window::width = 640; // in pixels
int window::height = 480; // in pixels
int window::moveBy = 4;
rgbcolor window::borderColor = rgbcolor{0xFF,0x00,0x00};
GLfloat window::borderWidth = 4.0;
vector<object> window::objects;
size_t window::selected_obj = 0;
mouse window::mus;

void window::close(){
  exit (sys_info::exit_status());
  DEBUGF ('g', sys_info::execname() << ": exit (" 
   << sys_info::exit_status() << ")");
}

// Implementation of object functions.
object::object (const shared_ptr<shape>& shape, vertex& where,
rgbcolor colr) {
  pshape = shape;
  center = where;
  color = colr;
}

void window::entry (int mouse_entered){
  window::mus.entered = mouse_entered;
  // if(window::mus.entered == GLUT_ENTERED){
  //   DEBUGF('g',sys_info::execname)
  // }
  glutPostRedisplay();
}

void window::display(){
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_COLOR_BUFFER_BIT);
  for(auto& object: window::objects){
    if(object.center.ypos < 0){
      object.center.ypos = window::height;
    }
    if(object.center.xpos < 0){
      object.center.xpos = window::width;
    }
    if(object.center.ypos > window::height){
      object.center.ypos = 0;
    }
    if(object.center.xpos > window::width){
      object.center.xpos = 0;
    }
    object.draw();
  }
  if(window::selected_obj > window::objects.size()){
    --window::selected_obj;
  }else if(window::selected_obj < 
    window::objects.size()-window::objects.size()){
      ++window::selected_obj;
  }else{
    window::objects[window::selected_obj].draw_border(
      borderColor, borderWidth, window::selected_obj);
  }
  mus.draw();
  glutSwapBuffers();
}

// Called when window is opened and when resized.
void window::reshape (int width, int height) {
  DEBUGF ('g', "width=" << width << ", height=" << height);
  window::width = width;
  window::height = height;
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D (0, window::width, 0, window::height);
  glMatrixMode (GL_MODELVIEW);
  glViewport (0, 0, window::width, window::height);
  glClearColor (0.25, 0.25, 0.25, 1.0);
  glutPostRedisplay();
}

// Executed when a regular keyboard key is pressed.
void window::keyboard (GLubyte key, int x, int y) {
   enum {BS = 8, TAB = 9, ESC = 27, SPACE = 32, DEL = 127};
   DEBUGF ('g', "key=" << unsigned (key) << ", x=" << x << ", y=" << y);
   window::mus.set (x, y);
   switch (key) {
    case 'Q': case 'q': case ESC:
      window::close();
      break;
    case 'H': case 'h':
      move_selected_object(-1 * moveBy,0);
      break;
    case 'J': case 'j':
      move_selected_object(0, -1 * moveBy);
      break;
    case 'K': case 'k':
      move_selected_object(0, 1 * moveBy);
      break;
    case 'L': case 'l':
      move_selected_object(1 * moveBy,0);
      break;
    case 'N': case 'n': case SPACE: case TAB:
      select_object(window::selected_obj+1);    
      break;
    case 'P': case 'p': case BS:
      select_object(window::selected_obj-1);
      break;
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      select_object (key - '0');
      break;
    default:
      cerr << key << ": invalid keystroke" << endl;
      break;
   }
   glutPostRedisplay();
}

// Executed when a special function key is pressed.
void window::special (int key, int x, int y) {
  DEBUGF ('g', "key=" << key << ", x=" << x << ", y=" << y);
  window::mus.set (x, y);
  switch (key) {
    case GLUT_KEY_LEFT: move_selected_object (-1 * moveBy, 0); break;
    case GLUT_KEY_DOWN: move_selected_object (0, -1 * moveBy); break;
    case GLUT_KEY_UP: move_selected_object (0, +1 * moveBy); break;
    case GLUT_KEY_RIGHT: move_selected_object (+1 * moveBy, 0); break;
    case GLUT_KEY_F1: select_object (1); break;
    case GLUT_KEY_F2: select_object (2); break;
    case GLUT_KEY_F3: select_object (3); break;
    case GLUT_KEY_F4: select_object (4); break;
    case GLUT_KEY_F5: select_object (5); break;
    case GLUT_KEY_F6: select_object (6); break;
    case GLUT_KEY_F7: select_object (7); break;
    case GLUT_KEY_F8: select_object (8); break;
    case GLUT_KEY_F9: select_object (9); break;
    case GLUT_KEY_F10: select_object (10); break;
    case GLUT_KEY_F11: select_object (11); break;
    case GLUT_KEY_F12: select_object (12); break;
    default:
      cerr << unsigned (key) << ": invalid function key" << endl;
      break;
   }
   glutPostRedisplay();
}


void window::motion (int x, int y) {
  DEBUGF ('g', "x=" << x << ", y=" << y);
  window::mus.set (x, y);
  glutPostRedisplay();
}

void window::passivemotion (int x, int y) {
  DEBUGF ('g', "x=" << x << ", y=" << y);
  window::mus.set (x, y);
  glutPostRedisplay();
}

void window::mousefn (int button, int state, int x, int y) {
   DEBUGF ('g', "button=" << button << ", state=" << state 
   << ", x=" << x << ", y=" << y);
   window::mus.state (button, state);
   window::mus.set (x, y);
   glutPostRedisplay();
}

// void object::draw() {
//   pshape->draw (center, color);
// }

// void object::move (GLfloat delta_x, GLfloat delta_y) {
//   center.xpos += delta_x;
//   center.ypos += delta_y;
// }

// Implementation of mouse functions.
void mouse::state (int button, int state) {
  switch (button) {
    case GLUT_LEFT_BUTTON: left_state = state; break;
    case GLUT_MIDDLE_BUTTON: middle_state = state; break;
    case GLUT_RIGHT_BUTTON: right_state = state; break;
  }
}

void mouse::draw() {
  static rgbcolor color ("green");
  ostringstream text;
  text << "(" << xpos << "," << window::height - ypos << ")";
  if (left_state == GLUT_DOWN) text << "L"; 
  if (middle_state == GLUT_DOWN) text << "M"; 
  if (right_state == GLUT_DOWN) text << "R"; 
  if (entered == GLUT_ENTERED) {
    void* font = GLUT_BITMAP_HELVETICA_18;
    glColor3ubv (color.ubvec);
    glRasterPos2i (10, 10);
    auto ubytes = reinterpret_cast<const GLubyte*>
    (text.str().c_str());
    glutBitmapString (font, ubytes);  }
}

void window::main () {
  static int argc = 0;
  glutInit (&argc, nullptr);
  glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE);
  glutInitWindowSize (window::width, window::height);
  glutInitWindowPosition (128, 128);
  glutCreateWindow (sys_info::execname().c_str());
  glutCloseFunc (window::close);
  glutEntryFunc (window::entry);
  glutDisplayFunc (window::display);
  glutReshapeFunc (window::reshape);
  glutKeyboardFunc (window::keyboard);
  glutSpecialFunc (window::special);
  glutMotionFunc (window::motion);
  glutPassiveMotionFunc (window::passivemotion);
  glutMouseFunc (window::mousefn);
  DEBUGF ('g', "Calling glutMainLoop()");
  glutMainLoop();
}

