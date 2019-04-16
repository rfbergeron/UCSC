// $Id: shape.cpp,v 1.2 2019-02-28 15:24:20-08 - - $
// John Gnanasekaran (jgnanase) and Robert Bergeron (rbergero)

#include <typeinfo>
#include <unordered_map>
#include <cmath>
using namespace std;

#include "shape.h"
#include "util.h"

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

vertex_list rectangle::from_dimensions(GLfloat width, GLfloat height) {
   vertex_list newlist;
   newlist.push_back({GLfloat(0), GLfloat(0)});
   newlist.push_back({GLfloat(0), height});
   newlist.push_back({width, height});
   newlist.push_back({width, GLfloat(0)});
   return newlist;
}

vertex_list diamond::from_dimensions(GLfloat width, GLfloat height) {
   vertex_list newlist;
   newlist.push_back({GLfloat(0), height / 2});
   newlist.push_back({width / 2, height});
   newlist.push_back({width, height / 2});
   newlist.push_back({width / 2, GLfloat(0)});
   return newlist;
}

vertex_list isosceles::from_dimensions(GLfloat width, GLfloat height) {
   vertex_list newlist;
   newlist.push_back({GLfloat(0), GLfloat(0)});
   newlist.push_back({width / 2, height});
   newlist.push_back({width, GLfloat(0)});
   return newlist;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (string bitmap_name, const string& textdata_):
      glut_bitmap_font(fontcode[bitmap_name]), textdata(textdata_) {
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
            polygon(from_dimensions(width, height)) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");

}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

diamond::diamond (GLfloat width, GLfloat height): 
   polygon(from_dimensions(width, height)) {
   DEBUGF ('c', this);
}

triangle::triangle (const vertex_list& vertices_): polygon(vertices_) {
   DEBUGF ('c', this);
}

isosceles::isosceles (GLfloat width, GLfloat height):
            triangle(from_dimensions(width, height)) {
   DEBUGF ('c', this);
}

equilateral::equilateral (GLfloat width):
   isosceles(width, width * sqrt(3.0) / 2.0) {
   DEBUGF ('c', this);
}

void text::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   const GLubyte* glgreeting =
         reinterpret_cast<const GLubyte*> (textdata.c_str());
   
   int greeting_width = glutBitmapLength (glut_bitmap_font, glgreeting);
   int greeting_height = glutBitmapHeight (glut_bitmap_font);
   
   glColor3ubv(color.ubvec);
   glRasterPos2f(center.xpos - greeting_width / 2.0, 
      center.ypos - greeting_height / 2.0);      
   glutBitmapString (glut_bitmap_font, glgreeting);
}

void text::draw_border (const vertex& center, 
   const rgbcolor& color, GLfloat width) const {
   DEBUGF ('d', this << "(" << center << "," 
      << color << "," << width << ")");
   const GLubyte* glgreeting =
         reinterpret_cast<const GLubyte*> (textdata.c_str());
         
   GLfloat greeting_width (glutBitmapLength
      (glut_bitmap_font, glgreeting));
   GLfloat greeting_height (glutBitmapHeight (glut_bitmap_font));
      
   rectangle border(greeting_width, greeting_height);
   border.draw_border(center, color, width);
}

void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin (GL_POLYGON);
   glColor3ubv (color.ubvec);
   const GLfloat delta = 2 * M_PI / 64;
   const GLfloat width = dimension.xpos;
   const GLfloat height = dimension.ypos;
   const GLfloat xoffset = center.xpos;
   const GLfloat yoffset = center.ypos;
   for (GLfloat theta = 0; theta < 2 * M_PI; theta += delta) {
      GLfloat xpos = width * cos (theta) + xoffset;
      GLfloat ypos = height * sin (theta) + yoffset;
      glVertex2f (xpos, ypos);
   }
   glEnd();
}

void ellipse::draw_border (const vertex& center, 
   const rgbcolor& color, GLfloat border_width) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin (GL_POLYGON);
   glColor3ubv (color.ubvec);
   const GLfloat delta = 2 * M_PI / 64;
   const GLfloat width = dimension.xpos;
   const GLfloat height = dimension.ypos;
   const GLfloat xoffset = center.xpos;
   const GLfloat yoffset = center.ypos;
   for (GLfloat theta = 0; theta < 2 * M_PI; theta += delta) {
      GLfloat xpos = (width + border_width) * cos (theta) + xoffset;
      GLfloat ypos = (height + border_width) * sin (theta) + yoffset;
      glVertex2f (xpos, ypos);
   }
   glEnd();
}

void polygon::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin (GL_POLYGON);
   glColor3ubv (color.ubvec);
   const GLfloat xoffset = center.xpos;
   const GLfloat yoffset = center.ypos;
   GLfloat x_center = 0;
   GLfloat y_center = 0;
   for(auto& vertex : vertices) {
      x_center += vertex.xpos;
      y_center += vertex.ypos;
   }
   x_center /= vertices.size();
   y_center /= vertices.size();
   for (auto& vertex : vertices) {
      GLfloat xpos = vertex.xpos + xoffset - x_center;
      GLfloat ypos = vertex.ypos + yoffset - y_center;
      glVertex2f (xpos, ypos);
   }
   glEnd();
}

void polygon::draw_border (const vertex& center, 
   const rgbcolor& color, GLfloat border_width) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin (GL_POLYGON);
   glColor3ubv (color.ubvec);
   const GLfloat xoffset = center.xpos;
   const GLfloat yoffset = center.ypos;
   GLfloat x_center = 0;
   GLfloat y_center = 0;
   for(auto& vertex : vertices) {
      x_center += vertex.xpos;
      y_center += vertex.ypos;
   }
   x_center /= vertices.size();
   y_center /= vertices.size();
   for (auto& vertex : vertices) {
      GLfloat run = vertex.xpos - x_center;
      GLfloat rise = vertex.ypos - y_center;
      GLfloat magnitude = sqrt((run * run) + (rise * rise));
      GLfloat xpos = vertex.xpos + xoffset - x_center 
         + (run / magnitude * border_width);
      GLfloat ypos = vertex.ypos + yoffset - y_center 
         + (rise / magnitude * border_width);
      glVertex2f (xpos, ypos);
   }
   glEnd();
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
