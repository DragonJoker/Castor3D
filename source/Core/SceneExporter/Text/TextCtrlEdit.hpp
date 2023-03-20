/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextEditCtrl_H___
#define ___CSE_TextEditCtrl_H___

#include <Castor3D/Gui/Controls/CtrlEdit.hpp>
#include <Castor3D/Gui/Theme/StyleEdit.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::EditCtrl >
		: public TextWriterT< castor3d::EditCtrl >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::EditCtrl const & overlay
			, castor::StringStream & file )override;
	};

	template<>
	class TextWriter< castor3d::EditStyle >
		: public TextWriterT< castor3d::EditStyle >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & fontName );
		bool operator()( castor3d::EditStyle const & overlay
			, castor::StringStream & file )override;

	private:
		String m_fontName;
	};
}

#endif
