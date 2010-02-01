#ifndef LEXICON_H
#define LEXICON_H

#include <gc/gc_cpp.h>
#include <unistd.h>
#include <map>
#include <string>

#define FOR_EACH(x, S) for((x)=(S).begin();(x)!=(S).end();++(x))

namespace RJL {

  using namespace std;

  class InvalidIndex { public: InvalidIndex() {}; };
  class ProgramError { public: ProgramError() {}; };

  class Lexicon {
  public:
    typedef size_t index_type;
    typedef string value_type;
    typedef map<index_type, value_type> index_value_map_type;
    typedef map<value_type, index_type> value_index_map_type;
  protected:
    index_value_map_type _index_value_map;
    value_index_map_type _value_index_map;
    index_type           _next_index;
  public:
    Lexicon() : _next_index(1) {};

    index_type index(value_type const& value) {
      value_index_map_type::const_iterator it = _value_index_map.find(value);
      index_type return_index = 0;
      if ( it == _value_index_map.end() ) {
	_value_index_map.insert(pair<value_type,index_type>(value, _next_index));
	_index_value_map.insert(pair<index_type,value_type>(_next_index, value));
	return_index = _next_index;
	++_next_index;
      }
      else {
	return_index = it->second;
      }
      return return_index;
    };

    value_type const& word(index_type index) const {
      if ( index < 1 || index >= _next_index ) {
	throw InvalidIndex();
      };
      index_value_map_type::const_iterator it = _index_value_map.find(index);
      if ( it == _index_value_map.end() ) {
	throw InvalidIndex();
      }
      else {
	return it->second;
      }
    };
  };
}; // namespace RJL


#endif
