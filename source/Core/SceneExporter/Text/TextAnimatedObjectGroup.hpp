/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextAnimatedObjectGroup_H___
#define ___CSE_TextAnimatedObjectGroup_H___

#include <Castor3D/Scene/Animation/AnimatedObjectGroup.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::AnimatedObjectGroup >
		: public TextWriterT< castor3d::AnimatedObjectGroup >
	{
	public:
		explicit TextWriter( String const & tabs );
		virtual bool operator()( castor3d::AnimatedObjectGroup const & group
			, castor::StringStream & file )override;
	};
}

#endif

