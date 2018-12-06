void doit(string fname){

  try {
    CharReader rdr(fname);
    int line, col;

    cout << "here" << endl;
    while(!rdr.isEof()){
      line = rdr.line;
      col = rdr.col;
      cout << line << ' ' << col << endl;
      char c = rdr.get();
      cout << c << '|';
    }
  }
  catch (exception e){
    cout << "could not read file" << endl;
  }
}
