/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextPhongPass_H___
#define ___C3D_TextPhongPass_H___

#include "Castor3D/Material/Pass/PhongPass.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::PhongPass >
		: public TextWriterT< castor3d::PhongPass >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs
			, String const & subfolder );
		C3D_API bool operator()( castor3d::PhongPass const & pass
			, TextFile & file )override;

	private:
		String m_subfolder;
	};
}

#endif
