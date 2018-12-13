#include <fstream>

string readF(string fname){
  ifstream rf;
  rf.open(fname.c_str());

  if (! rf.is_open())
    throw runtime_error("Could not open read file.");

  string line, tot="";
  while(! (getline(rf, line)).eof()){
    tot += line+'\n';
  } tot += line+'\n';
  rf.close();
  return tot;
}

void writeF(string fname, string data){
  ofstream wf;
  wf.open(fname.c_str());
  if (! wf.is_open())
    throw runtime_error("Could not open write file.");

  wf.write(data.c_str(), data.length());
  
  wf.close();
  return;
}

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
