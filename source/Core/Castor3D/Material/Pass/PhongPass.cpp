#include "Castor3D/Material/Pass/PhongPass.hpp"

#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"

using namespace castor;

namespace castor3d
{
	PhongPass::PhongPass( Material & p_parent )
		: Pass{ p_parent }
		, m_diffuse{ RgbColour::fromRGBA( 0xFFFFFFFF ) }
		, m_specular{ RgbColour::fromRGBA( 0xFFFFFFFF ) }
	{
	}

	PhongPass::~PhongPass()
	{
	}

	void PhongPass::accept( PassBuffer & p_buffer )const
	{
		p_buffer.visit( *this );
	}

	void PhongPass::doInitialise()
	{
	}

	void PhongPass::doCleanup()
	{
	}

	void PhongPass::doSetOpacity( float p_value )
	{
	}

	void PhongPass::doPrepareTextures( TextureUnitPtrArray & result )
	{
		doJoinDifOpa( result, cuT( "DifOpa" ) );
		doJoinSpcShn( result );
	}

	void PhongPass::doJoinSpcShn( TextureUnitPtrArray & result )
	{
		doMergeImages( TextureFlag::eSpecular
			, offsetof( TextureConfiguration, specularMask )
			, 0x00FFFFFF
			, TextureFlag::eShininess
			, offsetof( TextureConfiguration, glossinessMask )
			, 0xFF000000
			, cuT( "SpcShn" )
			, result );
	}
}
