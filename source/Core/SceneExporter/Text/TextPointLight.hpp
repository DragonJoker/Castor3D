/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextPointLight_H___
#define ___CSE_TextPointLight_H___

#include <Castor3D/Scene/Light/PointLight.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::PointLight >
		: public TextWriterT< castor3d::PointLight >
	{
	public:
		explicit TextWriter( castor::String const & tabs );
		bool operator()( castor3d::PointLight const & light
			, castor::StringStream & file )override;
	};
}

#endif
