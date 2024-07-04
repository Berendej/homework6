#ifndef _MATRIX_2D_H_
#define _MATRIX_2D_H_

#include <map>
#include <algorithm>
#include <tuple>
//#include <iostream> // for debug
#include <memory>

using std::map;
using std::tuple;

namespace two_dim
{

/** \brief proxy_value_c serves as a holder of numeric
 *  value in matrix. could be empty. if dflt value is 
 *  assigned it treat it as a clear operation
 */
 template<typename T, int dflt>
class proxy_value_c
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
    operator const T() 
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
    proxy_value_c& operator=(const T&bro)
    {
        assign(bro);
        return *this;
    }
    bool assigned()
    {
        return (nullptr != m_sp.get());
    }
};

/** \brief row_c implements sparced vector of proxy values
*/
template<typename N, typename T, int dflt>
class row_c
{
    using proxy_value_t = proxy_value_c<T, dflt>;
    using value_map_t = map<N, proxy_value_t>;
    using i_value_map_t = typename value_map_t::iterator;
    value_map_t m_val_map;
public:
    proxy_value_t& operator[](N ix)
    {
        i_value_map_t it = m_val_map.find(ix);
        if ( it != m_val_map.end() )
        {
            return it->second;
        }
        auto pr = m_val_map.insert( std::make_pair(ix, proxy_value_t{(T)dflt}));
        if (!pr.second)
        {
            throw std::bad_alloc();
        }
        return (pr.first)->second;
    }

    row_c(const row_c &bro) : m_val_map(bro.m_val_map)
    {
    }

    row_c()
    {
    }

    size_t size()
    {
        size_t res = 0;
        std::for_each ( m_val_map.begin(),
                   m_val_map.end(),
                   [&](auto p) {if (p.second.assigned()) res++; } );
        return res;
    }

    class iterator
    {
        row_c *m_p_parent;
        i_value_map_t m_internal_it;
    public:
        tuple<size_t, T> operator*()
        {
            return std::tie(m_internal_it->first, m_internal_it->second);
        }
        void operator++(int)
        {
            // shift to the first not empty value
            m_internal_it++;
            while ( m_internal_it != m_p_parent->m_val_map.end() )
            {
                if(m_internal_it->second != dflt)
                {
                    break;
                }
                m_internal_it++;
            }
        }

        bool at_end()
        {
            return m_internal_it == m_p_parent->m_val_map.end();
        }

        iterator( row_c *parent,
            const i_value_map_t &internal_it):
            m_p_parent(parent),
            m_internal_it(internal_it)
        {
        }

        iterator (const iterator& bro) :
            m_p_parent(bro.m_p_parent),
            m_internal_it(bro.m_internal_it)
        {
        }

        iterator() : m_p_parent(nullptr)
        {

        }

        iterator& operator=(const iterator& bro)
        {
            m_p_parent = bro.m_p_parent;
            m_internal_it = bro.m_internal_it;
            return *this;
        }

    };

    iterator begin()
    {
        i_value_map_t it = m_val_map.begin();
        // shift to the first not empty value
        while ( it != m_val_map.end() )
        {
            if(it->second != dflt)
            {
                break;
            }
            it++;
        }
        iterator res{this, it};
        return res;
    }
};

/** \brief matrix_c implements sparced 2d matrix having no limits
 */ 
template<typename N, typename T, int dflt>
class matrix_c
{
using inst_row_t = row_c<N, T, dflt>;
using row_map_t =  map<N, inst_row_t >;
using i_row_map_t = typename row_map_t::iterator;

    row_map_t m_row_map;

public:
    inst_row_t& operator[](N ix)
    {
        i_row_map_t it = m_row_map.find(ix);
        if ( it != m_row_map.end() )
        {
            return it->second;
        }
        auto pr = m_row_map.insert( std::make_pair(ix, inst_row_t{} ));
        if (!pr.second)
        {
            throw std::bad_alloc();
        }
        return (pr.first)->second;
    }
    size_t size()
    {
        size_t res = 0;
        std::for_each(m_row_map.begin(), 
                      m_row_map.end(),
                      [&](auto r) {res += r.second.size();});
        return res;
    }

    class iterator
    {
        using row_iterator_t = typename inst_row_t::iterator;
        matrix_c *m_p_parent;
        i_row_map_t m_row_it;
        row_iterator_t m_internal_it;
        bool   m_end;
    public:
        tuple<size_t, size_t, T> operator*()
        {
            if (m_end) throw std::exception();
            auto tpl{*m_internal_it};
            return std::tie(m_row_it->first, std::get<0>(tpl), std::get<1>(tpl));
        }
        // only post increment defined
        void operator++(int)
        {
            if (m_end) return;
            m_internal_it++;
            while (m_internal_it.at_end())
            {
                m_row_it++;
                while ( m_row_it != m_p_parent->m_row_map.end() 
                    and m_row_it->second.size() == 0 )
                {
                    m_row_it++;
                }
                if(m_row_it == m_p_parent->m_row_map.end() )
                {
                    m_end = true;
                    return;
                }
                m_internal_it = m_row_it->second.begin();
            }
            // here m_internal_it points to not empty value
        }

        iterator(  matrix_c *parent,
                    const i_row_map_t & row_it,
                    const row_iterator_t & internal_it) : 
                m_p_parent(parent),
                m_row_it(row_it),
                m_internal_it(internal_it), 
                m_end(false)
        {
        }

        // special case for the begin() of empty matirx
        iterator(matrix_c *parent) : m_p_parent(parent), m_end(true)
        {
        }

        bool at_end()
        {
            return m_end;
        }
    };

    iterator begin()
    {
        i_row_map_t it = m_row_map.begin();
        // shift to the first not empty value
        while ( it != m_row_map.end() )
        {
            if( it->second.size() > 0 )
            {
                break;
            }
            it++;
        }
        if( it == m_row_map.end() )
        {
            // special case for empty matrix
            return iterator(this);
        }
        iterator res{this, it, it->second.begin()};
        return res;
    }
};

} // namespace two_dim
#endif