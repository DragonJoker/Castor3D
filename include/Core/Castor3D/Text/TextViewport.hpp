/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextViewport_H___
#define ___C3D_TextViewport_H___

#include "Castor3D/Render/Viewport.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Viewport >
		: public TextWriterT< castor3d::Viewport >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API bool operator()( castor3d::Viewport const & viewport
			, StringStream & file )override;
	};
}

#endif
