/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextAnimatedObjectGroup_H___
#define ___CSE_TextAnimatedObjectGroup_H___

#include <Castor3D/Scene/Animation/AnimatedObjectGroup.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< AnimatedObjectGroup >
		: public TextWriterT< AnimatedObjectGroup >
	{
	public:
		explicit TextWriter( String const & tabs );
		virtual bool operator()( AnimatedObjectGroup const & group
			, StringStream & file )override;
	};
}

#endif

