/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextPass_H___
#define ___CSE_TextPass_H___

#include <Castor3D/Material/Pass/Pass.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Pass >
		: public TextWriterT< castor3d::Pass >
	{
	public:
		explicit TextWriter( String const & tabs
			, Path const & folder
			, String const & subfolder );
		bool operator()( castor3d::Pass const & pass
			, castor::StringStream & file )override;

	private:
		Path m_folder;
		String m_subfolder;
	};
}

#endif
