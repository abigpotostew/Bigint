// $Id: main.cpp,v 1.87 2012-04-30 12:56:27-07 - - $

/* 
 * B Stewart Bracken
 * bbracken@ucsc.edu
 */

#include <deque>
#include <exception>
#include <map>
#include <iostream>
#include <utility>
#include <sstream>

using namespace std;

#include "bigint.h"
#include "iterstack.h"
#include "util.h"
#include "scanner.h"
#include "trace.h"

typedef iterstack<bigint> bigint_stack;

#define DO_BINOP(FN_NAME,TFLAG,OPER) \
   void FN_NAME (bigint_stack &stack) { \
      if (stack.size()>1){\
         bigint right = stack.top(); \
         stack.pop(); \
         TRACE (TFLAG, "right = " << right); \
         bigint left = stack.top(); \
         stack.pop(); \
         TRACE (TFLAG, "left = " << left); \
         bigint result = left OPER (right); \
         TRACE (TFLAG, "result = " << result); \
         stack.push (result); \
         }\
         else throw ydc_exn("stack empty");\
   }
DO_BINOP(do_add, '+', +   )
DO_BINOP(do_sub, '-', -   )
DO_BINOP(do_mul, '*', *   )
DO_BINOP(do_div, '/', /   )
DO_BINOP(do_rem, '%', %   )
DO_BINOP(do_pow, '^', .pow)

void do_clear (bigint_stack &stack) {
   TRACE ('c', "");
   stack.clear();
}

void do_dup (bigint_stack &stack) {
   bigint top = stack.top();
   TRACE ('d', top);
   stack.push (top);
}
 
void do_printall (bigint_stack &stack) {
   bigint_stack::const_reverse_iterator itor = stack.rbegin();
   bigint_stack::const_reverse_iterator end = stack.rend();
   for (; itor != end; ++itor) cout << *itor << endl;
}

void do_print (bigint_stack &stack) {
   if ( !stack.empty() )
      cout << stack.top() << endl;
   else
      throw ydc_exn("stack empty");
}

void do_debug (bigint_stack &stack) {
   (void) stack; // SUPPRESS: warning: unused parameter 'stack'
   cout << "Y not implemented" << endl;
}


class ydc_quit: public exception {};
void do_quit (bigint_stack &stack) {
   (void) stack; // SUPPRESS: warning: unused parameter 'stack'
   throw ydc_quit ();
}

typedef void (*function) (bigint_stack&);
typedef map <string, function> fnmap;
fnmap load_fn () {
   fnmap functions;
   functions["+"] = do_add;
   functions["-"] = do_sub;
   functions["*"] = do_mul;
   functions["/"] = do_div;
   functions["%"] = do_rem;
   functions["^"] = do_pow;
   functions["Y"] = do_debug;
   functions["c"] = do_clear;
   functions["d"] = do_dup;
   functions["f"] = do_printall;
   functions["p"] = do_print;
   functions["q"] = do_quit;;
   return functions;
}

//
// scan_options
//    Options analysis:  The only option is -Dflags. 
//

void scan_options (int argc, char **argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            traceflags::setflags (optarg);
            break;
         default:
            complain() << "-" << (char) optopt << ": invalid option"
                       << endl;
            break;
      }
   }
   if (optind < argc) {
      complain() << "operand not permitted" << endl;
   }
}
int main (int argc, char **argv) {
   sys_info::set_execname (argv[0]);
   scan_options (argc, argv);
   fnmap functions = load_fn();
   bigint_stack operand_stack = bigint_stack();
   
   
   /*bigint small = bigint("1000000000000001");
   cout<<"Small = "<<small<<endl;
   int smint = small.smallint();
   cout<<"smint = " << smint << " smint+1 = "<<smint+1<<endl;*/
   
   
   
   scanner input;
   
   try {
      for (;;) {
         try {
            token_t token = input.scan();
            if (token.symbol == SCANEOF) break;
            switch (token.symbol) {
               case NUMBER:
                  operand_stack.push (bigint(token.lexinfo));
                  break;
               case OPERATOR: {
                  function fn = functions[token.lexinfo];
                  if (fn == NULL) {
                     throw ydc_exn (octal (token.lexinfo[0])
                                    + " is unimplemented");
                  }
                  fn (operand_stack);
                  break;
                  }
               default:
                  break;
            }
         }catch (ydc_exn &exn) {
            cout << "ydc: "<< exn.what() << endl;
         }
      }
   }catch (ydc_quit &) {
      // Intentionally left empty.
   }
   return sys_info::get_status ();
}

