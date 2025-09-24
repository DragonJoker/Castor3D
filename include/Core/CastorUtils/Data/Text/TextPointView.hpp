/*
See LICENSE file in root folder
*/
#ifndef ___CU_TextPointView_H___
#define ___CU_TextPointView_H___

#include "CastorUtils/Math/PointView.hpp"

#include "CastorUtils/Data/TextWriter.hpp"

namespace c3d
{
	template< typename ValueT >
	class TextWriter< PointView< ValueT, 1u > >
		: public TextWriterT< PointView< ValueT, 1u > >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( PointView< ValueT, 1u > const & object, StringStream & file )override;
	};

	template< typename ValueT >
	class TextWriter< PointView< ValueT, 2u > >
		: public TextWriterT< PointView< ValueT, 2u > >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( PointView< ValueT, 2u > const & object, StringStream & file )override;
	};

	template< typename ValueT >
	class TextWriter< PointView< ValueT, 3u > >
		: public TextWriterT< PointView< ValueT, 3u > >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( PointView< ValueT, 3u > const & object, StringStream & file )override;
	};

	template< typename ValueT >
	class TextWriter< PointView< ValueT, 4u > >
		: public TextWriterT< PointView< ValueT, 4u > >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( PointView< ValueT, 4u > const & object, StringStream & file )override;
	};
}

#include "TextPointView.inl"

#endif
