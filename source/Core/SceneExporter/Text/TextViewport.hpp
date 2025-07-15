/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextViewport_H___
#define ___CSE_TextViewport_H___

#include <Castor3D/Render/Viewport.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< Viewport >
		: public TextWriterT< Viewport >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( Viewport const & viewport
			, StringStream & file )override;
	};
}

#endif
