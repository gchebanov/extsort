#include <bits/stdc++.h>

using namespace std;

const size_t N = 1000;
typedef double T;

const T pi = M_PI;

int main(int argc, char* argv[]) {
  ifstream fin("file.out", ios::in | ios::binary);
  size_t i = 0;
  T v;
  while (fin.read((char*)(&v), sizeof(T))) {
    assert(i * pi == v);
    i += 1;
  }
  cerr << "ok, " << i << '\n';
  return 0;
}
