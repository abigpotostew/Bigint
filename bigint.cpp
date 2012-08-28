// $Id: bigint.cpp,v 1.250 2012-04-30 12:58:21-07 - - $

/* 
 * B Stewart Bracken
 * bbracken@ucsc.edu
 */

#include <locale>
#include <exception>
#include <limits>
#include <stack>
#include <stdexcept>
#include <sstream>
#include <cmath>

using namespace std;

#include "bigint.h"
#include "trace.h"

#define CDTOR_TRACE TRACE ('~', this<<" -> " << \
                           big_value <<" = "<<*this)



bigint::bigint (): negative(false),
                   big_value (new bigvalue_t()) {
                      CDTOR_TRACE;
}

bigint::bigint (const bigint &that): negative(that.negative){
   if (this == &that)
      *this = that;
   else 
      big_value = new bigvalue_t(*that.big_value);
   CDTOR_TRACE;
}

bigint &bigint::operator= (const bigint &that) {
   TRACE('c', "Copying by value (deep) " << *this);
   if (this == &that) return *this;
   if ( big_value != NULL )
      delete big_value;
   big_value = new bigvalue_t(*(that.big_value));
   this->negative = that.negative;
   return *this;
}

bigint::~bigint() {
   TRACE('~',  "Deconstructing: this["<<this<<"] -> "<<*this);
   if ( big_value!=NULL)
      delete big_value;
}

bigint::bigint (int that): negative(false),
            big_value(new bigvalue_t()) {
   string s;
   stringstream out;
   out << that;
   s = out.str();
   init(s);
   CDTOR_TRACE;
}

bigint::bigint (const string &that): negative(false),
                           big_value(new bigvalue_t()){
   init(that);
   CDTOR_TRACE;
}

void bigint::init(const string& s){
   string::const_reverse_iterator itor = s.rbegin();
   string::const_reverse_iterator end = s.rend();
   int offset = 0;
   if ( *(end - 1) == '-' || *(end - 1) == '_' ) {
      negative = true;
      offset = 1;
   }
   for(; itor != end-offset; ++itor ){
      if (! isdigit (*itor)) continue;
      big_value->push_back( *itor - '0' );
   }
}

bigint bigint::operator+ (const bigint &that) const {
   bigint result = bigint();
   if ( this->negative == that.negative ){
      do_bigadd(this->big_value, that.big_value, result.big_value);
      result.set_negative(that.negative);
   }else {
      int abs_compare = abscompare(that);
      if ( abs_compare > 0 ){
         do_bigsub(this->big_value, that.big_value, result.big_value);
         result.set_negative(negative);
      }else if ( abs_compare < 0 ){
         do_bigsub(that.big_value, this->big_value, result.big_value);
         result.set_negative(that.negative);
      }
   }
   result.zero_positive_check();
   return result;
}

bigint bigint::operator- (const bigint &that) const {
   bigint result = bigint();
   if ( negative == that.negative ){
      int abs_compare = abscompare(that);
      if ( abs_compare > 0 ){
         do_bigsub(big_value, that.big_value, result.big_value);
         result.set_negative(this->negative?true:false);
      }else if ( abs_compare < 0){
         do_bigsub(that.big_value, big_value, result.big_value);
         result.set_negative(negative?false:true);
      }
   }else {
      do_bigadd( big_value, that.big_value, result.big_value);
      if ( negative ) result.set_negative(negative);
   }
   result.zero_positive_check();
   return result;
}

bigint bigint::operator- () const {
   bigint result = *this;
   result.set_negative(!negative);
   return result;
}

int bigint::compare (const bigint &that) const {
   if ( negative == false && that.negative == true )
      return 1;
   else if ( negative == true && that.negative == false )
      return -1;
   bool sign = negative;
   unsigned long this_size, that_size;
   this_size = big_value->size();
   that_size = that.big_value->size();
   if ( this_size > that_size ){
      if( sign == false )
         return 1;
      else
         return -1;
   }else if ( this_size < that_size ){
      if( sign == false )
         return -1;
      else
         return 1;
   }
   else { //equal size vectors
      bigvalue_t::const_reverse_iterator itor_this = 
         big_value->rbegin();
      bigvalue_t::const_reverse_iterator end_this = 
         big_value->rend();
      bigvalue_t::const_reverse_iterator itor_that = 
         that.big_value->rbegin();
      bigvalue_t::const_reverse_iterator end_that = 
         that.big_value->rend();
      while ( itor_this != end_this && itor_that != end_that ){
         if ( *itor_this > *itor_that ){
            if ( sign == false ) return 1;
            else return -1;
         }else if ( *itor_this < *itor_that ){
            if ( sign == false ) return -1;
            else return 1;
         }
         ++itor_this;
         ++itor_that;
      }
   }
   //EQUAL!!!
   return 0;
}

