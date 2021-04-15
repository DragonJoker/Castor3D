/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextSpecularGlossinessPbrPass_H___
#define ___C3D_TextSpecularGlossinessPbrPass_H___

#include "Castor3D/Material/Pass/SpecularGlossinessPbrPass.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::SpecularGlossinessPbrPass >
		: public TextWriterT< castor3d::SpecularGlossinessPbrPass >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs
			, Path const & folder
			, String const & subfolder );
		C3D_API bool operator()( castor3d::SpecularGlossinessPbrPass const & pass
			, StringStream & file )override;

	private:
		Path m_folder;
		String m_subfolder;
	};
}

#endif
