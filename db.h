#ifndef _DB_H_
#define _DB_H_

#include <iostream>
using namespace std ;

// otl
#define OTL_BIGINT long long
#define OTL_ODBC_MYSQL // Compile OTL 4/ODBC. Uncomment this when used with MS SQL 7.0/ 2000
#define OTL_STL // enable STL / ANSI C++ compliance.
#define OTL_ODBC_UNIX // uncomment this line if UnixODBC is used
#define OTL_UNCAUGHT_EXCEPTION_ON // enable safe exception handling / error

#include "otlv4.h"

inline void show_sql_error( const otl_exception &err, ostream &os = cout )
{
	os << "err msg: " << err.msg << endl;                 // print out error message
	os << "err s_sql: " << err.stm_text << endl;            // print out SQL that caused the error
	os << "err num: " << err.sqlstate << endl;            // print out SQLSTATE info.
	os << "err var: " << err.var_info << endl;            // print out the variable that caused the error
}

#endif


