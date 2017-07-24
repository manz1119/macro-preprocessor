CC=g++
CXXFLAGS=-g -O3 -Wall

macroPP:	file_parser.o	macroPP.o
		${CC} -o macroPP macroPP.o file_parser.o

macroPP.o:	macroPP.h macroPP.cpp
		${CC} -c macroPP.cpp

file_parser.o:	file_parser.h	file_parser.cc	file_parse_exception.h
	$(CC)   -c file_parser.cc

deo:
	rm -rf *.o

clean:
	rm -rf $(EXEC) *.o

spotless:
	rm -rf $(EXEC) *.o *core*
