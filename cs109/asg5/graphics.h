// $Id: graphics.h,v 1.1 2015-07-16 16:47:51-07 - - $
// John Gnanasekaran (jgnanase) and Robert Bergeron (rbergero)

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <memory>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "rgbcolor.h"
#include "shape.h"

class object {
   private:
   shared_ptr<shape> pshape;
   vertex center;
   rgbcolor color;

   public:
   // Default copiers, movers, dtor all OK.
   object (shared_ptr<shape> ps, vertex cn, rgbcolor cl):
         pshape (ps),
         center (cn),
         color (cl) {}
   void draw () { pshape->draw (center, color); }
   void draw_border (rgbcolor bordercolor, GLfloat borderwidth) {
      pshape->draw_border (center, bordercolor, borderwidth);
   }
   void move (GLfloat delta_x, GLfloat delta_y) {
      center.xpos += delta_x;
      center.ypos += delta_y;
   }
};

class mouse {
   friend class window;

   private:
   int xpos {0};
   int ypos {0};
   int entered {GLUT_LEFT};
   int left_state {GLUT_UP};
   int middle_state {GLUT_UP};
   int right_state {GLUT_UP};

   private:
   void set (int x, int y) {
      xpos = x;
      ypos = y;
   }
   void state (int button, int state);
   void draw ();
};

class window {
   friend class mouse;

   private:
   static int width;    // in pixels
   static int height;   // in pixels
   static vector<object> objects;
   static size_t selected_obj;
   static mouse mus;
   static rgbcolor bordercolor;
   static GLfloat borderwidth;
   static GLfloat stepsize;

   private:
   static void close ();
   static void entry (int mouse_entered);
   static void display ();
   static void reshape (int width, int height);
   static void keyboard (GLubyte key, int, int);
   static void special (int key, int, int);
   static void motion (int x, int y);
   static void passivemotion (int x, int y);
   static void mousefn (int button, int state, int x, int y);
   static void select_object (size_t);
   static void move_selected_object (GLfloat, GLfloat);

   public:
   static void push_back (const object& obj) {
      objects.push_back (obj);
   }
   static void setwidth (int width_) { width = width_; }
   static void setheight (int height_) { height = height_; }
   static void setborder (rgbcolor bordercolor_, GLfloat borderwidth_) {
      bordercolor = bordercolor_;
      borderwidth = borderwidth_;
   }
   static void setstepsize (GLfloat stepsize_) { stepsize = stepsize_; }
   static void main ();
};

#endif
