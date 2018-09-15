#include <iostream>
#include "plumb.hpp"
#include "test.hpp"

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
  PlusTwo p2;
  check(p2.act(5) == 7, "5 + 2 = 7");

  Prov7 p7;
  check(p7.give() == 7, "p7 gives a 7");

  StorePipe<int> oot;
  oot.in = &p7;
  check(oot.give() == 7, "oot pulls a 7");
  check(oot.value == 7, "oot stores a 7");

  Catch<int> ctch;
  oot.out = &ctch;
  oot.get(4);
  check(oot.value == 4, "oot gets a 4");
  check(ctch.value == 4, "Catching a 4 from oot");

  StrMult aaa;
  attachPipes(&oot, &aaa);
  check(aaa.give() == "AAAAAAA", "AAAAAAA");
  check(aaa.give() != "AAAAAAAb", "! AAAAAAAb");
  
  return 0;
}
