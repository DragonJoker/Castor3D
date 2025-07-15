/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextShadow_H___
#define ___CSE_TextShadow_H___

#include <Castor3D/Scene/Shadow.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< ShadowConfig >
		: public TextWriterT< ShadowConfig >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( ShadowConfig const & config
			, StringStream & file )override;
	};
}

#endif
