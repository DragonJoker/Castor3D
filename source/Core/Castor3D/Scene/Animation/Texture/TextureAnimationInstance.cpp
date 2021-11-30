#include "Castor3D/Scene/Animation/Texture/TextureAnimationInstance.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimationKeyFrame.hpp"
#include "Castor3D/Scene/Animation/AnimatedTexture.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	TextureAnimationInstance::TextureAnimationInstance( AnimatedTexture & object, TextureAnimation & animation )
		: AnimationInstance{ object, animation }
		, m_animatedTexture{ object }
		, m_textureAnimation{ animation }
	{
	}

	void TextureAnimationInstance::doUpdate()
	{
		auto translate = m_textureAnimation.getTranslate( m_currentTime );
		auto rotate = m_textureAnimation.getRotate( m_currentTime );
		auto scale = m_textureAnimation.getScale( m_currentTime );

		m_transform.translate->x = translate->x;
		m_transform.translate->y = translate->y;
		m_transform.translate->z = translate->z;

		m_transform.rotate = rotate;

		m_transform.scale->x = scale->x;
		m_transform.scale->y = scale->y;
		m_transform.scale->z = scale->z;

		if ( m_first )
		{
			m_prev = m_textureAnimation.isEmpty() ? m_textureAnimation.end() : m_textureAnimation.begin();
			m_curr = m_textureAnimation.isEmpty() ? m_textureAnimation.end() : m_textureAnimation.begin() + 1;
			setLooped( !m_textureAnimation.isEmpty() );
			m_first = false;
		}

		if ( !m_textureAnimation.isEmpty() )
		{
			m_textureAnimation.findKeyFrame( m_currentTime
				, m_prev
				, m_curr );
			m_ratio = float( ( m_currentTime - ( *m_prev )->getTimeIndex() ).count() ) / float( ( ( *m_curr )->getTimeIndex() - ( *m_prev )->getTimeIndex() ).count() );
			m_tile = static_cast< TextureAnimationKeyFrame const & >( *( *m_curr ) ).getTile();
		}
	}

	//*************************************************************************************************
}
