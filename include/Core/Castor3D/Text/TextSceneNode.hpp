/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextSceneNode_H___
#define ___C3D_TextSceneNode_H___

#include "Castor3D/Scene/SceneNode.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::SceneNode >
		: public TextWriterT< castor3d::SceneNode >
	{
	public:
		C3D_API explicit TextWriter( castor::String const & tabs
			, float scale = 1.0f );
		C3D_API bool operator()( castor3d::SceneNode const & node
			, castor::StringStream & file )override;

	private:
		float m_scale;
	};
}

#endif
