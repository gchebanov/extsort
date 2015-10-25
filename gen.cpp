#include <bits/stdc++.h>

using namespace std;

typedef double T;

const T pi = M_PI;

int main(int argc, char* argv[]) {
  size_t N = 10000000;
  if (argc > 1) N = stol(argv[1]);
  ofstream fout("file.in", ios::out | ios::binary);
  vector<size_t> permutation(N);
  for (size_t i = 0; i < N; ++i)
    permutation[i] = i;
  random_shuffle(begin(permutation), end(permutation));
  for (size_t i : permutation) {
    T v = i * pi;
    fout.write((char*)(&v), sizeof(T));
  }
  return 0;
}
