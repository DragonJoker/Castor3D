/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextSceneNodeAnimation_H___
#define ___CSE_TextSceneNodeAnimation_H___

#include <Castor3D/Scene/Animation/SceneNodeAnimation.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< SceneNodeAnimation >
		: public TextWriterT< SceneNodeAnimation >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( SceneNodeAnimation const & object
			, StringStream & file )override;
	};
}

#endif
