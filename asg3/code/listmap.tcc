// $Id: listmap.tcc,v 1.15 2019-10-30 12:44:53-07 - - $

#include "listmap.h"
#include "debug.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename key_t, typename mapped_t, class less_t> listmap<key_t,mapped_t,less_t>::~listmap() {
   while(!empty()){
      erase(begin());
   }
}

//
// iterator listmap::insert (const value_type&)
//
template <typename key_t, typename mapped_t, class less_t> typename listmap<key_t,mapped_t,less_t>::iterator listmap<key_t,mapped_t,less_t>::insert (const value_type& pair) {
   DEBUGF ('l', &pair << "->" << pair);
   iterator buf = begin();
   while(buf != end() && less(buf->first, pair.first)){
      ++buf;
   }

   if(buf != end() && !less(pair.first, buf->first)){
      buf->second = pair.second;
      return buf;
   }
   node* swag = new node(buf.where, buf.where->prev, pair);
   buf.where->prev->next = swag;
   buf.where->prev = swag;
   
   return iterator();
}

//
// listmap::find(const key_type&)
//
template <typename key_t, typename mapped_t, class less_t> typename listmap<key_t,mapped_t,less_t>::iterator listmap<key_t,mapped_t,less_t>::find (const key_type& that) {
   DEBUGF ('l', that);
   iterator buf = begin();
   while( buf != end() && buf->first != that){
      ++buf;
   }
   return buf;
}

//
// iterator listmap::erase (iterator position)
//
template <typename key_t, typename mapped_t, class less_t> typename listmap<key_t,mapped_t,less_t>::iterator listmap<key_t,mapped_t,less_t>::erase (iterator position) {
   DEBUGF ('l', &*position);
   iterator buf = position.where;
   ++buf;
   position.where->prev->next = position.where->next;
   position.where->next->prev = position.where->prev;
   delete position.where;
   return buf;
}