int bigint::abscompare (const bigint &that) const {
   unsigned long this_size, that_size;
   bigvalue_t::const_reverse_iterator itor_this = big_value->rbegin();
   bigvalue_t::const_reverse_iterator end_this = big_value->rend();
   bigvalue_t::const_reverse_iterator itor_that =
      that.big_value->rbegin();
   bigvalue_t::const_reverse_iterator end_that =
      that.big_value->rend();
   this_size = big_value->size();
   that_size = that.big_value->size();
   if ( this_size > that_size )
      return 1;
   else if ( this_size < that_size )
      return -1;
   else {
      while ( itor_this != end_this && itor_that != end_that ){
         if ( *itor_this > *itor_that ) return 1;
         else if ( *itor_this < *itor_that ) return -1;
         ++itor_this;
         ++itor_that;
      }
   }
   return 0;
}

int bigint::smallint () const {
   if (*this < numeric_limits<int>::min()
    || *this > numeric_limits<int>::max())
               throw range_error ("smallint: out of range");
   int small = 0;
   bigvalue_t::const_iterator itor = big_value->begin();
   bigvalue_t::const_iterator end = big_value->end();
   for ( int i = 1; itor != end; ++itor, i*=10 ){
      small += i * (*itor);
   }
   if ( negative ) small = -small;
   return small;
}

bigint bigint::mul_by_2 () {
   bigint result = bigint();
   do_bigadd(big_value,big_value,result.big_value);
   return result;
}

//a is bigger than b
void bigint::do_bigadd(const bigvalue_t* a,
                       const bigvalue_t* b,
                       bigvalue_t* target) const{
   int carry = 0;
   bool a_done = false, b_done = false;
   bigvalue_t::const_iterator itor_a = a->begin();
   bigvalue_t::const_iterator end_a = a->end();
   bigvalue_t::const_iterator itor_b = b->begin();
   bigvalue_t::const_iterator end_b = b->end();
   if ( a->size() == 0 ) a_done = true;
   if ( b->size() == 0 ) b_done = true;
   for(;;){
      int sum = (b_done?0:*itor_b) + (a_done?0:*itor_a) + carry;
      TRACE('b', char(*itor_a+'0') << " + "<<char(*itor_b+'0')<<
            " + "<<carry<< " = " << sum);
      carry = 0;
      if ( sum >= 10 ) {
         carry = 1;
         sum -= 10;
         TRACE('b',"   Accounting for >10, diff - 10 = "
            << sum);
      }
      target->push_back(sum);
      if( ++itor_a == end_a ) a_done = true;
      if ( ++itor_b == end_b ) b_done = true;
      
      if ( a_done && b_done ){
         if ( carry > 0 ) target->push_back(carry);
         break;
      }
   }
   remove_lead_0s(target);
}

//a is bigger than b
void bigint::do_bigsub(const bigvalue_t* a,
                       const bigvalue_t* b,
                       bigvalue_t* target) const{
   int carry = 0;
   bool a_done = false, b_done = false;
   bigvalue_t::const_iterator itor_a = a->begin();
   bigvalue_t::const_iterator end_a = a->end();
   bigvalue_t::const_iterator itor_b = b->begin();
   bigvalue_t::const_iterator end_b = b->end();
   if ( a->size() == 0 ) a_done = true;
   if ( b->size() == 0 ) b_done = true;
   for(;;){
      int diff = (a_done?0:*itor_a) - (b_done?0:*itor_b) + carry;
      TRACE('b', char(*itor_a+'0') << " - "<<char(*itor_b+'0')<<
            " + "<<carry<< " = " << diff);
      carry = 0;
      if ( diff < 0 && !a_done ) {
         carry = -1;
         diff += 10;
         TRACE('b',"   Accounting for negative, diff + 10 = "
            << diff);
      }
      target->push_back(diff);
      if( !a_done && ++itor_a == end_a ) a_done = true;
      if ( !b_done && ++itor_b == end_b ) b_done = true;
      if ( a_done && b_done )break;
   }
   remove_lead_0s(target);
}

