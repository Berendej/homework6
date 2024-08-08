#define BOOST_TEST_MODULE homework6 test

#include "matrix_2d.h"
#include <boost/test/included/unit_test.hpp>

bool test_matrix()
{
    two_dim::matrix_c<unsigned, int, 0> m;
    for( int i = 0; i < 10; i++)
    {
        m[i][i] = i;
    }
    //print_matrix(m);
//2. Второстепенную диагональ (от [0,9] до [9,0]) значениями от 9 до 0.
    for( int i = 0; i < 10; i++)
    {
        m[i][9-i] = 9-i;
    }
// 4. Вывести количество занятых ячеек.
    if ( 18 != m.size() )
    {
        return false;
    }
    ((m[100][100] = 314) = 0) = 217;
    if ( 217 != m[100][100] )
    {
        return false;
    }
    return true;
}

BOOST_AUTO_TEST_CASE(matrix_test)
{
    BOOST_CHECK( test_matrix()  );
}
