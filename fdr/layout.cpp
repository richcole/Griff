#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <assert.h>
#include <time.h>
#include <sys/time.h>

using namespace std;

template <class T> 
class Vector : public vector<T> {
  T _zero;
public:
  Vector(T const zero) : _zero(zero) {
  }

  Vector(Vector<T> const& vect) :
    vector<T>(vect), _zero(vect.zero())
  {
  };

  bool operator=(Vector<T> const& vect) {
    vector<T>::operator=(vect);
    _zero = vect.zero();
  }

  T const& zero() const {
    return _zero;
  }
    

  typedef typename vector<T>::size_type size_type;

  T& operator[](size_type index) {
    assert(index > 0);
    if ( index >= size() ) {
      resize(index, _zero);
    }
    return *(begin() + (index-1));
  }

  T const& operator[](size_type index) const {
    assert(index > 0);
    if ( index >= size() ) {
      const_cast<Vector<T>*>(this)->resize(index, _zero);
    }
    return *(begin() + (index-1));
  }

  Vector<T> operator-(Vector<T> const& M) const {
    Vector<T> R(_zero);
    for(size_type i=1;i<=size();++i) {
      R[i] = (*this)[i] - M[i];
    }
    return R;
  }

  T norm() const {
    T result = _zero;
    for(size_type i=1;i<=size();++i) {
       result += (*this)[i] * (*this)[i];
    }
    return sqrt(result);
  }

  Vector<T>& operator+=(Vector<T> const& M) {
    for(size_type i=1;i<=size() || i<=M.size();++i) {
      (*this)[i] += M[i];
    }
  }

  Vector<T> operator*(T const& t) const {
    Vector<T> R(_zero);
    for(size_type i=1;i<=size();++i)
      R[i] = (*this)[i] * t;
    return R;
  }

};
      

class Matrix : public Vector<Vector<float> > {
  int _num_rows, _num_columns;
public:
  Matrix() : 
    Vector<Vector<float> >(Vector<float>(0))
  {
    _num_rows = 0; _num_columns = 0;
  }

  size_type num_rows() const {
    return _num_rows;
  }

  size_type num_columns() const {
    return _num_columns;
  }

  void update() {
    size_type i;

    _num_rows = size();

    _num_columns = 0;
    for(i=1;i<=num_rows();++i) {
      if ( (*this)[i].size() > num_columns() ) {
        _num_columns = (*this)[i].size();
      }
    }
  }

  void resize(size_type num_columns, size_type num_rows) {
    Vector<Vector<float> >::resize(num_rows, zero());
    for(size_type i=1;i<=num_rows;++i) {
      (*this)[i].resize(num_columns);
    }
    _num_rows = num_rows;
    _num_columns = num_columns;
  }

  Matrix operator *(Matrix const& m) const {
    Matrix R;
    size_type i, j, k;
    for(i=1;i<=num_rows();++i) {
      for(j=1;j<=m.num_columns();++j) {
        for(k=1;k<=num_columns();++k) {
          R[i][j] += (*this)[i][k] * m[k][j];
        }
      }
    }
    R.update();
    return R;
  }

  Matrix transpose() const {
    Matrix R;
    for(size_type i=1;i<=_num_rows;++i) 
      for(size_type j=1;j<=_num_columns;++j)
        R[j][i] = (*this)[i][j];
    R.update();
    return R;
  }

  Matrix operator+(Matrix const& M) const {
    Matrix R;
    for(size_type i=1;i<=_num_rows;++i) {
      for(size_type j=1;j<=_num_columns;++j) {
        R[i][j] = (*this)[i][j] + M[i][j];
      }
    }
    R.update();
    return R;
  }

  Matrix operator*(float t) const {
    Matrix R;
    for(size_type i=1;i<=_num_rows;++i) {
      for(size_type j=1;j<=_num_columns;++j) {
        R[i][j] = (*this)[i][j] * t;
      }
    }
    R.update();
    return R;
  }

  Matrix& operator*=(float t) {
    for(size_type i=1;i<=_num_rows;++i) {
      for(size_type j=1;j<=_num_columns;++j) {
        (*this)[i][j] *= t;
      }
    }
    return *this;
  }

  Matrix& operator+=(Matrix const& M) {
    for(size_type i=1;i<=_num_rows;++i) {
      for(size_type j=1;j<=_num_columns;++j) {
        (*this)[i][j] += M[i][j];
      }
    }
    update();
    return *this;
  }

  void truncate(float k) {
    for(size_type i=1;i<=_num_rows;++i) {
      for(size_type j=1;j<=_num_columns;++j) {
        float v = (*this)[i][j];
        if ( fabs(v) > k ) {
          (*this)[i][j] = (v / fabs(v)) * k;
        }
      }
    }
  }

  void truncate_y(float k) {
    for(size_type i=1;i<=_num_rows;++i) {
        float v = (*this)[i][2];
        if ( v < k ) {
          (*this)[i][2] = k;
        }
      }
    }

};
  
void read_matrix(istream& input, Matrix& matrix)
{
  char buffer[4096];
  Matrix::size_type i = 1, j = 1;
  while( input.getline(buffer, sizeof(buffer)), ! input.fail() ) {
    istringstream line_stream(buffer);
    float val;
    j = 1;
    while( line_stream >> val, ! line_stream.fail() ) {
      matrix[i][j] = val;
      ++j;
    }
    ++i;
  }
  matrix.update();
}

