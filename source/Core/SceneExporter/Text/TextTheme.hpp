/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextTheme_H___
#define ___CSE_TextTheme_H___

#include <Castor3D/Gui/Theme/Theme.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< Theme >
		: public TextWriterT< Theme >
	{
	public:
		explicit TextWriter( String const & tabs
			, Scene const * scene );
		bool operator()( Theme const & object
			, StringStream & file )override;

	private:
		Scene const * m_scene;
	};
}

#endif
