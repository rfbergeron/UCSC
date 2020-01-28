// $Id: listmap.tcc,v 1.11 2018-01-25 14:19:14-08 - - $
// John Gnanasekaran(jgnanase) and Robert Bergeron (rbergero)

#include "debug.h"
#include "listmap.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::node.
/////////////////////////////////////////////////////////////////
//

//
// listmap::node::node (link*, link*, const value_type&)
//
template<typename Key, typename Value, class Less>
listmap<Key, Value, Less>::node::node (node* next,
                                       node* prev,
                                       const value_type& value):
      link (next, prev),
      value (value) {}

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template<typename Key, typename Value, class Less>
listmap<Key, Value, Less>::~listmap () {
   while (!empty ()) {
      erase (begin ());
   }
   DEBUGF ('l', reinterpret_cast<const void*> (this));
}

//
// iterator listmap::insert (const value_type&)
//
template<typename Key, typename Value, class Less>
typename listmap<Key, Value, Less>::iterator
listmap<Key, Value, Less>::insert (const value_type& pair) {
   // listmap::node::node new_node = listmap::node::node();
   listmap<Key, Value, Less>::iterator finder = begin ();

   // loops until it finder reaches a key which is not less than
   // than the key that is to be inserted
   while (finder != end () && compare ((*finder).first, pair.first)) {
      ++finder;
   }

   // checks that the keys are not equal. if they are, replace
   // existing value at that key
   if (finder != end () && !compare (pair.first, (*finder).first)) {
      (*finder).second = pair.second;
      return finder;
   }

   listmap<Key, Value, Less>::node* to_insert =
         new listmap<Key, Value, Less>::node (finder.where,
                                              finder.where->prev,
                                              pair);

   DEBUGF ('y', "Inserting node with value " << to_insert->value);

   finder.where->prev->next = to_insert;
   finder.where->prev = to_insert;

   DEBUGF ('y',
           "Addresses that should match:\n"
                 << to_insert->prev->next << " and " << to_insert
                 << "\n"
                 << to_insert << " and " << finder.where->prev);

   DEBUGF ('l', &pair << "->" << pair);
   return {to_insert};
}

//
// listmap::find(const key_type&)
//
template<typename Key, typename Value, class Less>
typename listmap<Key, Value, Less>::iterator
listmap<Key, Value, Less>::find (const key_type& that) {
   listmap<Key, Value, Less>::iterator iter = begin ();
   while (iter != end ()) {
      if ((*iter).first == that) {
         return iter;
      }
      ++iter;
   }
   return iter;
   DEBUGF ('l', that);
}

//
// iterator listmap::erase (iterator position)
//
template<typename Key, typename Value, class Less>
typename listmap<Key, Value, Less>::iterator
listmap<Key, Value, Less>::erase (iterator position) {
   DEBUGF ('l', &*position);
   if (!empty ()) {
      listmap<Key, Value, Less>::node* target_prev =
            position.where->prev;
      listmap<Key, Value, Less>::node* target_next =
            position.where->next;

      target_prev->next = target_next;
      target_next->prev = target_prev;

      delete position.where;

      return {target_next};
   } else {
      cerr << "Warning: list is empty.\n";
      return end ();
   }
}

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::iterator.
/////////////////////////////////////////////////////////////////
//

//
// listmap::value_type& listmap::iterator::operator*()
//
template<typename Key, typename Value, class Less>
typename listmap<Key, Value, Less>::value_type&
      // value is a pair variable. Not a pointer.
      // method returns a reference
      listmap<Key, Value, Less>::iterator::operator* () {
   DEBUGF ('l', where);
   return where->value;
}

//
// listmap::value_type* listmap::iterator::operator->()
//
template<typename Key, typename Value, class Less>
typename listmap<Key, Value, Less>::value_type*
      listmap<Key, Value, Less>::iterator::operator-> () {
   DEBUGF ('l', where);
   return &(where->value);
}

//
// listmap::iterator& listmap::iterator::operator++()
//
template<typename Key, typename Value, class Less>
typename listmap<Key, Value, Less>::iterator&
listmap<Key, Value, Less>::iterator::operator++ () {
   DEBUGF ('l', where);
   where = where->next;
   return *this;
}

//
// listmap::iterator& listmap::iterator::operator--()
//
template<typename Key, typename Value, class Less>
typename listmap<Key, Value, Less>::iterator&
listmap<Key, Value, Less>::iterator::operator-- () {
   DEBUGF ('l', where);
   where = where->prev;
   return *this;
}

//
// bool listmap::iterator::operator== (const iterator&)
//
template<typename Key, typename Value, class Less>
inline bool listmap<Key, Value, Less>::iterator::operator== (
      const iterator& that) const {
   return this->where == that.where;
}

//
// bool listmap::iterator::operator!= (const iterator&)
//
template<typename Key, typename Value, class Less>
inline bool listmap<Key, Value, Less>::iterator::operator!= (
      const iterator& that) const {
   return this->where != that.where;
}
