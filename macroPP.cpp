
#include "macroPP.h"
#include "file_parser.h"
#include <string>
#include <iostream>
#include <fstream>
#include <map>

using namespace std;

//constructor
macroPP::macroPP(string s) {
	number_of_macros1 = 0;
	number_of_macros2 = 0;
	fp = new file_parser(s);
	fp->read_file();
}

//destructor
macroPP::~macroPP(){}

int macroPP::size(){
	return number_of_macros1;
}

int macroPP::size_mincludes(){
	return number_of_macros2;
}


//writes and creates .se
//with macro expansions removed
void macroPP::write(){
	string b = fp->get_filename() + "e";
	ofstream output_file;
	output_file.open(b.c_str());

	for(int i =0; i<fp->size(); i++) {

		string name = fp->get_token(i,1);
		string temp;   											//for swapping \ with parameters

		//PART 1 of the write loop
		//this part of the code removes the #mincludes
		//and the user defined macro definitions
		if(fp->get_token(i,0)=="#minclude")
			i++;
		else if(fp->get_token(i,1).find("macro") != -1) {
			for(int k = i; k < fp->size(); k++){
				if(fp->return_line(k).find("endm") != -1)
					i = k;
			}
    }


		//PART 2.1 looks for a user defined macro to expand
		else if(user_macro_expansions.find(name) != user_macro_expansions.end()){
			output_file << "*" + (fp->return_line(i));
			string temp2 = fp->return_line(i);
			const string delim = " ,ï¿½\t\n";
			string opcode, operand;

			string::size_type last = temp2.find_first_not_of(delim , 0);
			string::size_type first = temp2.find_first_of(delim, last);

			opcode = temp2.substr(last, first-last);
			operand = temp2.substr(first);

			int where = opcode.find(".");
			if(where != -1){
				user_macro_expansions[name].parameters.push_back(opcode.substr(where+1));
				user_macro_expansions[name].has_size = true;
			}
			else{
				user_macro_expansions[name].parameters.push_back("NULL");
				user_macro_expansions[name].has_size = false;
			}

			last = operand.find_first_not_of("," , 0);
			first = operand.find_first_of("," , 0);

			while(string::npos != first || string::npos != last){
				string str = operand.substr(last, first-last);
				user_macro_expansions[name].parameters.push_back(str);
				last = operand.find_first_not_of(",	" , first);
				first = operand.find_first_of(",	" , last);
				}

			for(int j = 0; j<user_macro_expansions[name].storage.size(); j++){
				temp = user_macro_expansions[name].storage[j];
				//replaces \@s
				int pos = temp.find("\@");
				pos--;
				if(pos > -1){
					stringstream s;
					s << temp.substr(0, pos);
					s << user_macro_expansions[name].invo;
					s << temp.substr(temp.find("\@")+1);
					temp = s.str();
				}

				//replaces parameters or \numericals
				for(int k = 0; k < user_macro_expansions[name].parameters.size(); k++){
					stringstream s;
					if(!user_macro_expansions[name].has_size && k ==0)
						k++;
					s << "\\";
					s << k;
					pos = temp.find(s.str());
					s.str("");
					if(pos > -1){
						s << temp.substr(0, pos);
						s << user_macro_expansions[name].parameters[k];
						s << temp.substr(pos+2);
						temp = s.str();
						s.str("");
					}
				}

				output_file << temp;
			}

			output_file << "*   END OF MACRO EXPANSION" << "\n";
			user_macro_expansions[name].invo = user_macro_expansions[name].invo +1;
		}


		//PART 3 doesn't necissarilly come after PART 2
		//it writes lines to the .se file that don't
		//need special handling
		else
    	output_file << (fp->return_line(i));
  }
  output_file.close();
}


//storing macro definition into vector<string> inside struct macro
void macroPP::add_macro(string name, int sp, int ep){
	macro temp;
	temp.invo = 0;
	temp.macro_name = name;
	temp.start_pos = sp;
	temp.end_pos = ep;
	for(int i =sp+1; i<ep; i++){
		temp.storage.push_back(fp->return_line(i));
	}
	user_macro_expansions[name] = temp;
	number_of_macros1++;
}

//storing macro definition into vector<string> inside struct macro
void macroPP::add_minclude_macro(string name, int sp, int ep){
	macro temp;
	temp.invo = 0;
	temp.macro_name = name;
	temp.start_pos = sp;
	temp.end_pos = ep;
	for(int i =sp+1; i<ep; i++){
		temp.storage.push_back(ft->return_line(i));
	}
	user_macro_expansions[name] = temp;
	number_of_macros1++;
}


void macroPP::get_minclude_macros(string file){
	ft = new file_parser(file);
	ft->read_file();
	int max = ft->size();
	string mac_name;
	int start = -1;
	int end = -1;

	//interates through .s file looking for macros and mincludes
	for(int i=0; i < max; i++){

		//stores macros from user defined macros in .s file
		if(ft->get_token(i,1)=="macro" && start == -1)
			start = i;
		if(ft->get_token(i,1)=="endm" && start != -1){
			end = i;
			mac_name = ft->get_token(start,0);
			add_minclude_macro(mac_name, start, end);
			start = -1;
		}
	}
}
//end of methods
//and their definitions

//iterates through tokenized file
//looks for start pos and end pos
//stores macro by calling add_macro(name of macro, start pos, end pos)
void macroPP::get_macros(){
	int max = fp->size();
	string mac_name;
	int start = -1;
	int end = -1;

	//interates through .s file looking for macros and mincludes
	for(int i=0; i < max; i++){

		//stores macros from user defined macros in .s file
		if(fp->get_token(i,1)=="macro" && start == -1)
			start = i;
		if(fp->get_token(i,1)=="endm" && start != -1){
			end = i;
			mac_name = fp->get_token(start,0);
			add_macro(mac_name, start, end);
			start = -1;
		}
	}
}
//end of methods
//and their definitions


//start of main method
//tokenizes file
//main will then iterate through tokenized file
//then call macroPP to store macros
int main(int argc, char **argv){
	try{
		if(argc != 2)  {
			cout << "Usage: macroPP FILENAME" << endl;
			return 1;
		}

		vector<string> minfilename;
		macroPP pp(argv[1]);

		file_parser fparser(argv[1]);
		fparser.read_file();
		for(int i = 0; i < fparser.size(); i++){
			if(fparser.get_token(i,0) == "#minclude")
				minfilename.push_back(fparser.get_token(i,1));
		}
		pp.get_macros();

		for(int i = 0; i < minfilename.size(); i++){
			pp.get_minclude_macros(minfilename[i]);
		}

		pp.write();
		return 0;
	}
	catch(file_parse_exception e){
		cout << e.getMessage() << endl;
	}
}
