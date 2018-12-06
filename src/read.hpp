#include <fstream>

//struct to read file char by char
struct CharReader {
  ifstream stream;
  int line, col; //a consumer should read these before a get to see where the next char is coming from
  
  CharReader(string fname): line(0), col(0){
    stream.open(fname.c_str());
    if (! stream.is_open()){
      throw runtime_error("Could not open write file. [CharReader]");
    }
  }

  char get(){
    char c;
    stream.get(c);
    if (c == '\n') {line++; col=0;}
    else col++;
    return c;
  }

  bool isEof(){
    bool ans = stream.eof();
    if (ans) {stream.close();}
    return ans;
  }
};
