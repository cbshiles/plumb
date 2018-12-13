#include <stack>
//Each quote-enclosed area is known as a context

enum ContextType { core, macro, single, dub };


//The rule is - the boundary itself is not passed on to the context.
//It is still accounted for, however in positioning of the code chunk
//The positioning info should remain external to the Context class itself,
//part of a wrapper for each Context. The Context can throw an error @ whereever local
//to it, and the wrapper translates to global positioning.

//should be part of plumb, not lang
struct FileIn: public Input<string> {
  string fname;
  
  FileIn(string name): fname(name){}

  string give() {return readF(fname);}
};

/* defines a character's position in a file,
by # of characters in and by a line # & position on line 
the idea is to make it easy for error messages to refer to the correct spot in code
*/
struct CharSpot {
  CharSpot(): n(0L), line(1L), linen(0L), fresh(true), lastWasNewline(false){
  }
  long n, line, linen;
  bool fresh, lastWasNewline;
  void feed(char c){
    if (fresh){
      fresh = false;
      return;
    }
    
    n++;
    if(lastWasNewline){
      line++;
      linen=1L;
    } else {
      linen++;
    }
    lastWasNewline = c == '\n';
  }
};

struct Context {
  vector<Context*> subs;
  CharSpot start;
  ContextType ctype;
  long charLen, lineLen;
  bool fresh;
  
  Context(ContextType ct):
    ctype(ct), fresh(true)
  {}

  int read(char c, CharSpot cs){
    if (fresh){
      start = cs;
      fresh = false;
    }
    return interpret(c);
  }

  virtual int interpret(char c) = 0;

  virtual string typeName() = 0;
  
  void close(CharSpot last){
    charLen = last.n - start.n;
    lineLen = last.line - start.line;
  }
};

struct CoreContext: public Context {
  CoreContext(): Context(core){}

  string typeName(){
    return "CORE";
  }
  
  int interpret(char c){
    // you could order these based on frquency of special char
    if (c == '`')
      return 1;
    if (c == '\'')
      return 2;
    if (c == '"')
      return 3;
    return 0;
  }
};

struct MacroContext: public Context {
  MacroContext(): Context(macro){}

  string typeName(){
    return "MACRO";
  }

  int interpret(char c){
    if (c == '`')
      return -1;
    if (c == '\'')
      return 2;
    if (c == '"')
      return 3;
    return 0;
  }
};

struct SingleContext: public Context {
  SingleContext(): Context(single){}
  int interpret(char c){
    return c == '\'' ? -1 : 0;
  }

    string typeName(){
      return "SINGLE";
  }

};

struct DubContext: public Context {
  DubContext(): Context(dub), lastWasSlash(false){}
  bool lastWasSlash;
  int interpret(char c){
    int res = (!lastWasSlash && c == '"')? -1 : 0;
    lastWasSlash = c == '\\';
    return res;
  }

    string typeName(){
      return "DOUBLE";
  }

};

struct ContextMap {
  ContextMap(string fc, Context* txt): fileContent(fc), rootContext(txt){}
  string fileContent;
  Context* rootContext;

  //private: (so contexts from other maps aren't being used)
  string contextContent(Context* ctxt){
    return fileContent.substr(ctxt->start.n, ctxt->charLen);
  }
};

struct ContextError {
  string msg;
  CharSpot spot;

  ContextError(string gsm, CharSpot tops): msg(gsm), spot(tops){}

  string errMsg(){
    return to_string(spot.line)+":"+to_string(spot.linen)+": "+msg;
  }
};

struct ContextPipe: public DeltaPipe<string, ContextMap*>{
  ContextMap* act(string fileStr){
    CharSpot cspot;
    Context* root = new CoreContext();
    ContextMap* cmap = new ContextMap(fileStr, root);

    Context* currCtxt = root;
    int i=0, end=fileStr.length();
    stack<Context*> contextStack; //keep track of nested contexts
    while(i<end){
      char c = fileStr[i];
      cspot.feed(c);
    //feed currCtxt a character & the up to date CharSpot
      int res = currCtxt->read(c, cspot);
      if (res < 0){
	currCtxt->close(cspot);
	currCtxt = contextStack.top();
	contextStack.pop();
      }
      else if (res > 0){
	Context* sub;
	switch(res){
	  
	case 1: //macro
	  sub = new MacroContext();
	  break;
	case 2: //single
	  sub = new SingleContext();
	  break;
	case 3: //dub
	  sub = new DubContext();
	  break;
	}
	currCtxt->subs.push_back(sub);
	contextStack.push(currCtxt);
	currCtxt = sub;
      }
      i++;
    }
    //need to check if stack is empty
    if (contextStack.size() != 0){
      Context* unclosed; //should really use all contexts still on stack
      throw new ContextError("Stacks fulla shit!", cspot);
    }
    root->close(cspot);
    //let it return a # indicating exiting currCtxt (-1), no change (0), or a sub of type (1-4)
    
    /* So, we've finally got to the actual coding...
       we'll need a function that builds the tree, while looking at
       the current context & feeding in the chars (more or less)

       also, have a CharSpot that you're keeping up to date. Since its an
       object and not a pointer, i think i can just copy it?
     */

    return cmap;
  }
};

ContextMap* f2cmap(string fname){
  FileIn fin(fname);
  ContextPipe cpipe;
  cpipe.in = &fin;
  return cpipe.give();
}

void doit2(){
  TestUnit g("lang");

  ContextMap* cmap = f2cmap("test/uno.carl");
  //cout << cmap->fileContent << endl;
  Context* root = cmap->rootContext;
  g.check(root->subs.size() == 3, "3 sub-contexts");

  Context* dubCtxt = root->subs[0];
  g.check(cmap->contextContent(dubCtxt) == "double quote \\\"area\\\"", "dub context");
  
  Context* macroCtxt = root->subs[2];
  g.check(macroCtxt->subs.size() == 1, "macro sub sub context");
  
  Context* subsub = macroCtxt->subs[0];
  g.check(subsub->charLen == 6, "sub sub is 6 chars long"+to_string(subsub->charLen));
  g.check(cmap->contextContent(subsub) == "quotes", "sub sub contains the word quotes");

  //badmap
  bool hadError = false;
  try {
    ContextMap* bmap = f2cmap("test/dos.carl");
  }
  catch (ContextError* cex){
    hadError = true;
    cout << cex->errMsg() << endl;
  }

  g.check(hadError, "dos.carl throws error");
  
  g.summary();
}
