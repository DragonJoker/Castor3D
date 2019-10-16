#include "Castor3D/Texture/TextureUnit.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"
#include "Castor3D/Texture/Sampler.hpp"

#include <CastorUtils/Graphics/Image.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		String writeMask( uint32_t mask )
		{
			auto stream = castor::makeStringStream();
			stream << cuT( "0x" ) << std::hex << std::setw( 8u ) << std::setfill( cuT( '0' ) ) << mask;
			return stream.str();
		}

		void doUpdateShift( castor::Point2ui & mask )
		{
			if ( mask[0] )
			{
				for ( uint32_t i = 0u; i < 4u; ++i )
				{
					if ( ( mask[0] >> ( i * 8u ) ) & 0x01 )
					{
						mask[1] = i;
						i = 4u;
					}
				}
			}
		}
	}

	TextureUnit::TextWriter::TextWriter( String const & tabs, MaterialType type )
		: castor::TextWriter< TextureUnit >{ tabs }
		, m_type{ type }
	{
	}

	bool TextureUnit::TextWriter::operator()( TextureUnit const & unit, TextFile & file )
	{
		bool result = true;

		if ( unit.isTextured() && unit.getTexture() )
		{
			auto texture = unit.getTexture();
			auto image = texture->getDefaultImage().toString();

			if ( !image.empty() || !texture->getDefaultImage().isStaticSource() )
			{
				if ( result )
				{
					result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "texture_unit\n" ) ) > 0
						&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
				}

				if ( result && unit.getSampler() && unit.getSampler()->getName() != cuT( "Default" ) )
				{
					result = file.writeText( m_tabs + cuT( "\tsampler \"" ) + unit.getSampler()->getName() + cuT( "\"\n" ) ) > 0;
					castor::TextWriter< TextureUnit >::checkError( result, "TextureUnit sampler" );
				}

				if ( result && unit.getTexture()->getMipmapCount() > 1 )
				{
					result = file.writeText( m_tabs + cuT( "\tlevels_count " ) + string::toString( unit.getTexture()->getMipmapCount(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
					castor::TextWriter< TextureUnit >::checkError( result, "TextureUnit mip level count" );
				}

				if ( result  )
				{
					if ( !texture->getDefaultImage().isStaticSource() )
					{
						if ( unit.getRenderTarget() )
						{
							result = RenderTarget::TextWriter( m_tabs + cuT( "\t" ) )( *unit.getRenderTarget(), file );
						}
					}
					else
					{
						Path relative{ Scene::TextWriter::copyFile( Path{ image }, file.getFilePath(), Path{ cuT( "Textures" ) } ) };
						String path = relative;
						string::replace( path, cuT( "\\" ), cuT( "/" ) );
						result = file.writeText( m_tabs + cuT( "\timage \"" ) + path + cuT( "\"\n" ) ) > 0;
						castor::TextWriter< TextureUnit >::checkError( result, "TextureUnit image" );
					}
				}

				auto configuration = unit.getConfiguration();

				if ( result && configuration.colourMask[0] )
				{
					switch ( m_type )
					{
					case MaterialType::ePhong:
						result = file.writeText( m_tabs + cuT( "\tdiffuse_mask " ) + writeMask( configuration.colourMask[0] ) + cuT( "\n" ) ) > 0;
						break;
					case MaterialType::eMetallicRoughness:
					case MaterialType::eSpecularGlossiness:
						result = file.writeText( m_tabs + cuT( "\talbedo_mask " ) + writeMask( configuration.colourMask[0] ) + cuT( "\n" ) ) > 0;
						break;
					}

					castor::TextWriter< TextureUnit >::checkError( result, "TextureUnit diffuse/albedo mask" );
				}

				if ( result && configuration.specularMask[0] )
				{
					switch ( m_type )
					{
					case MaterialType::ePhong:
					case MaterialType::eSpecularGlossiness:
						result = file.writeText( m_tabs + cuT( "\tspecular_mask " ) + writeMask( configuration.specularMask[0] ) + cuT( "\n" ) ) > 0;
						break;
					case MaterialType::eMetallicRoughness:
						result = file.writeText( m_tabs + cuT( "\tmetalness_mask " ) + writeMask( configuration.specularMask[0] ) + cuT( "\n" ) ) > 0;
						break;
					}

					castor::TextWriter< TextureUnit >::checkError( result, "TextureUnit specular/metalness mask" );
				}

				if ( result && configuration.glossinessMask[0] )
				{
					switch ( m_type )
					{
					case MaterialType::ePhong:
						result = file.writeText( m_tabs + cuT( "\tshininess_mask " ) + writeMask( configuration.glossinessMask[0] ) + cuT( "\n" ) ) > 0;
						break;
					case MaterialType::eMetallicRoughness:
						result = file.writeText( m_tabs + cuT( "\troughness_mask " ) + writeMask( configuration.glossinessMask[0] ) + cuT( "\n" ) ) > 0;
						break;
					case MaterialType::eSpecularGlossiness:
						result = file.writeText( m_tabs + cuT( "\tglossiness_mask " ) + writeMask( configuration.glossinessMask[0] ) + cuT( "\n" ) ) > 0;
						break;
					}

					castor::TextWriter< TextureUnit >::checkError( result, "TextureUnit gloss mask" );
				}

				if ( result && configuration.opacityMask[0] )
				{
					result = file.writeText( m_tabs + cuT( "\topacity_mask " ) + writeMask( configuration.opacityMask[0] ) + cuT( "\n" ) ) > 0;
					castor::TextWriter< TextureUnit >::checkError( result, "TextureUnit opacity mask" );
				}

				if ( result && configuration.emissiveMask[0] )
				{
					result = file.writeText( m_tabs + cuT( "\temissive_mask " ) + writeMask( configuration.emissiveMask[0] ) + cuT( "\n" ) ) > 0;
					castor::TextWriter< TextureUnit >::checkError( result, "TextureUnit emissive mask" );
				}

				if ( result && configuration.normalMask[0] )
				{
					result = file.writeText( m_tabs + cuT( "\tnormal_mask " ) + writeMask( configuration.normalMask[0] ) + cuT( "\n" ) ) > 0;
					castor::TextWriter< TextureUnit >::checkError( result, "TextureUnit normal mask" );

					if ( result && configuration.normalFactor != 1.0f )
					{
						result = file.writeText( m_tabs + cuT( "\tnormal_factor " ) + string::toString( configuration.normalFactor, std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
						castor::TextWriter< TextureUnit >::checkError( result, "TextureUnit normal factor" );
					}

					if ( result && configuration.normalGMultiplier != 1.0f )
					{
						result = file.writeText( m_tabs + cuT( "\tnormal_directx true\n" ) ) > 0;
						castor::TextWriter< TextureUnit >::checkError( result, "TextureUnit DirectX normal" );
					}
				}

				if ( result && configuration.heightMask[0] )
				{
					result = file.writeText( m_tabs + cuT( "\theight_mask " ) + writeMask( configuration.heightMask[0] ) + cuT( "\n" ) ) > 0;
					castor::TextWriter< TextureUnit >::checkError( result, "TextureUnit height mask" );

					if ( result && configuration.heightFactor != 1.0f )
					{
						result = file.writeText( m_tabs + cuT( "\theight_factor " ) + string::toString( configuration.heightFactor, std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
						castor::TextWriter< TextureUnit >::checkError( result, "TextureUnit height factor" );
					}
				}

				if ( result && configuration.occlusionMask[0] )
				{
					result = file.writeText( m_tabs + cuT( "\tocclusion_mask " ) + writeMask( configuration.occlusionMask[0] ) + cuT( "\n" ) ) > 0;
					castor::TextWriter< TextureUnit >::checkError( result, "TextureUnit occlusion mask" );
				}

				if ( result && configuration.transmittanceMask[0] )
				{
					result = file.writeText( m_tabs + cuT( "\ttransmittance_mask " ) + writeMask( configuration.transmittanceMask[0] ) + cuT( "\n" ) ) > 0;
					castor::TextWriter< TextureUnit >::checkError( result, "TextureUnit transmittance mask" );
				}

				if ( result && checkFlag( configuration.environment, TextureConfiguration::ReflectionMask ) )
				{
					result = file.writeText( m_tabs + cuT( "\treflection true\n" ) ) > 0;
					castor::TextWriter< TextureUnit >::checkError( result, "TextureUnit reflection" );
				}

				if ( result && checkFlag( configuration.environment, TextureConfiguration::RefractionMask ) )
				{
					result = file.writeText( m_tabs + cuT( "\trefraction true\n" ) ) > 0;
					castor::TextWriter< TextureUnit >::checkError( result, "TextureUnit refraction" );
				}

				if ( result )
				{
					result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
				}
			}
		}

		return result;
	}

	//*********************************************************************************************

	TextureUnit::TextureUnit( Engine & engine )
		: OwnedBy< Engine >( engine )
		, m_autoMipmaps{ false }
		, m_changed{ false }
		, m_sampler{ engine.getDefaultSampler() }
		, m_descriptor
		{
			0u,
			0u,
			1u,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
		}
	{
		m_transformations.setIdentity();
	}

	TextureUnit::~TextureUnit()
	{
		auto renderTarget = m_renderTarget.lock();

		if ( renderTarget )
		{
			getEngine()->getRenderTargetCache().remove( std::move( renderTarget ) );
		}
	}

	void TextureUnit::setTexture( TextureLayoutSPtr texture )
	{
		m_texture = texture;
		m_changed = true;
	}

	bool TextureUnit::initialise()
	{
		RenderTargetSPtr target = m_renderTarget.lock();
		bool result = false;

		if ( target )
		{
			target->initialise();
			m_texture = target->getTexture().getTexture();
			result = true;
		}
		else if ( m_texture )
		{
			result = m_texture->initialise();
			auto sampler = getSampler();
			CU_Require( sampler );
			sampler->initialise();

			if ( result && m_texture->getMipmapCount() > 1u )
			{
				m_texture->generateMipmaps();
			}

			m_descriptor = ashes::WriteDescriptorSet
			{
				0u,
				0u,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				{
					{
						sampler->getSampler(),
						m_texture->getDefaultView(),
						VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					}
				}
			};
		}

		return result;
	}

	void TextureUnit::cleanup()
	{
		if ( m_texture )
		{
			m_texture->cleanup();
		}

		RenderTargetSPtr target = m_renderTarget.lock();

		if ( target )
		{
			target->cleanup();
		}
	}

	VkImageType TextureUnit::getType()const
	{
		CU_Require( m_texture );
		return m_texture->getType();
	}

	castor::String TextureUnit::toString()const
	{
		auto renderTarget = m_renderTarget.lock();

		if ( renderTarget )
		{
			return cuT( "RT_" ) + castor::string::toString( renderTarget->getIndex() );
		}

		CU_Require( m_texture );
		return m_texture->getDefaultImage().toString();
	}

	void TextureUnit::setConfiguration( TextureConfiguration value )
	{
		m_configuration = std::move( value );
		doUpdateShift( m_configuration.colourMask );
		doUpdateShift( m_configuration.specularMask );
		doUpdateShift( m_configuration.glossinessMask );
		doUpdateShift( m_configuration.opacityMask );
		doUpdateShift( m_configuration.emissiveMask );
		doUpdateShift( m_configuration.normalMask );
		doUpdateShift( m_configuration.heightMask );
		doUpdateShift( m_configuration.occlusionMask );
		doUpdateShift( m_configuration.transmittanceMask );
		onChanged( *this );
	}
}
