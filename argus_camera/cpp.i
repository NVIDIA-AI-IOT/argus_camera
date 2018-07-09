%module cpp

%include <std_string.i>
%include <std_vector.i>
%include <stdint.i>

%typemap(in) uint8_t* {
  $1 = (uint8_t*) PyInt_AsLong($input);
}

namespace std {
  %template (UInt32Vector) vector<uint32_t>;
  %template (UInt64Vector) vector<uint64_t>;
  %template (FloatVector) vector<float>;
}

%{
#include "../src/ArgusCamera.hpp"
%}

%include "../src/ArgusCamera.hpp"
