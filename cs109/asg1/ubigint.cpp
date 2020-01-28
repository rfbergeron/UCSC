// $Id: ubigint.cpp,v 1.14 2016-06-23 17:21:26-07 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "debug.h"
#include "ubigint.h"

ubigint::ubigint (ubigvalue_t that): ubig_value (that) {
   // DEBUGF ('~', this << " -> " << ubig_value)
}

ubigint::ubigint (const string& that) {
   // DEBUGF ('~', "that = \"" << that << "\"");
   for (string::const_reverse_iterator rit = that.rbegin ();
        rit != that.rend ();
        ++rit) {
      if (not isdigit (*rit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      ubig_value.push_back (*rit - '0');
   }
}

ubigint ubigint::operator+ (const ubigint& that) const {
   ubigvalue_t sum;
   udigit_t carry = 0;
   udigit_t digit;
   vector<udigit_t>::const_iterator that_iter =
         that.ubig_value.begin ();
   vector<udigit_t>::const_iterator this_iter = ubig_value.begin ();

   while (that_iter != that.ubig_value.end () &&
          this_iter != ubig_value.end ()) {
      digit = *that_iter + *this_iter + carry;
      if (digit >= 10) {
         carry = 1;
         digit -= 10;
      } else {
         carry = 0;
      }

      sum.push_back (digit);
      ++this_iter;
      ++that_iter;
   }

   while (that_iter != that.ubig_value.end ()) {
      digit = *that_iter + carry;
      if (digit >= 10) {
         carry = 1;
         digit -= 10;
      } else {
         carry = 0;
      }

      sum.push_back (digit);
      ++that_iter;
   }

   while (this_iter != ubig_value.end ()) {
      digit = *this_iter + carry;
      if (digit >= 10) {
         carry = 1;
         digit -= 10;
      } else {
         carry = 0;
      }

      sum.push_back (digit);
      ++this_iter;
   }

   if (carry != 0) {
      sum.push_back (carry);
   }

   while (sum.size () > 0 && sum.back () == 0) {
      sum.pop_back ();
   }

   return ubigint (sum);
}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   ubigvalue_t difference;
   udigit_t carry = 0;
   udigit_t digit;
   vector<udigit_t>::const_iterator iter_that =
         that.ubig_value.begin ();
   vector<udigit_t>::const_iterator iter_this = ubig_value.begin ();

   while (iter_that != that.ubig_value.end () &&
          iter_this != ubig_value.end ()) {
      digit = *iter_this - *iter_that;
      if (*iter_this < *iter_that + carry) {
         digit = *iter_this + 10 - *iter_that - carry;
         carry = 1;
      } else {
         digit = *iter_this - *iter_that - carry;
         carry = 0;
      }

      difference.push_back (digit);
      ++iter_this;
      ++iter_that;
   }

   // that should be shorter than this, so no need
   // to iterate through it. All we need to do now
   // is finish off this. Still need to track carry
   // and handle cascading decrease, but there's
   // nothing else to subtract from this
   while (iter_this != ubig_value.end ()) {
      digit = *iter_this - carry;
      if (*iter_this < carry) {
         digit = *iter_this + 10 - carry;
         carry = 1;
      } else {
         digit = *iter_this - carry;
         carry = 0;
      }

      difference.push_back (digit);
      ++iter_this;
   }

   while (difference.size () > 0 && difference.back () == 0) {
      difference.pop_back ();
   }

   return ubigint (difference);
}

ubigint ubigint::operator* (const ubigint& that) const {
   ubigvalue_t product;
   int product_length = ubig_value.size () + that.ubig_value.size ();

   for (int j = 0; j < product_length; ++j) {
      product.push_back (0);
   }

   for (unsigned int i = 0; i < ubig_value.size (); ++i) {
      udigit_t carry = 0;
      for (unsigned int j = 0; j < that.ubig_value.size (); ++j) {
         udigit_t partial =
               product.at (i + j) +
               (ubig_value.at (i) * that.ubig_value.at (j)) + carry;
         product.at (i + j) = partial % 10;
         carry = partial / 10;
      }
      product.at (i + that.ubig_value.size ()) = carry;
   }

   return product;
}

void ubigint::multiply_by_2 () {
   ubigint two = ubigint ("2");
   ubigint product = *this * two;
   this->ubig_value = product.ubig_value;
}

void ubigint::divide_by_2 () {
   ubigvalue_t dividend;
   vector<udigit_t>::const_iterator iter = ubig_value.begin ();
   while (iter != ubig_value.end ()) {
      udigit_t current = *iter / 2;
      ++iter;
      if (iter != ubig_value.end ()) {
         if (*iter % 2 != 0) {
            current += 5;
         }
      }
      dividend.push_back (current);
   }
   ubig_value = dividend;
}

struct quo_rem {
   ubigint quotient;
   ubigint remainder;
};
quo_rem udivide (const ubigint& dividend, ubigint divisor) {
   // Note: divisor is modified so pass by value (copy).
   ubigint zero;
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 = ubigint ("1");
   ubigint quotient;
   ubigint remainder = dividend;   // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2 ();
      power_of_2.multiply_by_2 ();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2 ();
      power_of_2.divide_by_2 ();
   }
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   if (that.ubig_value.size () != ubig_value.size ()) {
      return false;
   }

   vector<udigit_t>::const_iterator iter_that =
         that.ubig_value.begin ();
   vector<udigit_t>::const_iterator iter_this = ubig_value.begin ();

   while (iter_that != that.ubig_value.end () &&
          iter_this != ubig_value.end ()) {
      if (*iter_that != *iter_this) {
         return false;
      }
      ++iter_that;
      ++iter_this;
   }

   return true;
}

bool ubigint::operator< (const ubigint& that) const {
   if (that.ubig_value.size () < ubig_value.size ()) {
      return false;
   } else if (that.ubig_value.size () > ubig_value.size ()) {
      return true;
   }

   vector<udigit_t>::const_iterator iter_that =
         that.ubig_value.begin ();
   vector<udigit_t>::const_iterator iter_this = ubig_value.begin ();

   while (iter_that != that.ubig_value.end () &&
          iter_this != ubig_value.end ()) {
      if (*iter_that > *iter_this) {
         return true;
      } else if (*iter_that < *iter_this) {
         return false;
      }
      ++iter_that;
      ++iter_this;
   }

   return false;
}

ostream& operator<< (ostream& out, const ubigint& that) {
   vector<udigit_t>::reverse_const_iterator rit =
         that.ubig_value.rbegin ();
   string output;
   while (rit != that.ubig_value.rend ()) {
      output.append (*rit);
      rit++;
   }

   return out << "ubigint(" << output << ")";
}
