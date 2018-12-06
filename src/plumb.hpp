using namespace std;

//this is meant to be a simple lib making use of a particular design pattern
//the goal is not to investigate c++'s eccentricities more than nescesary

template <typename OUT>
struct Input {
  virtual OUT give() = 0;
};


template <typename IN>
struct Output {
  virtual void get(IN n) = 0;
};

//there will be a symmetric joiner pattern, but not much code reusability there
template <typename T>
struct Splitter: public Output<T> {
  vector<Output<T>*> outputs;

  void get(T t){
    int i;
    for(i=0; i<outputs.size(); i++){
      *(outputs[i])->get(t);
    }
  }
};

template <typename T>
struct Heart {

  Input<T> *in;
  Output<T> *out;

  Heart(Input<T> *i, Output<T> *o): in(i), out(o)
  {}

  void beat(){
    out->get(in->give());
  }
};

struct PlumbingError {
  string msg; 
  PlumbingError(string gms){
    msg = gms;
  }
};

template <typename IN, typename OUT>
struct Pipe: public Input<OUT>, public Output<IN> {

  Pipe(): in(NULL), out(NULL)
  {}
  
  Input<IN> *in;
  Output<OUT> *out;

  IN take(){
    if (in != NULL){
      return in->give();
    } else {
      throw new PlumbingError("Pipe's got no input!");
    }
  }

  void put(OUT ret){
    if (out != NULL){
      out->get(ret);
    } else {
      throw new PlumbingError("Pipe's got no output!");
    }
  }
};

template <typename T>
struct Catch: Output<T> {

  T value;
  
  void get(T t) {value = t;}
  
};

//used for side effects
template <typename T>
struct StaticPipe: public Pipe<T, T> {

  T give(){
    T t = this->take();
    effect(t);
    return t;
  }

  void get(T t){
    effect(t);
    this->put(t);
  }

  virtual void effect(const T t) = 0;
};

template <typename T>
struct PrintPipe: public StaticPipe<T> {
  void effect(const T t){
    cout << t << endl;
  }
};

template <typename T>
struct StorePipe: public StaticPipe<T> {
  T value;
  void effect(const T t){
    value = t;
  }
};

//struct that is a function
//should be extended in a way that the Funk is reusable (no memory ownership)
template <typename IN, typename OUT>
struct Funk {
  virtual OUT act(const IN n) = 0;
};

template <typename IN, typename OUT>
struct DeltaPipe: public Pipe<IN, OUT>, public Funk<IN, OUT> {

  OUT give() {return this->act(this->take());}

  void get(IN n) {this->put(this->act(n));}

};

template <typename A, typename B, typename C>
void attachPipes(Pipe<A, B> *pa, Pipe<B, C> *pb){
  pa->out = pb;
  pb->in = pa;
}
