/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextViewport_H___
#define ___CSE_TextViewport_H___

#include <Castor3D/Render/Viewport.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Viewport >
		: public TextWriterT< castor3d::Viewport >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::Viewport const & viewport
			, StringStream & file )override;
	};
}

#endif
