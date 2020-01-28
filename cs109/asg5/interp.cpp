// $Id: interp.cpp,v 1.2 2015-07-16 16:57:30-07 - - $
// John Gnanasekaran (jgnanase) and Robert Bergeron (rbergero)

#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

unordered_map<string, interpreter::interpreterfn>
      interpreter::interp_map {
            {"define", &interpreter::do_define},
            {"draw", &interpreter::do_draw},
            {"border", &interpreter::do_border},
            {"moveby", &interpreter::do_moveby},
      };

unordered_map<string, interpreter::factoryfn> interpreter::factory_map {
      {"text", &interpreter::make_text},
      {"ellipse", &interpreter::make_ellipse},
      {"circle", &interpreter::make_circle},
      {"polygon", &interpreter::make_polygon},
      {"rectangle", &interpreter::make_rectangle},
      {"square", &interpreter::make_square},
      {"diamond", &interpreter::make_diamond},
      {"triangle", &interpreter::make_triangle},
      {"isosceles", &interpreter::make_isosceles},
};

interpreter::shape_map interpreter::objmap;

interpreter::~interpreter () {
   for (const auto& itor : objmap) {
      cout << "objmap[" << itor.first << "] = " << *itor.second << endl;
   }
}

// only defines the shape directly; drawing is not done directly
// through this func
void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   param begin = params.cbegin ();
   string command = *begin;
   // iterates throught string vector
   auto itor = interp_map.find (command);
   if (itor == interp_map.end ()) throw runtime_error ("syntax error");
   // calls the function associated with the input command
   interpreterfn func = itor->second;
   func (++begin, params.cend ());
}

// VERY IMPORTANT!!!
// all params are iterators to the vector<string> argument

// begin should point to the name of the shape now
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
   if (itor == objmap.end ()) {
      throw runtime_error (name + ": no such shape");
   }
   rgbcolor color {begin[0]};
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
   // itor->second->draw (where, color);
   window::push_back ({itor->second, where, color});
}

void interpreter::do_border (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 2) throw runtime_error ("syntax error");
   string colorname = *begin++;
   window::setborder (rgbcolor (colorname), stof (*begin));
}

void interpreter::do_moveby (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 1) throw runtime_error ("syntax error");
   window::setstepsize (GLfloat (stof (*begin)));
}

// begin should point to the type of shape now
shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find (type);
   if (itor == factory_map.end ()) {
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   return func (begin, end);
}

// for all of these, begin should now just point to the first argument
shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string name = *begin++;
   return make_shared<text> (name, *begin);
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<ellipse> (GLfloat (stof (*begin++)),
                                GLfloat (stof (*begin)));
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<circle> (GLfloat (stof (*begin)));
}

shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   vertex_list make_list;
   while (begin != end) {
      int x = stof (*begin);
      ++begin;
      int y = stof (*begin);
      ++begin;
      make_list.push_back ({GLfloat (x), GLfloat (y)});
   }
   return make_shared<polygon> (make_list);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   float w = stof (*begin);
   ++begin;
   float h = stof (*begin);
   return make_shared<rectangle> (GLfloat (w), GLfloat (h));
}

shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<square> (GLfloat (stof (*begin)));
}

shape_ptr interpreter::make_diamond (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   float w = stof (*begin);
   ++begin;
   float h = stof (*begin);
   return make_shared<diamond> (GLfloat (w), GLfloat (h));
}

shape_ptr interpreter::make_triangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   vertex_list make_list;
   while (begin != end) {
      int x = stof (*begin);
      ++begin;
      int y = stof (*begin);
      ++begin;
      make_list.push_back ({GLfloat (x), GLfloat (y)});
   }
   return make_shared<triangle> (make_list);
}

shape_ptr interpreter::make_isosceles (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   float w = stof (*begin);
   ++begin;
   float h = stof (*begin);
   return make_shared<isosceles> (GLfloat (w), GLfloat (h));
}

shape_ptr interpreter::make_equilateral (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<equilateral> (GLfloat (stof (*begin)));
}
