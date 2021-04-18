#include "Castor3D/Scene/Animation/Texture/TextureAnimationInstance.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"
#include "Castor3D/Scene/Animation/AnimatedTexture.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	TextureAnimationInstance::TextureAnimationInstance( AnimatedTexture & object, TextureAnimation & animation )
		: AnimationInstance{ object, animation }
		, m_animatedTexture{ object }
		, m_textureAnimation{ animation }
		, m_prev{ animation.isEmpty() ? animation.end() : animation.begin() }
		, m_curr{ animation.isEmpty() ? animation.end() : animation.begin() + 1 }
	{
	}

	TextureAnimationInstance::~TextureAnimationInstance()
	{
	}

	void TextureAnimationInstance::doUpdate()
	{
		m_animatedTexture.getTexture().setAnimationTransform( m_textureAnimation.getTranslate( m_currentTime )
			, m_textureAnimation.getRotate( m_currentTime )
			, m_textureAnimation.getScale( m_currentTime ) );
	}

	//*************************************************************************************************
}
