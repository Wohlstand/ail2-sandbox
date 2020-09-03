/*[]------------------------------------------------------------[]*/
/*|                                                              |*/
/*|     istuint.cpp                                              |*/
/*|                                                              |*/
/*|     Class istream                                            |*/
/*|          istream& istream::operator>> ( unsigned int& )      |*/
/*|                                                              |*/
/*[]------------------------------------------------------------[]*/

/*
 *      C/C++ Run Time Library - Version 5.0
 *
 *      Copyright (c) 1990, 1992 by Borland International
 *      All Rights Reserved.
 *
 */

/*
    To reduce the total amount of code, shorts and ints are extracted into
    a long, and the result stored in the proper type.  If long arithmetic
    creates an unacceptable overhead, you can duplicate the code with
    the obvious modifications for shorts or ints.
*/

#include <ioconfig.h>
#include <iostream.h>

istream _FAR & istream::operator>> (unsigned int _FAR & i)
{
    unsigned long l;
    *this >> l;
    if( ! fail() )
        i = (unsigned int) l;
    return *this;
}


