#include "Castor3D/Material/Pass/MetallicRoughnessPbrPass.hpp"

#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"

using namespace castor;

namespace castor3d
{
	MetallicRoughnessPbrPass::MetallicRoughnessPbrPass( Material & parent )
		: Pass{ parent }
		, m_albedo{ RgbColour::fromRGBA( 0xFFFFFFFF ) }
	{
	}

	MetallicRoughnessPbrPass::~MetallicRoughnessPbrPass()
	{
	}

	void MetallicRoughnessPbrPass::accept( PassBuffer & buffer )const
	{
		buffer.visit( *this );
	}

	void MetallicRoughnessPbrPass::doInitialise()
	{
	}

	void MetallicRoughnessPbrPass::doCleanup()
	{
	}

	void MetallicRoughnessPbrPass::doSetOpacity( float value )
	{
	}

	void MetallicRoughnessPbrPass::doPrepareTextures( TextureUnitPtrArray & result )
	{
		doJoinDifOpa( result, cuT( "AlbOpa" ) );
		doJoinMtlRgh( result );
	}

	void MetallicRoughnessPbrPass::doJoinMtlRgh( TextureUnitPtrArray & result )
	{
		doMergeImages( TextureFlag::eMetalness
			, offsetof( TextureConfiguration, specularMask )
			, 0x00FF0000
			, TextureFlag::eRoughness
			, offsetof( TextureConfiguration, glossinessMask )
			, 0x0000FF00
			, cuT( "MtlRgh" )
			, result );
	}
}
