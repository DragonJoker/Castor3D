/*
See LICENSE file in root folder
*/
#ifndef ___CU_TextQuaternion_H___
#define ___CU_TextQuaternion_H___

#include "CastorUtils/Math/Quaternion.hpp"

#include "CastorUtils/Data/TextWriter.hpp"

namespace castor
{
	template< typename ValueT >
	class TextWriter< QuaternionT< ValueT > >
		: public TextWriterT< QuaternionT< ValueT > >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( QuaternionT< ValueT > const & object, TextFile & file )override;
	};
}

#include "TextQuaternion.inl"

#endif
