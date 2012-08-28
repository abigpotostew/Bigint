// $Id: iterstack.h,v 1.6 2012-04-12 19:32:54-07 - - $

/* 
 * B Stewart Bracken
 * bbracken@ucsc.edu
 */

// 
// The class std::stack does not provide an iterator, which is
// needed for this class.  So, like std::stack, class iterstack
// is implemented on top of a container.
// 
// We use private inheritance because we want to restrict
// operations only to those few that are approved.  All functions
// are merely inherited from the container, with only ones needed
// being exported as public.
// 
// No implementation file is needed because all functions are
// inherited, and the convenience functions that are added are
// trivial, and so can be inline.
// 

#ifndef __ITERSTACK_H__
#define __ITERSTACK_H__

#include <list>

using namespace std;

template <typename value_type>
class iterstack: private list<value_type> {
   public:
      using list<value_type>::const_reference;
      using list<value_type>::const_reverse_iterator;
      using list<value_type>::push_back;
      using list<value_type>::pop_back;
      using list<value_type>::clear;
      using list<value_type>::back;
      using list<value_type>::size;
      using list<value_type>::empty;
      using list<value_type>::rbegin;
      using list<value_type>::rend;
      inline void push (const value_type &value) { push_back (value); }
      inline void pop() { pop_back(); }
      inline value_type& top() { return back(); }
      inline const value_type& top() const { return back(); }
};

#endif

