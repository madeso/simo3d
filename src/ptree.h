#ifndef PTREE_H
#define PTREE_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <fstream>

typedef boost::property_tree::basic_ptree<string, string> ptree;

#ifdef _UNICODE
#define WX_CSTR(x) x.wc_str()
typedef std::wifstream ifstream;
#else
#define WX_CSTR(x) x.c_str()
typedef std::ifstream ifstream;
#endif

#ifdef _T
//#pragma message("_T already defined...")
#else
#ifdef _UNICODE
        #define _T(x) L##x
#else
        #define _T(x) x
#endif
#endif

#endif