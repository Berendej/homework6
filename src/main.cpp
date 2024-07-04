#include <cstdint>
#include <iostream>
#include <cassert>

#include "matrix_2d.h"
#include "matrix_ndim.h"

using std::cout;


/** print_matrix() prints content of hardcoded 2-dimensional matrix
 *  traverse over matrix with iterator
 */
template<typename N, typename T, int dflt>
void print_matrix( two_dim::matrix_c<N, T, dflt> &m )
{
    auto it{m.begin()};
    while( not it.at_end() )
    {
        //auto ppv = *it;
        N x;
        N y;
        T v;
        std::tie(x, y, v) = *it;
        std::cout << x << " " << y << " " << v << std::endl;
        it++;
    }
}

/**
 * demo of two_dim::matrix 
 * not called in main
 */
void demo()
{
    two_dim::matrix_c<int, float, 44> m;
    cout << m.size() << std::endl;
    auto a = m[-1][0];
    cout << a << std::endl;
    assert(a == 44.0);
    cout << m.size() << std::endl; 
    m[-2][0] = 2.0;
    float f = m[0][0];
    cout << f << std::endl;
    cout << m.size() << std::endl;
    m[2][34] = 10.0;
    m[0][444] = 40.0;
    m[2][34] = 44.0;
    char ch;
    auto it{m.begin()};
    while( not it.at_end() )
    {
        auto ppv = *it;
        int x;
        int y;
        float v;
        std::tie(x, y, v) = ppv;
        std::cout << x << " " << y << " " << v << std::endl;
        std::cin >> ch;
        it++;
    }
}


/** two_dim_demo() example of hardcoded 2-dimensional matrix usage
 *  it's simple but has iterator
 */
void two_dim_demo()
{
    std::cout << "*****************************************************\n";
    std::cout << "testing 2-dim matrix\n";
//1. При запуске программы необходимо создать матрицу с пустым значением 0, 
    two_dim::matrix_c<unsigned, int, 0> m;
//заполнить главную диагональ матрицы (от [0,0] до [9,9]) значениями от 0 до 9.
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
    //print_matrix(m);
//3. Необходимо вывести фрагмент матрицы от [1,1] до [8,8]. 
// Между столбцами пробел. Каждая строка матрицы на новой строке консоли.
    for( int i = 1; i <= 8; i++)
    {
        for( int j = 1; j <= 8; j++ )
        {
            cout << m[i][j] << " ";
        }
        cout << "\n";
    }
// 4. Вывести количество занятых ячеек.
    cout << "size = " << m.size() << "\n";

//5. Вывести все занятые ячейки вместе со своими позициями.
    print_matrix(m);

//7. Опционально реализовать каноническую форму оператора `=`, допускающую выражения
    ((m[100][100] = 314) = 0) = 217;
    cout << "expression ((m[100][100] = 314) = 0) = 217 yelds "
         << m[100][100] << "\n";

//6. Опционально реализовать N-мерную матрицу.

}

/** \brief test_1d() example of n-dimensional matrix usage
 *  for only 1 dimension
 */
void test_1d()
{
    std::cout << "*****************************************************\n";
    std::cout << "testing n-dimensional matrix with dimensions set to 1\n";
    n_dim::matrix_c<int, 1, 0> m;
    m[3] = 3;
    m[3] = 0;
    for (int i = 3; i < 6; i++)
    {
        m[i] = i;
    }
    for (int i = 0; i < 10; i++)
    {
        cout << m[i] << " ";
    }
    cout << std::endl;
    cout << "size = " <<  m.size() << std::endl;
}

/** \brief test_3d() example of n-dimensional matrix usage
 *  for 3 dimensions
 */
void test_3d()
{
    std::cout << "*****************************************************\n";
    std::cout << "testing n-dimensional matrix with dimensions set to 3\n";
    n_dim::matrix_c<int, 3, 0> m;
    for (int i = 0; i < 10; i++)
    {
        m[i][i][i] = i; // sum 9
    }
    for (int i = 0; i < 10; i++)
    {
        cout << " layer " << i << " :\n";
        for (int j = 0; j < 10; j++)
        {
            for (int k = 0; k < 10; k++)
            {
                cout << m[i][j][k] << " ";
            }
            cout << std::endl;
        }
        cout << std::endl;
    }
    cout << "size = " <<  m.size() << std::endl; // must be.9
}

/** \brief test_2d() example of n-dimensional matrix usage
 *  for 2 dimensions
 */
void test_2d()
{
    std::cout << "*****************************************************\n";
    std::cout << "testing n-dimensional matrix with dimensions set to 2\n";
    n_dim::matrix_c<int, 2, 0> m;
    for( int i = 0; i < 10; i++)
    {
        m[i][i] = i;
    }
    for( int i = 0; i < 10; i++)
    {
        m[i][9-i] = 9-i;
    }
    for( int i = 1; i <= 8; i++)
    {
        for( int j = 1; j <= 8; j++ )
        {
            cout << m[i][j] << " ";
        }
        cout << "\n";
    }
    cout << "size = " <<  m.size() << "\n";
    ((m[100][100] = 314) = 0) = 217;
    cout << "expression ((m[100][100] = 314) = 0) = 217 yelds "
         << m[100][100] << "\n";
}

/** \brief n_dim_demo() example 1,2,3 dimensional matrices usage
 */
void n_dim_demo()
{
    test_1d();
    test_2d();
    test_3d();
}

int main(int c, char *args[])
{
    two_dim_demo();
    n_dim_demo();
}