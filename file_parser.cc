#include "file_parser.h"

using namespace std;

        // ctor, filename is the parameter.  A driver program will read
        // the filename from the command line, and pass the filename to
        // the file_parser constructor.  Filenames must not be hard-coded.
file_parser::file_parser(string s){
	filename = s;
        number_of_lines =   0; 
}

        // dtor
file_parser::~file_parser(){}

        // reads the source file, storing the information is some
        // auxiliary data structure you define in the private section.
        // Throws a file_parse_exception if an error occurs.
        // if the source code file fails to conform to the above
        // specification, this is an error condition.
void file_parser::read_file(){
            ifstream input;
            string line;
            input.open(filename.c_str(),ios::in);
            if(!input) throw file_parse_exception("Could not open file.");
            while(!input.eof()) {
                getline(input, line);
            		storage.push_back(make_tokens(line));
                number_of_lines++;
            }
            input.close();
}

file_parser::tokens file_parser::make_tokens(string line){
      	const string delim = " �\t\n";
      	tokens row;
	      row = init_token(row);

        string temp;
	      int source_line = number_of_lines + 1;

        int commentlocation = line.find('*');
        int quotelocation = line.find("'");
        int quotelocation2;

        //tokenizing comment and creating new substr without comment
        if(commentlocation != -1){
            if((quotelocation != -1) && (quotelocation < commentlocation)){
              quotelocation2 = line.find_first_of("'", quotelocation+1);
              if(quotelocation2 == -1){
	      		       stringstream s;
			             s << "bad syntax, error in line: " << source_line;
			             throw file_parse_exception(s.str());
              }
              commentlocation = line.find_first_of("*", quotelocation2+1);
              if(commentlocation != -1){
                row.comment = line.substr(commentlocation);
                line = line.substr(0, commentlocation);
              }
            }
            else{
              row.comment = line.substr(commentlocation);
              line = line.substr(0, commentlocation);
              if(line.length() < 2)
                return row;
            }
        } //end of comment tokenzing


        //handling quotes if they exist
        if(quotelocation != -1){
          while(line[quotelocation] != ' ' && line[quotelocation] != '\t'){
            quotelocation = quotelocation-1;
          }
          row.operand = line.substr(quotelocation);
          line = line.substr(0, quotelocation);
        }

	if(line.find(":") != string::npos){
		string::size_type beg = line.find_first_not_of(delim, 0);
		string::size_type col = line.find_first_of(":", beg);
		string label = line.substr(0, col+1);
		for(int i =0; i<label.length(); i++){
			if(isalnum(line[i]) == 0 && line[i] != ':'){
				if(line[i+1] == '@')
          				i = line.length();
        			else{
					stringstream s;
					s << "bad syntax, error in label on line: " << source_line;
					throw file_parse_exception(s.str());
					}
				}
			}
		row.label = label;
		line = line.substr(col+1);

		}


	if(line[0] == '#'){
		string::size_type fin = line.find_first_of(" \t", 2);
		row.label = line.substr(0, fin);
		row.opcode = line.substr(fin+1);
		return row;
		}


	if(isalpha(line[0]) && line.find(":") != -1){
		for(int i =0; i<line.length(); i++){
			if(isalnum(line[i]) == 0 && line[i] != ':'){
        			if(line[i+1] == '@')
          				i= line.length();
        			else{
					stringstream s;
					s << "bad syntax, error in label: " << source_line;
					throw file_parse_exception(s.str());
					}
			}
		}
		row.label = line;
		return row;
	}

	else if(line[0] == '.' || isalpha(line[0]) ){
		//cout << "got to "<< number_of_lines << endl;
		int ndelim = line.find_first_not_of(delim,0);
    		int of = line.find_first_of(delim, ndelim);

		string temp = line.substr(ndelim, of-ndelim);
		row.label = temp;
		line = line.substr(of+1);
	}

	//regular opcode/operand parsing
	int pass = 0;
	string::size_type last = line.find_first_not_of(delim, 0);
	string::size_type first = line.find_first_of(delim, last);

	while(string::npos != first || string::npos != last){
		string str = line.substr(last, first-last);
		if(pass == 0)
			row.opcode = str;

		if(pass == 1)
			row.operand = str;


		last = line.find_first_not_of(delim, first);
		first = line.find_first_of(delim, last);
		pass++;
		}//end while

		if(pass > 2){
			stringstream s;
			s << "bad syntax, too many tokens on line: " << source_line;
			throw file_parse_exception(s.str());
			}

		return row;


        //you can put operand tokenizer right here for the case of no quote location
      }//end of method make_tokens


file_parser::tokens file_parser::init_token(tokens t){
			t.label = "	";
			t.opcode = "	";
			t.operand = "	";
			t.comment = "	";

			return t;
}

             // returns the token found at (row, column).  Rows and columns
            // are zero based.  Returns the empty string "" if there is no
            // token at that location. column refers to the four fields
            // identified above.
string file_parser::get_token(unsigned int row, unsigned int col){
    	if(row > number_of_lines || col > 3)
    		throw file_parse_exception("Error: Row must not exceed number of line, Column must not exceed 3.");

    	if(col == 0)
    		return storage[row].label;

    	else if(col == 1)
    		return storage[row].opcode;

    	else if(col == 2)
    		return storage[row].operand;

    	else if(col == 3)
    		return storage[row].comment;
}

      // prints the source code file to stdout.  Should not repeat
      // the exact formatting of the original, but uses tabs to align
      // similar tokens in a column. The fields should match the
      // order of token fields given above (label/opcode/operands/comments)
void file_parser::print_file(){
                for(unsigned int i = 0; i < storage.size(); i++){
      	  	cout << storage[i].label + " " + storage[i].opcode + " " + storage[i].operand + " " + storage[i].comment <<endl;
      	  	}
}

      //returns a tokenized line
string file_parser::return_line(int i){
	string file_contents;
	  file_contents += storage[i].label + " " + storage[i].opcode + " ";
          file_contents += storage[i].operand + " " + storage[i].comment +"\n";
          if(storage[i].label == "	" && storage[i].opcode == "	"){
            if(storage[i].operand == "	")
              file_contents = storage[i].comment +"\n";
          }
      	  return file_contents;
      }

      string file_parser::get_filename(){
        return filename;
      }

      // returns the number of lines in the source code file
      int file_parser::size(){
                  return number_of_lines;
}
