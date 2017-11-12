#pragma once

template <typename T>
struct IsPOD
{
	enum
	{
		value = false
	};
};
template <typename T>
struct IsPOD<T*>
{
	enum
	{
		value = true
	};
};

#define POD_TYPE( type ) template<> struct IsPOD< type > { enum { value = true }; };

POD_TYPE(bool)
POD_TYPE(UINT16)
POD_TYPE(UINT32)
POD_TYPE(INT16)
POD_TYPE(INT32)
POD_TYPE(INT64)
POD_TYPE(unsigned char)
POD_TYPE(char)
POD_TYPE(float)
POD_TYPE(double)

template <typename T >
class TArray
{
private:
	T* m_data;
	UINT m_allocSize;
	UINT m_size;

private:
	void Reallocate( UINT const size )
	{
		if ( size != m_allocSize )
		{
			T* oldData = m_data;
			m_data = nullptr;

			if ( !IsPOD< T >::value )
			{
				for ( UINT i = size; i < m_size; ++i )
				{
					oldData[ i ].~T();
				}
			}

			if ( 0 < size )
			{
				m_data = (T*)malloc( size * sizeof( T ) );
				UINT const copySize = size < m_size ? size : m_size;
				if ( !IsPOD< T >::value )
				{
					for ( UINT i = 0; i < copySize; ++i )
					{
						new ( &m_data[ i ] ) T( oldData[ i ] );
					}
				}
				else
				{
					memcpy( m_data, oldData, copySize * sizeof( T ) );
				}
			}
			free( oldData );

			m_allocSize = size;
			m_size = m_allocSize < m_size ? m_allocSize : m_size;
		}
	}

public:
	TArray()
		: m_data( nullptr )
		, m_allocSize( 0 )
		, m_size( 0 )
	{}
	~TArray()
	{
		if ( !IsPOD< T >::value )
		{
			for ( UINT i = 0; i < m_size; ++i )
			{
				m_data[ i ].~T();
			}
		}

		free( m_data );
	}

	void Resize( UINT const size )
	{
		if ( m_size < size )
		{
			if ( m_allocSize < size )
			{
				Reallocate( size );
			}

			if ( !IsPOD< T >::value )
			{
				for ( UINT i = m_size; i < size; ++i )
				{
					new (&m_data[ i ]) T();
				}
			}
		}
		else if ( size < m_size )
		{
			if ( !IsPOD< T >::value )
			{
				for ( UINT i = size; i < m_size; ++i )
				{
					m_data[ i ].~T();
				}
			}
		}
		m_size = size;
	}

	void Reserve( UINT const size )
	{
		if ( m_allocSize < size )
		{
			Reallocate( size );
		}
	}

	void Add( T const& object )
	{
		if ( m_allocSize < m_size + 1 )
		{
			Reallocate( ( m_allocSize + 1 ) << 1 );
		}

		m_data[ m_size ] = object;
		++m_size;
	}
	void Add()
	{
		if ( m_allocSize < m_size + 1 )
		{
			Reallocate( ( m_allocSize + 1 ) << 1 );
		}
		else
		{
			if ( !IsPOD< T >::value )
			{
				new ( &m_data[ m_size ] ) T();
			}
		}
		++m_size;
	}
	void EraseBack()
	{
		ASSERT( 0 < m_size );
		if ( !IsPOD< T >::value )
		{
			m_data[ m_size - 1 ].~T();
		}
		--m_size;
	}

	void Clear()
	{
		if ( !IsPOD< T >::value )
		{
			for ( UINT i = 0; i < m_size; ++i )
			{
				m_data[ i ].~T();
			}
		}

		m_size = 0;
	}

	void Free()
	{
		Reallocate( 0 );
	}

	FORCE_INLINE T* begin() 
	{ 
		return &m_data[0]; 
	}
	FORCE_INLINE T const* begin() const 
	{ 
		return &m_data[0]; 
	}
	FORCE_INLINE T* end() 
	{ 
		return &m_data[m_size]; 
	}
	FORCE_INLINE T const* end() const 
	{ 
		return &m_data[m_size]; 
	}

	FORCE_INLINE T* Data()
	{
		return m_data;
	}
	FORCE_INLINE T const* Data() const
	{
		return m_data;
	}

	FORCE_INLINE T& GetAt( UINT i )
	{
		ASSERT( i < m_size );
		return m_data[ i ];
	}
	FORCE_INLINE T const& GetAt( UINT i ) const
	{
		ASSERT( i < m_size );
		return m_data[ i ];
	}

	FORCE_INLINE T& operator[]( UINT i )
	{
		ASSERT( i < m_size );
		return m_data[ i ];
	}
	FORCE_INLINE T const& operator[]( UINT i ) const
	{
		ASSERT( i < m_size );
		return m_data[ i ];
	}

	FORCE_INLINE UINT Size() const
	{
		return m_size;
	}
};
