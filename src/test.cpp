#include <iostream>
#include "plumb.hpp"
#include "test.hpp"
#include "read.hpp"
#include "langtest.hpp"

using namespace std;

struct PlusTwo: public Funk<int, int> {
  int act(int n){
    return n+2;
  }
};

struct Prov7: public Input<int> {
  int give(){
    return 7;
  }
};

struct StrMult: public DeltaPipe<int, string> {

  string act(int n){
    string ans = "";
    for (int i=0; i<n; i++)
      ans += 'A';
    return ans;
  }
  
};



int main() 
{
  TestUnit u("plumb");
  
  PlusTwo p2;
  u.check(p2.act(5) == 7, "5 + 2 = 7");

  Prov7 p7;
  u.check(p7.give() == 7, "p7 gives a 7");

  StorePipe<int> oot;
  oot.in = &p7;
  u.check(oot.give() == 7, "oot pulls a 7");
  u.check(oot.value == 7, "oot stores a 7");

  Catch<int> ctch;
  oot.out = &ctch;
  oot.get(4);
  u.check(oot.value == 4, "oot gets a 4");
  u.check(ctch.value == 4, "Catching a 4 from oot");

  StrMult aaa;
  attachPipes(&oot, &aaa);
  u.check(aaa.give() == "AAAAAAA", "AAAAAAA");
  u.check(aaa.give() != "AAAAAAAb", "! AAAAAAAb");
  u.summary();

  doit2();

  return 0;
}
