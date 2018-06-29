%module cpp

%include <std_string.i>
%include <std_vector.i>

%typemap(in) uint8_t * {
  $1 = (uint8_t *) PyInt_AsLong($input);
}

%{
#include "../src/ArgusCamera.h"
%}

%include "../src/ArgusCamera.h"