void bigint::remove_lead_0s(bigvalue_t* value) const{
   bigvalue_t::const_reverse_iterator itor = value->rbegin();
   bigvalue_t::const_reverse_iterator end = value->rend();
   while ( *itor == 0 && itor != end){
      ++itor;
      value->pop_back();
   }
}

void bigint::set_negative(bool positivity){
   negative = positivity;
}

void bigint::zero_positive_check(){
   if ( big_value->size() == 0 )
      negative = false;
}

static bigpair popstack (stack <bigpair> &egyptstack) {
   bigpair result = egyptstack.top ();
   egyptstack.pop();
   return result;
}

//
// Ancient Egyptian multiplication algorithm.
//
bigint bigint::operator* (const bigint &that) const {
   bigint left = *this;
   bigint top = that;
   bigint count = 1;
   TRACE ('*', *this << " * " << that);
   stack <bigpair> egyptstack;
   while ( count <= *this ){
      TRACE('*',"Pushing: "<<count<<" & "<<top);
      egyptstack.push(bigpair(count,top));
      top = top.mul_by_2();
      count = count.mul_by_2();
   }
   bigint result;
   while ( !egyptstack.empty() ){
      bigpair pair = popstack(egyptstack);
      TRACE('*',"Popping: "<<pair.first<<" & "<<pair.second);
      if ( pair.first <= left ){
         left = left - pair.first;
         result = result + pair.second;
      }
   }
   if ( negative != that.negative )
      result.set_negative(true);
   return result;
}

//
// Ancient Egyptian division algorithm.
//
bigpair bigint::div_rem (const bigint &that) const {
   if (that == 0) throw range_error ("divide by 0");
   TRACE ('/', *this << " /% " << that);
   stack <bigpair> egyptstack;
   bigint quotient("0"), remainder = *this;
   bigint top = bigint("1"), right = that;
   while ( right <= *this ){
      egyptstack.push(bigpair(top,right));
      top = top.mul_by_2();
      right = right.mul_by_2();
   }
   while ( !egyptstack.empty() ){
      bigpair pair = popstack(egyptstack);
      if ( pair.second <= remainder ){
         remainder = remainder - pair.second;
         quotient = quotient + pair.first;
      }
   }
   return bigpair (quotient, remainder);
}

bigint bigint::operator/ (const bigint &that) const {
   return div_rem (that).first;
}

bigint bigint::operator% (const bigint &that) const {
   return div_rem (that).second;
}


#define TRACE_POW TRACE ('^', "result: " << result \
                  << ", base: " << base << ", expt: " << expt);
bigint bigint::pow (const bigint &that) const {
   bigint base = *this;
   if (that > 999) throw range_error ("exp too big");
   int expt = that.smallint();
   bigint result = 1;
   TRACE_POW;
   if (expt < 0) {
      base = 1 / base;
      expt = - expt;
   }
   while (expt > 0) {
      TRACE_POW;
      if (expt & 1) { //odd
         result = result * base;
         --expt;
      }else { //even
         base = base * base;
         expt /= 2;
      }
   }
   TRACE_POW;
   return result;
}

//
// Macros can make repetitive code easier.
//

#define COMPARE(OPER) \
   bool bigint::operator OPER (const bigint &that) const { \
      return compare (that) OPER 0; \
   }
COMPARE (==)
COMPARE (!=)
COMPARE (< )
COMPARE (<=)
COMPARE (> )
COMPARE (>=)

#define INT_LEFT(RESULT,OPER) \
   RESULT operator OPER (int left, const bigint &that) { \
      return bigint (left) OPER that; \
   }
INT_LEFT (bigint, +)
INT_LEFT (bigint, -)
INT_LEFT (bigint, *)
INT_LEFT (bigint, /)
INT_LEFT (bigint, %)
INT_LEFT (bool, ==)
INT_LEFT (bool, !=)
INT_LEFT (bool, < )
INT_LEFT (bool, <=)
INT_LEFT (bool, > )
INT_LEFT (bool, >=)

ostream &operator<< (ostream &out, const bigint &that) {
   bigint::bigvalue_t::reverse_iterator itor = that.big_value->rbegin();
   bigint::bigvalue_t::const_reverse_iterator end = 
      that.big_value->rend();
   if ( that.negative == true ) out << "_";
   if ( itor != end ){
      for (int i = 1; itor != end; ++itor, ++i ){
         out << char(*itor+'0');
         if ( i == 69 ){
            out << "\\\n";
            i=0;
         }
      }
   } else
      out << '0';
   return out;
}

