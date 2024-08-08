#ifndef _MATRIX_NDIM_
#define _MATRIX_NDIM_

#include <map>
#include <algorithm>
#include <tuple>
#include <memory>
#include <cassert>

using std::map;
using std::tuple;

namespace n_dim
{

/** \brief subscribed_c serves as a common 
 * class for proxy_value_c and matrix_c
 * 
 *  Purpose: operator[] on the lowest level case should return proxy_value
 *  and at the upper and middle leven should return matrix
 *  so it just returns subcribed_c which has virtuals
 */ 
template<typename T>
class subscripted_c
{
public:
    virtual subscripted_c& operator[](int ix) = 0;

    virtual operator const T() {  return 0;  }

    virtual subscripted_c& operator=(const T& bro) = 0;
};

/** \brief proxy_value_c serves as a holder of numeric
 *  value in matrix. could be empty.
 */
template<typename T, int dim, int dflt>
class proxy_value_c : public subscripted_c<T>
{
    using sp_T_t = std::shared_ptr<T>;
    sp_T_t m_sp; // initially empty

    void assign(const T&bro)
    {
        if ( (T)dflt !=  bro )
        { 
            T *p = new T{bro};
            m_sp.reset(p);
        }
        else
        {
            m_sp.reset(); // free value
        }
    }

public:
    operator const T() override
    {
        if (m_sp)
        {
            return *m_sp;
        }
        return dflt;
    }
    proxy_value_c(const T& bro)
    {
        assign(bro);
    }
    
    proxy_value_c()
    {
    }

    proxy_value_c(const proxy_value_c& bro)
    {
        m_sp = bro.m_sp;
    }

    virtual ~proxy_value_c()
    {
    }

    subscripted_c<T>& operator=(const T&bro) override
    {
        assign(bro);
        return (subscripted_c<T> & )*this;
    }

    bool assigned()
    {
        return (nullptr != m_sp.get());
    }

    subscripted_c<T>& operator[](int ix) override
    {
        // stub, must never be called
        assert(false);
        return *this;
    }
};

/** \brief matrix_c implements sparced n-dim matrix having no limits
 *  the trick is to use sub level of proxy_values (value_map_t) or [dim-1] matrix (row_t)
 *  recursively. 
 * 
 *  Warning: can't declare row_map_t = map<int, row_t > because it would
 *  lead to endless recursive generation of classes matrix(dim-1) and so on
 *  the trick is to use pointer or smart pointer which prevent this.
 *  Smart pointers intensively used in matrix_c in order to be efficient.
 *  For instance  for 4-d martix if we insert m[1][1][4][5] = 33;
 *  it would lead to MASSIVE relocation (copy and remove ) 
 *  of matrices and sub-matrices and sub-sub-matrices and so on
 *  with smart pointers the only thing which is reallocates is a smart pointer
 *  the matrices remain untouched
 * 
 *  if constexpr - helps to generate 2 kinds of matrices upper and middle layers
 *  and lowest layer which keeps numbers 
 */ 
template<typename T, int dim, int dflt>
class matrix_c : public subscripted_c<T>
{
    // dimension. dim == 1 for the last dimension (keeps proxy values)

    // members for dim == 1
    using proxy_value_t = proxy_value_c<T, dim, dflt>;
    using sp_proxy_value_t = std::shared_ptr<proxy_value_t>;
    using value_map_t = map<int, sp_proxy_value_t>;
	using sp_value_map_t = std::shared_ptr<value_map_t>;
    using i_value_map_t = typename value_map_t::iterator;
    sp_value_map_t m_sp_val_map;


    // members for dim > 1
    // upper or intermediate matrices
    // *** dim-1 *** does the trick !
    using row_t = matrix_c<T, dim-1, dflt>;
    using sp_row_t = std::shared_ptr<row_t>;
    using row_map_t = map<int, sp_row_t >;
	using sp_row_map_t = std::shared_ptr<row_map_t>;
    using i_row_map_t = typename row_map_t::iterator;
    sp_row_map_t m_sp_row_map;

public:
    matrix_c()
    {
        if constexpr (dim > 1)
        {
            // matrix
            m_sp_row_map.reset(new row_map_t);
        }
        else
        {
            // lowest level (row)
            m_sp_val_map.reset(new value_map_t );
        }
    }

    virtual ~matrix_c()
    {
    }

    subscripted_c<T>& operator[](int ix) override
    {
        if constexpr ( dim > 1 )
        {
            i_row_map_t it = m_sp_row_map->find(ix);
            if (it != m_sp_row_map->end())
            {
                sp_row_t& r = it->second;
                return (subscripted_c<T>&)(*r);
            }
            sp_row_t sp_row( new row_t{} );
            auto pr = m_sp_row_map->insert(std::make_pair(ix, sp_row));
            if (!pr.second)
            {
                throw std::bad_alloc();
            }
            return (subscripted_c<T>&)(*sp_row);
        }
        else
        {
            i_value_map_t it = m_sp_val_map->find(ix);
            if ( it != m_sp_val_map->end() )
            {
                sp_proxy_value_t sp_val = it->second;
                return *sp_val;
            }

            sp_proxy_value_t sp_val( new proxy_value_t{(T)dflt } );
            auto pr = m_sp_val_map->insert( std::make_pair(ix, sp_val) );
            if (!pr.second)
            {
                throw std::bad_alloc();
            }
            return (*sp_val);
        }
    }

    size_t size()
    {
        size_t res = 0;
        if constexpr ( dim > 1 )
        {
            std::for_each(m_sp_row_map->begin(),
                        m_sp_row_map->end(),
                [&](auto r) 
                { 
                    sp_row_t sp_row = r.second; 
                    res += sp_row->size(); 
                }
            );
            return res;
        }
        else
        {
            std::for_each ( m_sp_val_map->begin(),
                   m_sp_val_map->end(),
                   [&](auto p) 
                   {
                        sp_proxy_value_t sp_val = p.second;
                        if (sp_val->assigned()) 
                            res++; 
                    } );
        }
        return res;
    }

    subscripted_c<T>& operator=(const T& bro) override
    {
        // should never be called
        assert(false);
        return (subscripted_c<T>&)*this;
    }

};

} // namespace ndim

#endif
