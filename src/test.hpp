using namespace std;

// crappy test suite
struct TestUnit {
  int tests, passes;
  string name;
  TestUnit(string n):passes(0), tests(0), name(n){}
    
  
  void check(bool t, string msg){
    tests++;
    if (t) {passes++;}
    cout << msg << ": " << (t?"TRUE":"FALSE") << endl;
  }

  void summary(){
    cout << "[" << name << "]: " << tests << " tests. " << passes << " passed." << endl;
  }
};
