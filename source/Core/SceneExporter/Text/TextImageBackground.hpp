/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextImageBackground_H___
#define ___CSE_TextImageBackground_H___

#include <Castor3D/Scene/Background/Image.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::ImageBackground >
		: public TextWriterT< castor3d::ImageBackground >
	{
	public:
		explicit TextWriter( String const & tabs
			, castor::Path const & folder );
		bool operator()( castor3d::ImageBackground const & overlay
			, castor::StringStream & file )override;

	private:
		castor::Path const & m_folder;
	};
}

#endif
