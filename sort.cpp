#include <bits/stdc++.h>

using namespace std;

template<
  typename T = double,
  size_t blk_sz = 1024 * 1024,
  size_t prefetch = 512
> class FileSorter {
private:
  size_t get_count(ifstream& f) {
    auto old_pos = f.tellg();
    f.seekg(streamoff(0), ios_base::end);
    auto last_pos = f.tellg();
    f.seekg(old_pos);
    const int item_sz = sizeof(T);
    if (last_pos % item_sz)
      throw logic_error("Bad file size");
    return last_pos / item_sz;
  }
  T readT(ifstream& f, size_t pos) {
    f.seekg(pos * sizeof(T));
    T res;
    f.read((char*)&res, sizeof(T));
    return res;
  }
  void prep_tmp(ifstream& fin, const string& tmp_file_name, size_t n) {
    fin.seekg(streampos(0));
    ofstream tf(tmp_file_name, ios::out | ios::binary);
    vector<T> a(blk_sz);
    for (size_t i = 0; i < n; i += blk_sz) {
      size_t j = min(i + blk_sz, n) - i;
      fin.read((char*)(a.data()), j * sizeof(T));
      std::sort(begin(a), begin(a) + j);
      tf.write((char*)(a.data()), j * sizeof(T));
    }
  }

  void sort_tmp(ofstream& fout, const string& tmp_file_name,  size_t n) {
    ifstream tf(tmp_file_name, ios::in | ios::binary);
    size_t m = (n + blk_sz - 1) / blk_sz;
    typedef pair<T, size_t> QT;
    priority_queue<QT,vector<QT>,greater<QT> > Q;
    for (size_t i = 0; i < m; ++i) {
      T v = readT(tf, i * blk_sz);
      Q.emplace(v, i * blk_sz);
    }
    while (!Q.empty()) {
      auto val_pos = Q.top(); Q.pop();
      fout.write((char*)(&val_pos.first), sizeof(T));
      auto new_pos = val_pos.second + 1;
      if (new_pos % blk_sz && new_pos < n) {
        T v = readT(tf, new_pos);
        Q.emplace(v, new_pos);
      }
    }
  }

  void sort_tmp_prefetch(ofstream& fout, const string& tmp_file_name, size_t n) {
    ifstream tf(tmp_file_name, ios::in | ios::binary);
    size_t m = (n + blk_sz - 1) / blk_sz;
    vector<array<T, prefetch> > PR(m);
    for (size_t i = 0; i < n; i += blk_sz) {
      size_t j = min(i + prefetch, n) - i;
      tf.seekg(i * sizeof(T));
      tf.read((char*)(begin(PR.at(i / blk_sz))), j * sizeof(T));
    }
    typedef pair<T, size_t> QT;
    priority_queue<QT,vector<QT>,greater<QT> > Q;
    for (size_t i = 0; i < m; ++i) {
      Q.emplace(PR[i][0], i * blk_sz);
    }
    while (!Q.empty()) {
      auto val_pos = Q.top(); Q.pop();
      fout.write((char*)(&val_pos.first), sizeof(T));
      auto new_pos = val_pos.second + 1;
      if (new_pos % blk_sz && new_pos < n) {
        size_t blk = new_pos / blk_sz;
        size_t off = new_pos % blk_sz;
        if (off % prefetch == 0) {
          size_t j = min(new_pos + prefetch, n) - new_pos;
          tf.seekg(new_pos * sizeof(T));
          tf.read((char*)(PR[blk].begin()), j * sizeof(T));
        }
        T v = PR[blk][off % prefetch];
        Q.emplace(v, new_pos);
      }
    }
  }

public:
  void sort(ifstream &fin, ofstream& fout, const string& tmp_file_name) {
    size_t n = get_count(fin);
    prep_tmp(fin, tmp_file_name, n);
    sort_tmp_prefetch(fout, tmp_file_name, n);
  }
};

int main(int argc, char* argv[]) {
  FileSorter<> fs;
  ifstream fin("file.in", ios::in | ios::binary);
  ofstream fout("file.out", ios::out | ios::binary);
  fs.sort(fin, fout, "tmp.bin");
  return 0;
}
