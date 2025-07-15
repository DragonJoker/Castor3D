/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextFog_H___
#define ___CSE_TextFog_H___

#include <Castor3D/Scene/Fog.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< Fog >
		: public TextWriterT< Fog >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( Fog const & config
			, StringStream & file )override;
	};
}

#endif
