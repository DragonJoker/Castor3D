/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextSkyboxBackground_H___
#define ___CSE_TextSkyboxBackground_H___

#include <Castor3D/Scene/Background/Skybox.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::SkyboxBackground >
		: public TextWriterT< castor3d::SkyboxBackground >
	{
	public:
		explicit TextWriter( castor::String const & tabs
			, Path const & folder );
		bool operator()( castor3d::SkyboxBackground const & overlay
			, castor::StringStream & file )override;

	private:
		Path m_folder;
	};
}

#endif
