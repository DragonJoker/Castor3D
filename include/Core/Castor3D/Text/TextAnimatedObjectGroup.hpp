/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextAnimatedObjectGroup_H___
#define ___C3D_TextAnimatedObjectGroup_H___

#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::AnimatedObjectGroup >
		: public TextWriterT< castor3d::AnimatedObjectGroup >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API virtual bool operator()( castor3d::AnimatedObjectGroup const & group
			, TextFile & file )override;
	};
}

#endif

