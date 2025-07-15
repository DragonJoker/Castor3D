/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextDirectionalLight_H___
#define ___CSE_TextDirectionalLight_H___

#include <Castor3D/Scene/Light/DirectionalLight.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< DirectionalLight >
		: public TextWriterT< DirectionalLight >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( DirectionalLight const & light
			, StringStream & file )override;
	};
}

#endif
