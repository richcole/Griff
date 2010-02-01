#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

struct BCNF_Header {
  char magic[4];
  int  order;
  int  nvars;
  int  nclauses;

  void init() {
    strcpy(magic, "BCNF");
    order = 0x01020304;
    nvars = 0;
    nclauses = 0;
  }
};

int main(int argc, char **argv)
{
  if ( argc != 3 ) {
    cerr << "Usage: convert_to_bcnf <inp> <outp>" << endl;
  };

  ifstream inp(argv[1]);
  ofstream outp(argv[2]);

  bool wrote_header = false;
  bool is_error = false;
  char buf[1024 * 1024];


  while ( inp.getline(buf, sizeof(buf)), !inp.fail() && ! is_error ) {
    switch(buf[0]) {
    case 'c':
      // ignore
      break;
    case 'p':
      // header
      if ( wrote_header ) {
        cerr << "Error: more than one header in file." << endl;
        is_error = true;
      }
      else {
        BCNF_Header header;
        header.init();
        istringstream sbuf(buf);
        string p, cnf;
        sbuf >> p >> cnf >> header.nvars >> header.nclauses;
        if ( sbuf.fail() ) {
          cerr << "Error: unable to parse header '" << buf << "'" << endl;
          is_error = true;
        }
        else {
          outp.write((char *)&header, sizeof(header));
          wrote_header = true;
        }
      }
      break;
    default:
      if ( ! wrote_header ) {
        cerr << "Error: clause found without header first." << endl;
        is_error = true;
      }
      else {
        istringstream sbuf(buf);
        int vs[1024];
        int i = 0;
        while( sbuf >> vs[i], ! sbuf.fail() ) {
          ++i;
        }
        outp.write((char *)&i, sizeof(i));
        outp.write((char *)vs, i*sizeof(*vs));
      }
    }
  }

  int end = -1;
  outp.write((char *)&end, sizeof(end));
};

  