void write_matrix(ostream& output, Matrix& matrix)
{
  int i, j;
  for(i=1;i<=matrix.num_rows();++i) {
    for(j=1;j<=matrix.num_columns();++j) {
      output << matrix[i][j] << " ";
    }
    output << endl;
  }
}

Matrix f_r(Matrix const& P, float k) {
  Matrix F;
  for(Matrix::size_type i=1;i<=P.num_rows();++i) {
    for(Matrix::size_type j=1;j<=P.num_rows();++j) {
      if ( i==j ) { continue; }
      Vector<float> d = P[j] - P[i];
      float dl = d.norm();
      if ( dl > 0.01 ) {
        F[j] += d * ((k * k) / (dl * dl * dl));
      } else {
        F[j][1] += k * k * (drand48() - 0.5);
        F[j][2] += k * k * (drand48() - 0.5);
      }
    }
  }
  F.update();
  return F;
}

Matrix f_a(Matrix const& P, Matrix const& E, float k) {
  Matrix F;
  for(Matrix::size_type i=1;i<=E.num_rows();++i) {
    Matrix::size_type u = (Matrix::size_type)E[i][1];
    Matrix::size_type v = (Matrix::size_type)E[i][2];
    Vector<float> d = P[v] - P[u];
    float dl = d.norm();
    F[u] += d * ((dl - k) / dl);
    F[v] += d * ((k - dl) / dl);
  }
  F.update();
  return F;
}

Matrix f_d(Matrix const& P, Matrix const& E, float k) {
  Matrix F;
  for(Matrix::size_type i=1;i<=E.num_rows();++i) {
    Matrix::size_type u = (Matrix::size_type)E[i][1];
    Matrix::size_type v = (Matrix::size_type)E[i][2];
    float d = (P[v][2] - P[u][2]);
    if ( d < k/4 ) {
      F[v][2] = k * k;
    }
  }
  F.update();
  return F;
}

Matrix f_l(Matrix const& P, Matrix const& E, float k) {
  Matrix F;
  for(Matrix::size_type p=1;p<=P.num_rows();++p) {
    for(Matrix::size_type i=1;i<=E.num_rows();++i) {
      Matrix::size_type a = (Matrix::size_type)E[i][1];
      Matrix::size_type b = (Matrix::size_type)E[i][2];
      if ( a == p || b == p ) { continue; }
      Vector<float> ab = P[b] - P[a];
      float abl = ab.norm();
      Vector<float> ap = P[p] - P[a];
      float t = ( (ab[1]*ap[1]) + (ab[2]*ap[2]) ) / abl;
      if ( t > 0 && t < abl ) {
        
        Vector<float> x = P[a];
        x += (ab * (t / abl));
        Vector<float> d = P[p] - x;

        float dl = d.norm();
        if ( dl > 0.01 ) {
          F[p] += d * ((k * k) / (dl * dl * dl));
          //          F[a] += d * (- (k * k) / (dl * dl));
          //          F[b] += d * (- (k * k) / (dl * dl));
        } else {
          F[p][1] += k * k * (drand48() - 0.5);
          F[p][2] += k * k * (drand48() - 0.5);
        }      
      }
    }
  }
  return F;
}
  

void usage() {
  cerr << "Usage: layout A.ascii E.ascii" << endl;
}

int main(int argc, char **argv)
{
  if ( ! (argc == 3 || argc == 4) ) {
    usage();
    exit(-1);
  }

  Matrix A, A_t, E, V, P, dP, dV, L, PL;

  {
    ifstream input(argv[1]);
    read_matrix(input, A);
  }

  {
    ifstream input(argv[2]);
    read_matrix(input, E);
  }

  if ( argc == 4 ) {
    ifstream input(argv[3]);
    read_matrix(input, L);
  }

  timeval tv;
  gettimeofday(&tv, 0);
  srand48(tv.tv_usec);
  for(int i=1;i<=A.num_columns();++i) {
    V[i][1] = (10.0 + i) * (i % 2 == 0 ? 1.0 : -1.0);
    V[i][2] = 5.0 + (10.0  * (drand48()));
  }
  V.update();

  P = A*V;
  A_t = A.transpose();

  write_matrix(cout, P);

  PL = L*V;
  cout << "PL=" << endl;
  write_matrix(cout, PL);

  Matrix::size_type num_nodes = A.num_rows();
  float k = 10;

  float temp_s = 4.0;
  float temp_e = 0.2;

  int num_rounds = 400;
  for(int round=1;round<num_rounds;++round) {
    Matrix F_r = f_r(P, 1.2*k);
    Matrix F_a = f_a(P, E, k);
    Matrix F_d = f_d(P, E, k);
    Matrix F_l = f_l(P, E, k);

    Matrix F_t = F_a + F_r + F_l;

    float  temp = temp_s - 
      ( (temp_s - temp_e) * ((double)round / (double)num_rounds)) ;
    F_t *= temp / ( num_nodes  );

    dP = F_t;

    dV = A_t * dP;
    dV.truncate(temp * 4 * k);

    V += dV;
    V.truncate_y(k/5);
    
    P = A * V;
  }

  cout << "P=" << endl;
  write_matrix(cout, P);
  PL = L*V;
  cout << "PL=" << endl;
  write_matrix(cout, PL);
}

    
