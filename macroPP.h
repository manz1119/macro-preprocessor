
#ifndef MACROPP_H
#define MACROPP_H


#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <ctype.h>
#include "file_parser.h"

using namespace std;

class macroPP {
    public:
    	//ctor
    	macroPP(string);

			//dtor
			~macroPP();

      void get_expansion(int);

      void get_macros();

			void print_macros();

      void write();

      int size();

      int size_mincludes();

      void consolidate(macroPP);

      void get_minclude_macros(string);



    private:
      file_parser *fp;
      file_parser *ft;
      //file_parser *fp_temp; //file_parser for #minclude file
      int number_of_macros1;
      int number_of_macros2;

			struct macro{
				string macro_name;
				vector<string> storage;
        vector<string> parameters;
        int invo;
				int start_pos;
				int end_pos;
        bool has_size;
			};

      map<string, macro> user_macro_expansions;
      map<string, macro> minclude_macro_expansions;
      void add_macro(string, int, int);
      void add_minclude_macro(string, int, int);
      //map<string, macro> returnMap();

};
#endif
