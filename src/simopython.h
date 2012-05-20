// wxwidgets define this, and python too, lets undefine it before we do python stuff
#ifdef HAVE_SSIZE_T
#undef HAVE_SSIZE_T
#endif

#define BOOST_PYTHON_STATIC_LIB
#define BOOST_PYTHON_STATIC_MODULE
#include <boost/python.hpp>