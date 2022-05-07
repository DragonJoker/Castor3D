/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextShadow_H___
#define ___CSE_TextShadow_H___

#include <Castor3D/Scene/Shadow.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::ShadowConfig >
		: public TextWriterT< castor3d::ShadowConfig >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::ShadowConfig const & config
			, StringStream & file )override;
	};
}

#endif
