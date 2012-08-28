// $Id: bigint.h,v 1.2 2012-04-10 21:02:20-07 - - $

/* 
 * B Stewart Bracken
 * bbracken@ucsc.edu
 */

#ifndef __BIGINT_H__
#define __BIGINT_H__

#include <exception>
#include <iostream>
#include <utility>

using namespace std;

#include "trace.h"

class bigint;
typedef pair <bigint, bigint> bigpair;

//
// Operators with a left side of int.
//
bigint operator+ (int left, const bigint &that);
bigint operator- (int left, const bigint &that);
bigint operator* (int left, const bigint &that);
bigint operator/ (int left, const bigint &that);
bigint operator% (int left, const bigint &that);
bool operator== (int left, const bigint &that);
bool operator!= (int left, const bigint &that);
bool operator<  (int left, const bigint &that);
bool operator<= (int left, const bigint &that);
bool operator>  (int left, const bigint &that);
bool operator>= (int left, const bigint &that);


//
// Define class bigint
//
class bigint {
      friend ostream &operator<< (ostream &, const bigint &);
   private:
      void init(const string& s);
      typedef unsigned char digit_t;
      typedef vector<digit_t> bigvalue_t;
      bool negative;
      bigvalue_t *big_value;
      bigpair div_rem (const bigint &that) const;
      int compare (const bigint &that) const;
      int abscompare (const bigint &that) const;
      bigint mul_by_2 ();
      void do_bigadd(const bigvalue_t* a, const bigvalue_t* b,
                     bigvalue_t* target) const;
      void do_bigsub(const bigvalue_t* a, const bigvalue_t* b,
                     bigvalue_t* target) const;
      void remove_lead_0s(bigvalue_t* value) const;
      void set_negative(bool positivity);
      void zero_positive_check();
   public:
      //
      // Override implicit members.
      //
      bigint ();
      bigint (const bigint &that);
      bigint &operator= (const bigint &that);
      ~bigint ();
      //
      // Extra ctors to make bigints.
      //
      bigint (const int that);
      bigint (const string &that);
      //
      // Basic add/sub operators.
      //
      bigint operator+ (const bigint &that) const;
      bigint operator- (const bigint &that) const;
      bigint operator- () const;
      int smallint () const;
      //
      // Extended operators implemented with add/sub.
      //
      bigint operator* (const bigint &that) const;
      bigint operator/ (const bigint &that) const;
      bigint operator% (const bigint &that) const;
      bigint pow (const bigint &that) const;
      //
      // Comparison operators.
      //
      bool operator== (const bigint &that) const;
      bool operator!= (const bigint &that) const;
      bool operator<  (const bigint &that) const;
      bool operator<= (const bigint &that) const;
      bool operator>  (const bigint &that) const;
      bool operator>= (const bigint &that) const;
};

#endif

