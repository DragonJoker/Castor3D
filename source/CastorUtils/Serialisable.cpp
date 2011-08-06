#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/Serialisable.hpp"
#include "CastorUtils/File.hpp"

using namespace Castor::Utils;

//*************************************************************************************************

Serialisable :: Serialisable()
	:	m_pParentSerialisable( NULL)
	,	m_pFile( NULL)
{
	CreateSerialiseMap();
}

Serialisable :: Serialisable( Serialisable const & p_serialisable)
	:	m_pParentSerialisable( NULL)
	,	m_pFile( NULL)
{
	CreateSerialiseMap();
}

Serialisable & Serialisable :: operator =( Serialisable const & p_serialisable)
{
	CreateSerialiseMap();
	return * this;
}

Serialisable :: ~Serialisable()
{
	m_arrayTypes.clear();
	m_arraySerialisables.clear();
	m_arrayMaps.clear();
}

bool Serialisable :: Serialise( File & p_file)const
{
	m_pFile = & p_file;
	bool l_bReturn = p_file.IsOk();

	if (l_bReturn)
	{
		PreSerialise();
	}

	for (size_t i = 0 ; i < m_arrayTypes.size() && l_bReturn ; i++)
	{
		l_bReturn = p_file.WriteArray<unsigned char>( static_cast<unsigned char *>( m_arrayTypes[i].first), m_arrayTypes[i].second) == m_arrayTypes[i].second;
	}

	for (size_t i = 0 ; i < m_arrayStrings.size() && l_bReturn ; i++)
	{
		l_bReturn = p_file.Write( * m_arrayStrings[i]);
	}

	for (size_t i = 0 ; i < m_arraySerialisables.size() && l_bReturn ; i++)
	{
		l_bReturn = m_arraySerialisables[i]->Serialise( p_file);
	}

	for (size_t i = 0 ; i < m_arrayMaps.size() && l_bReturn ; i++)
	{
		l_bReturn = m_arrayMaps[i]->Serialise( p_file);
	}

	for (size_t i = 0 ; i < m_arrayVectors.size() && l_bReturn ; i++)
	{
		l_bReturn = m_arrayMaps[i]->Serialise( p_file);
	}

	if (l_bReturn)
	{
		PostSerialise();
	}

	return l_bReturn;
}

bool Serialisable :: Unserialise( File & p_file, Serialisable * p_pParent)
{
	m_pParentSerialisable = p_pParent;
	m_pFile = & p_file;
	bool l_bReturn = p_file.IsOk();

	if (l_bReturn)
	{
		PreUnserialise();
	}

	for (size_t i = 0 ; i < m_arrayTypes.size() && l_bReturn ; i++)
	{
		l_bReturn = p_file.ReadArray<unsigned char>( static_cast<unsigned char *>( m_arrayTypes[i].first), m_arrayTypes[i].second) == m_arrayTypes[i].second;
	}

	for (size_t i = 0 ; i < m_arrayStrings.size() && l_bReturn ; i++)
	{
		l_bReturn = p_file.Read( * m_arrayStrings[i]);
	}

	for (size_t i = 0 ; i < m_arraySerialisables.size() && l_bReturn ; i++)
	{
		l_bReturn = m_arraySerialisables[i]->Unserialise( p_file, this);
	}

	for (size_t i = 0 ; i < m_arrayMaps.size() && l_bReturn ; i++)
	{
		l_bReturn = m_arrayMaps[i]->Unserialise( p_file);
	}

	if (l_bReturn)
	{
		PostUnserialise();
	}

	return l_bReturn;
}

//*************************************************************************************************