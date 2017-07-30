
#ifndef MACROPP_ERROR_EXCEPTION_H
#define MACROPP_ERROR_EXCEPTION_H
#include <string>

using namespace std;

	class macroPP_error_exception {

  public:
  macroPP_error_exception(string s) {
		message = s;
	}

	macroPP_error_exception() {
		message = "An error has occurred on line: %d\n", __LINE__;
	}

	string getMessage() {
		return message;
	}

private:
	string message;
};

#endif
