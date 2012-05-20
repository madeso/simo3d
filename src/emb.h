//
// Copyright (C) 2011 Mateusz Loskot <mateusz@loskot.net>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Blog article: http://mateusz.loskot.net/?p=2819

#include <boost/function.hpp>
#include "simopython.h"

#ifndef LOSKOT_EMB_H
#define LOSKOT_EMB_H

namespace emb
{
	typedef boost::function<void(const std::string&)> stdout_write_type;


	PyMODINIT_FUNC PyInit_emb(void);

	void set_stdout(stdout_write_type write);
	void reset_stdout();

	void set_stderr(stdout_write_type write);
	void reset_stderr();

} // namespace emb

#endif LOSKOT_EMB_H