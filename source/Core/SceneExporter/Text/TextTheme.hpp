/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextTheme_H___
#define ___CSE_TextTheme_H___

#include <Castor3D/Gui/Theme/Theme.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Theme >
		: public TextWriterT< castor3d::Theme >
	{
	public:
		explicit TextWriter( String const & tabs
			, castor3d::Scene const * scene );
		bool operator()( castor3d::Theme const & object
			, castor::StringStream & file )override;

	private:
		castor3d::Scene const * m_scene;
	};
}

#endif
