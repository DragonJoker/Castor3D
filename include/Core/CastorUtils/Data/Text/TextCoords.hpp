/*
See LICENSE file in root folder
*/
#ifndef ___CU_TextCoords_H___
#define ___CU_TextCoords_H___

#include "CastorUtils/Math/Coords.hpp"

#include "CastorUtils/Data/TextWriter.hpp"

namespace castor
{
	template< typename ValueT >
	class TextWriter< Coords< ValueT, 1u > >
		: public TextWriterT< Coords< ValueT, 1u > >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( Coords< ValueT, 1u > const & object, StringStream & file )override;
	};

	template< typename ValueT >
	class TextWriter< Coords< ValueT, 2u > >
		: public TextWriterT< Coords< ValueT, 2u > >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( Coords< ValueT, 2u > const & object, StringStream & file )override;
	};

	template< typename ValueT >
	class TextWriter< Coords< ValueT, 3u > >
		: public TextWriterT< Coords< ValueT, 3u > >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( Coords< ValueT, 3u > const & object, StringStream & file )override;
	};

	template< typename ValueT >
	class TextWriter< Coords< ValueT, 4u > >
		: public TextWriterT< Coords< ValueT, 4u > >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( Coords< ValueT, 4u > const & object, StringStream & file )override;
	};
}

#include "TextCoords.inl"

#endif
