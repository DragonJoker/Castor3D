/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextSceneNode_H___
#define ___CSE_TextSceneNode_H___

#include <Castor3D/Scene/SceneNode.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< SceneNode >
		: public TextWriterT< SceneNode >
	{
	public:
		explicit TextWriter( String const & tabs
			, float scale = 1.0f );
		bool operator()( SceneNode const & node
			, StringStream & file )override;

	private:
		float m_scale;
	};
}

#endif
