#ifndef DATA_HPP
#define DATA_HPP

#define SDEF 50
#define NDEF 300

template<typename T>
void swap(T& a, T&b)
{
  T tmp = a;
  a = std::move(b);
  b = std::move(tmp);
}

template<int S, int N>
class Data
{
private:
  byte **data = nullptr;
public:
  Data()
  {
      data = new byte*[N];
      for(int i = 0; i<N; ++i) data[i] = new byte[S];  
  }
  ~Data()
  {
      if(data) for(int i = 0; i<N; ++i) delete [] data[i];
      delete [] data;
  }
  Data(const Data<S, N>& obj)
  {
      data = new byte*[N];
      for(int i = 0; i<N; ++i){
        data[i] = new byte[S];
        std::copy(obj.data[i], obj.data[i] + S, data[i]);
      }
  }
  Data(Data<S, N>&& obj)
  {
      swap(data, obj.data);
  }
  Data& operator=(Data<S, N> d)
  {
      swap(data, d.data);
  }
  byte& operator[](int n)
  {
      return data[n/S][n%S];
  }
};

template<int S, int N>
class DataWrapper
{
private: 
  Data<S, N>* data;
public:
  DataWrapper(Data<S, N>* _data) : data(_data) {}
  byte& operator[](int n)
  {
      return (*data)[n];
  }
};


#endif
