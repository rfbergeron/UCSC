// $Id: bigint.cpp,v 1.76 2016-06-14 16:34:24-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"

bigint::bigint (ubigvalue_t that):
      ubig_value (that),
      is_negative (that < 0) {
   // DEBUGF ('~', this << " -> " << uvalue)
}

bigint::bigint (const ubigint& uvalue, bool is_negative):
      uvalue (uvalue),
      is_negative (is_negative) {}

bigint::bigint (const string& that) {
   is_negative = that.size () > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
}

bigint bigint::operator+ () const {}

bigint bigint::operator- () const { return {uvalue, not is_negative}; }

bigint bigint::operator+ (const bigint& that) const {
   ubigint result = uvalue - that.uvalue;
   return result;
}

bigint bigint::operator- (const bigint& that) const {
   ubigint result = uvalue - that.uvalue;
   return result;
}

bigint bigint::operator* (const bigint& that) const {
   bigint result = uvalue * that.uvalue;
   if (is_negative != that.is_negative) {
      result.is_negative = true;
   }
   return result;
}

bigint bigint::operator/ (const bigint& that) const {
   bigint result = uvalue / that.uvalue;
   if (is_negative != that.is_negative) {
      result.is_negative = true;
   }
   return result;
}

bigint bigint::operator% (const bigint& that) const {
   bigint result = uvalue % that.uvalue;
   return result;
}

bool bigint::operator== (const bigint& that) const {
   // If the vector isn't the same or the signs aren't the same return
   // false
   if (uvalue != that.uvalue || is_negative != that.is_negative) {
      return false;
   } else
      return true;
}

bool bigint::operator< (const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
   return out << "bigint(" << (that.is_negative ? "-" : "+") << ","
              << that.uvalue << ")";
}
