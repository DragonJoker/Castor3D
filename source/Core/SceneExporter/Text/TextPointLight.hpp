/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextPointLight_H___
#define ___CSE_TextPointLight_H___

#include <Castor3D/Scene/Light/PointLight.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< PointLight >
		: public TextWriterT< PointLight >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( PointLight const & light
			, StringStream & file )override;
	};
}

#endif
