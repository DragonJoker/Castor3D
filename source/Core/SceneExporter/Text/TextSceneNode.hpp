/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextSceneNode_H___
#define ___CSE_TextSceneNode_H___

#include <Castor3D/Scene/SceneNode.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::SceneNode >
		: public TextWriterT< castor3d::SceneNode >
	{
	public:
		explicit TextWriter( castor::String const & tabs
			, float scale = 1.0f );
		bool operator()( castor3d::SceneNode const & node
			, castor::StringStream & file )override;

	private:
		float m_scale;
	};
}

#endif
