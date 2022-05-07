/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextRsmConfig_H___
#define ___CSE_TextRsmConfig_H___

#include <Castor3D/Scene/Light/LightModule.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::RsmConfig >
		: public TextWriterT< castor3d::RsmConfig >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::RsmConfig const & config
			, castor::StringStream & file )override;
	};
}

#endif
