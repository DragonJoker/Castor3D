/*
See LICENSE file in root folder
*/
#ifndef ___CU_TextPoint_H___
#define ___CU_TextPoint_H___

#include "CastorUtils/Math/Point.hpp"

#include "CastorUtils/Data/TextWriter.hpp"

namespace castor
{
	template< typename ValueT >
	class TextWriter< Point< ValueT, 1u > >
		: public TextWriterT< Point< ValueT, 1u > >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( Point< ValueT, 1u > const & object, StringStream & file )override;
	};

	template< typename ValueT >
	class TextWriter< Point< ValueT, 2u > >
		: public TextWriterT< Point< ValueT, 2u > >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( Point< ValueT, 2u > const & object, StringStream & file )override;
	};

	template< typename ValueT >
	class TextWriter< Point< ValueT, 3u > >
		: public TextWriterT< Point< ValueT, 3u > >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( Point< ValueT, 3u > const & object, StringStream & file )override;
	};

	template< typename ValueT >
	class TextWriter< Point< ValueT, 4u > >
		: public TextWriterT< Point< ValueT, 4u > >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( Point< ValueT, 4u > const & object, StringStream & file )override;
	};
}
#include "TextPoint.inl"

#endif
