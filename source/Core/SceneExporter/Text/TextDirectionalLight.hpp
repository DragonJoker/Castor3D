/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextDirectionalLight_H___
#define ___CSE_TextDirectionalLight_H___

#include <Castor3D/Scene/Light/DirectionalLight.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::DirectionalLight >
		: public TextWriterT< castor3d::DirectionalLight >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::DirectionalLight const & light
			, castor::StringStream & file )override;
	};
}

#endif
