#include "Castor3D/Material/Pass/PBR/SpecularGlossinessPbrPass.hpp"

#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"

using namespace castor;

namespace castor3d
{
	SpecularGlossinessPbrPass::SpecularGlossinessPbrPass( Material & parent )
		: Pass{ parent }
		, m_diffuse{ RgbColour::fromRGBA( 0xFFFFFFFF ) }
		, m_specular{ RgbColour::fromRGBA( 0xFFFFFFFF ) }
	{
	}

	SpecularGlossinessPbrPass::~SpecularGlossinessPbrPass()
	{
	}

	PassSPtr SpecularGlossinessPbrPass::create( Material & parent )
	{
		return std::make_shared< SpecularGlossinessPbrPass >( parent );
	}

	void SpecularGlossinessPbrPass::accept( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getId() );

		data.colourDiv->r = getDiffuse().red();
		data.colourDiv->g = getDiffuse().green();
		data.colourDiv->b = getDiffuse().blue();
		data.colourDiv->a = 1.0f - getGlossiness();
		data.specDiv->r = getSpecular().red();
		data.specDiv->g = getSpecular().green();
		data.specDiv->b = getSpecular().blue();
		data.specDiv->a = castor::point::length( castor::Point3f{ getSpecular().constPtr() } );

		doFillData( data );
	}

	void SpecularGlossinessPbrPass::doInitialise()
	{
	}

	void SpecularGlossinessPbrPass::doCleanup()
	{
	}

	void SpecularGlossinessPbrPass::doSetOpacity( float value )
	{
	}

	void SpecularGlossinessPbrPass::doPrepareTextures( TextureUnitPtrArray & result )
	{
		doJoinDifOpa( result, cuT( "AlbOpa" ) );
		doJoinSpcGls( result );
	}

	void SpecularGlossinessPbrPass::doJoinSpcGls( TextureUnitPtrArray & result )
	{
		doMergeImages( TextureFlag::eSpecular
			, offsetof( TextureConfiguration, specularMask )
			, 0x00FFFFFF
			, TextureFlag::eGlossiness
			, offsetof( TextureConfiguration, glossinessMask )
			, 0xFF000000
			, cuT( "SpcGls" )
			, result );
	}
}
