//*************************************************************************************************

template <typename T> class ElementAdder
{
public:
	void operator ()( Serialisable * p_pThis, T * p_element)
	{
		p_pThis->m_arrayTypes.push_back( PtrSizePair( p_element, sizeof( T)));
	}
};
template <> class ElementAdder<Serialisable>
{
public:
	void operator ()( Serialisable * p_pThis, Serialisable * p_element)
	{
		p_pThis->m_arraySerialisables.push_back( p_element);
	}
};
template <> class ElementAdder<String>
{
public:
	void operator ()( Serialisable * p_pThis, String * p_element)
	{
		p_pThis->m_arrayStrings.push_back( p_element);
	}
};

//*************************************************************************************************

template <typename elem>
class MapStrSerialiser : public MapSerialiserBase
{
private:
	std::map< String, shared_ptr<elem> > * m_map;

public:
	MapStrSerialiser( std::map< String, shared_ptr<elem> > * p_map)
		:	m_map( p_map)
	{
	}
	virtual bool Serialise( File & p_file)const
	{
		typedef typename std::map< String, shared_ptr<elem> >::const_iterator const_iterator;
		bool l_bReturn = p_file.Write( m_map->size()) == sizeof( size_t);

		for (const_iterator l_it = m_map->begin() ; l_it != m_map->end() && l_bReturn ; ++l_it)
		{
			l_bReturn = l_it->second->Serialise( p_file);
		}

		return l_bReturn;
	}
	virtual bool Unserialise( File & p_file)
	{
		size_t l_uiSize;
		bool l_bReturn = p_file.Read( l_uiSize) == sizeof( size_t);
		typedef typename std::map< String, shared_ptr<elem> > elem_ptr_str_map;

		for (size_t i = 0 ; i < l_uiSize ; i++)
		{
			shared_ptr<elem> l_pElem( new elem);
			l_bReturn = l_pElem->Unserialise( p_file);
			m_map->insert( typename elem_ptr_str_map::value_type( l_pElem->GetName(), l_pElem));
		}

		return l_bReturn;
	}
};

//*************************************************************************************************

template <typename elem>
class MapRealSerialiser : public MapSerialiserBase
{
private:
	std::map< real, shared_ptr<elem> > * m_map;

public:
	MapRealSerialiser( std::map< real, shared_ptr<elem> > * p_map)
		:	m_map( p_map)
	{
	}
	virtual bool Serialise( File & p_file)const
	{
		bool l_bReturn = p_file.Write( m_map->size()) == sizeof( size_t);
		typedef typename std::map< real, shared_ptr<elem> >::const_iterator const_iterator;

		for (const_iterator l_it = m_map->begin() ; l_it != m_map->end() && l_bReturn ; ++l_it)
		{
			l_bReturn = l_it->second->Serialise( p_file);
		}

		return l_bReturn;
	}
	virtual bool Unserialise( File & p_file)
	{
		size_t l_uiSize;
		bool l_bReturn = p_file.Read( l_uiSize) == sizeof( size_t);
		typedef typename std::map< real, shared_ptr<elem> > elem_ptr_real_map;

		for (size_t i = 0 ; i < l_uiSize ; i++)
		{
			shared_ptr<elem> l_pElem( new elem);
			l_bReturn = l_pElem->Unserialise( p_file);
			m_map->insert( typename elem_ptr_real_map::value_type( l_pElem->GetName(), l_pElem));
		}

		return l_bReturn;
	}
};

//*************************************************************************************************

template <typename elem>
class VectorSerialiser : public VectorSerialiserBase
{
private:
	std::vector< shared_ptr<elem> > * m_array;

public:
	VectorSerialiser( std::vector< shared_ptr<elem> > * p_array)
		:	m_array( p_array)
	{
	}
	virtual bool Serialise( File & p_file)const
	{
		bool l_bReturn = p_file.Write( m_array->size()) == sizeof( size_t);
		typedef typename std::vector< shared_ptr<elem> >::const_iterator const_iterator;

		for (const_iterator l_it = m_array->begin() ; l_it != m_array->end() && l_bReturn ; ++l_it)
		{
			l_bReturn = ( * l_it)->Serialise( p_file);
		}

		return l_bReturn;
	}
	virtual bool Unserialise( File & p_file)
	{
		size_t l_uiSize;
		bool l_bReturn = p_file.Read( l_uiSize) == sizeof( size_t);

		for (size_t i = 0 ; i < l_uiSize ; i++)
		{
			shared_ptr<elem> l_pElem( new elem);
			l_bReturn = l_pElem->Unserialise( p_file);
			m_array->push_back( l_pElem);
		}

		return l_bReturn;
	}
};

//*************************************************************************************************

template <typename T>
inline void Serialisable :: AddElement( T * p_element)
{
	ElementAdder<T>()( this, p_element);
}
template <typename T>
inline void Serialisable :: AddElement( Resources::Buffer<T> * p_element)
{
	m_arrayTypes.push_back( PtrSizePair( p_element->data(), p_element->size()));
}
template <typename T, size_t Count>
inline void Serialisable :: AddElement( Math::Point<T, Count> * p_element)
{
	m_arrayTypes.push_back( PtrSizePair( p_element->ptr(), Count * sizeof( T)));
}
template <typename T, size_t Rows, size_t Columns>
inline void Serialisable :: AddElement( Math::Matrix<T, Rows, Columns> * p_element)
{
	m_arrayTypes.push_back( PtrSizePair( p_element->ptr(), Rows * Columns * sizeof( T)));
}
template <typename T>
inline void Serialisable :: AddElement( std::map< String, shared_ptr<T> > * p_map)
{
	m_arrayMaps.push_back( shared_ptr<MapSerialiserBase>( new MapStrSerialiser<T>( p_map)));
}
template <typename T>
inline void Serialisable :: AddElement( std::map< real, shared_ptr<T> > * p_map)
{
	m_arrayMaps.push_back( shared_ptr<MapSerialiserBase>( new MapRealSerialiser<T>( p_map)));
}
template <typename T>
inline void Serialisable :: AddElement( std::vector< shared_ptr<T> > * p_map)
{
	m_arrayVectors.push_back( shared_ptr<VectorSerialiserBase>( new VectorSerialiser<T>( p_map)));
}
