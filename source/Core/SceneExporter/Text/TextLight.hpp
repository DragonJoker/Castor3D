/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextLight_H___
#define ___CSE_TextLight_H___

#include <Castor3D/Scene/Light/Light.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< Light >
		: public TextWriterT< Light >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( Light const & overlay
			, StringStream & file )override;
	};
}

#endif
