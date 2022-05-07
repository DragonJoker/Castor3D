/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextLight_H___
#define ___CSE_TextLight_H___

#include <Castor3D/Scene/Light/Light.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Light >
		: public TextWriterT< castor3d::Light >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::Light const & overlay
			, castor::StringStream & file )override;
	};
}

#endif
