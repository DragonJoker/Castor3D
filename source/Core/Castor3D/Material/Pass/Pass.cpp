#include "Castor3D/Material/Pass/Pass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/Node/PassRenderNode.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <CastorUtils/Graphics/PixelFormat.hpp>

#include <algorithm>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		void mergeMasks( uint32_t lhs
			, uint32_t & rhs )
		{
			rhs |= lhs;
		}

		void mergeFactors( float lhs
			, float & rhs
			, float ref )
		{
			if ( lhs != rhs )
			{
				rhs = ( lhs == ref
					? rhs
					: lhs );
			}
		}

		void mergeConfigs( TextureConfiguration lhs
			, TextureConfiguration & rhs )
		{
			mergeMasks( lhs.colourMask[0], rhs.colourMask[0] );
			mergeMasks( lhs.specularMask[0], rhs.specularMask[0] );
			mergeMasks( lhs.glossinessMask[0], rhs.glossinessMask[0] );
			mergeMasks( lhs.opacityMask[0], rhs.opacityMask[0] );
			mergeMasks( lhs.emissiveMask[0], rhs.emissiveMask[0] );
			mergeMasks( lhs.normalMask[0], rhs.normalMask[0] );
			mergeMasks( lhs.heightMask[0], rhs.heightMask[0] );
			mergeMasks( lhs.occlusionMask[0], rhs.occlusionMask[0] );
			mergeMasks( lhs.transmittanceMask[0], rhs.transmittanceMask[0] );
			mergeMasks( lhs.needsGammaCorrection, rhs.needsGammaCorrection );
			mergeMasks( lhs.needsYInversion, rhs.needsYInversion );
			mergeFactors( lhs.heightFactor, rhs.heightFactor, 0.1f );
			mergeFactors( lhs.normalFactor, rhs.normalFactor, 1.0f );
			mergeFactors( lhs.normalGMultiplier, rhs.normalGMultiplier, 1.0f );
		}
	}

	//*********************************************************************************************

	Pass::Pass( Material & parent )
		: OwnedBy< Material >{ parent }
		, m_flags{ PassFlag::eLighting
			| ( ( getType() == MaterialType::eMetallicRoughness
				? PassFlag::eMetallicRoughness 
				: ( getType() == MaterialType::eSpecularGlossiness
					? PassFlag::eSpecularGlossiness
					: PassFlag::eNone ) ) ) }
	{
	}

	Pass::~Pass()
	{
		m_textureUnits.clear();
	}

	void Pass::initialise( RenderDevice const & device )
	{
		prepareTextures();

		for ( auto unit : m_textureUnits )
		{
			unit->initialise( device );
		}

		doInitialise();
	}

	void Pass::cleanup()
	{
		doCleanup();

		for ( auto unit : m_textureUnits )
		{
			unit->cleanup();
		}
	}

	void Pass::addTextureUnit( TextureUnitSPtr unit )
	{
		m_textures |= unit->getFlags();
		auto image = unit->toString();
		auto it = std::find_if( m_textureUnits.begin()
			, m_textureUnits.end()
			, [&image]( TextureUnitSPtr lookup )
			{
				return lookup->toString() == image;
			} );

		if ( it == m_textureUnits.end() )
		{
			if ( unit->getConfiguration().heightMask[0] )
			{
				m_heightTextureIndex = uint32_t( m_textureUnits.size() );
			}

			m_textureUnits.push_back( std::move( unit ) );
		}
		else
		{
			auto lhsConfig = unit->getConfiguration();
			unit = *it;
			auto rhsConfig = unit->getConfiguration();
			mergeConfigs( std::move( lhsConfig ), rhsConfig );
			unit->setConfiguration( std::move( rhsConfig ) );

			if ( unit->getConfiguration().heightMask[0] )
			{
				m_heightTextureIndex = uint32_t( std::distance( m_textureUnits.begin(), it ) );
			}
		}

		updateFlag( PassFlag::eAlphaBlending, hasAlphaBlending() );
		updateFlag( PassFlag::eAlphaTest, hasAlphaTest() );
		updateFlag( PassFlag::eBlendAlphaTest, hasBlendAlphaTest() );
		m_texturesReduced = false;
	}

	void Pass::removeTextureUnit( uint32_t index )
	{
		CU_Require( index < m_textureUnits.size() );
		log::info << cuT( "Destroying TextureUnit " ) << index << std::endl;
		auto it = m_textureUnits.begin() + index;
		auto config = ( *it )->getConfiguration();
		m_textureUnits.erase( it );
		remFlag( m_textures, TextureFlag( uint16_t( getFlags( config ) ) ) );
		updateFlag( PassFlag::eAlphaBlending, hasAlphaBlending() );
		updateFlag( PassFlag::eAlphaTest, hasAlphaTest() );
		updateFlag( PassFlag::eBlendAlphaTest, hasBlendAlphaTest() );
		m_texturesReduced = false;
	}

	TextureUnitSPtr Pass::getTextureUnit( uint32_t index )const
	{
		CU_Require( index < m_textureUnits.size() );
		return m_textureUnits[index];
	}

	bool Pass::needsAlphaProcessing()const
	{
		return checkFlag( m_textures, TextureFlag::eOpacity ) || m_opacity < 1.0f;
	}

	bool Pass::hasAlphaBlending()const
	{
		return needsAlphaProcessing()
			&& getAlphaBlendMode() != BlendMode::eNoBlend;
	}

	bool Pass::hasOnlyAlphaBlending()const
	{
		return needsAlphaProcessing()
			&& getAlphaBlendMode() != BlendMode::eNoBlend
			&& getAlphaFunc() == VK_COMPARE_OP_ALWAYS;
	}

	bool Pass::hasAlphaTest()const
	{
		return needsAlphaProcessing()
			&& getAlphaFunc() != VK_COMPARE_OP_ALWAYS;
	}

	bool Pass::hasBlendAlphaTest()const
	{
		return needsAlphaProcessing()
			&& getBlendAlphaFunc() != VK_COMPARE_OP_ALWAYS;
	}

	void Pass::prepareTextures()
	{
		if ( !m_texturesReduced )
		{
			for ( auto & unit : m_textureUnits )
			{
				auto configuration = unit->getConfiguration();

				if ( unit->getRenderTarget() )
				{
					configuration.needsGammaCorrection = 0u;
				}
				else if ( configuration.colourMask[0]
					|| configuration.emissiveMask[0] )
				{
					auto format = unit->getTexture()->getPixelFormat();
					configuration.needsGammaCorrection = !PF::isFloatingPoint( convert( format ) );
				}

				unit->setConfiguration( configuration );
			}

			std::sort( m_textureUnits.begin()
				, m_textureUnits.end()
				, []( TextureUnitSPtr const & lhs, TextureUnitSPtr const & rhs )
				{
					return lhs->getFlags() < rhs->getFlags();
				} );

			m_texturesReduced = true;
		}
	}

	MaterialType Pass::getType()const
	{
		return getOwner()->getType();
	}

	void Pass::setOpacity( float value )
	{
		m_opacity = value;

		if ( needsAlphaProcessing() )
		{
			if ( m_alphaBlendMode == BlendMode::eNoBlend )
			{
				m_alphaBlendMode = BlendMode::eInterpolative;
			}
		}
		else
		{
			m_alphaBlendMode = BlendMode::eNoBlend;
		}

		updateFlag( PassFlag::eAlphaBlending, hasAlphaBlending() );
		updateFlag( PassFlag::eAlphaTest, hasAlphaTest() );
		updateFlag( PassFlag::eBlendAlphaTest, hasBlendAlphaTest() );
		doSetOpacity( value );
		onChanged( *this );
	}

	PassFlags Pass::getPassFlags()const
	{
		return m_flags;
	}

	void Pass::setSubsurfaceScattering( SubsurfaceScatteringUPtr value )
	{
		m_subsurfaceScattering = std::move( value );
		updateFlag( PassFlag::eSubsurfaceScattering, m_subsurfaceScattering != nullptr );
		m_sssConnection = m_subsurfaceScattering->onChanged.connect( [this]( SubsurfaceScattering const & sss )
			{
				onSssChanged( sss );
			} );
		onChanged( *this );
	}

	bool Pass::needsGammaCorrection()const
	{
		return m_textureUnits.end() != std::find_if( m_textureUnits.begin()
			, m_textureUnits.end()
			, []( TextureUnitSPtr unit )
			{
				return unit->getConfiguration().needsGammaCorrection != 0u;
			} );
	}

	TextureUnitPtrArray Pass::getTextureUnits( TextureFlags mask )const
	{
		TextureUnitPtrArray result;

		for ( auto & unit : m_textureUnits )
		{
			auto & config = unit->getConfiguration();

			if ( ( checkFlag( mask, TextureFlag::eAlbedo ) && config.colourMask[0] )
				|| ( checkFlag( mask, TextureFlag::eSpecular ) && config.specularMask[0] )
				|| ( checkFlag( mask, TextureFlag::eGlossiness ) && config.glossinessMask[0] )
				|| ( checkFlag( mask, TextureFlag::eOpacity ) && config.opacityMask[0] )
				|| ( checkFlag( mask, TextureFlag::eEmissive ) && config.emissiveMask[0] )
				|| ( checkFlag( mask, TextureFlag::eNormal ) && config.normalMask[0] )
				|| ( checkFlag( mask, TextureFlag::eHeight ) && config.heightMask[0] )
				|| ( checkFlag( mask, TextureFlag::eOcclusion ) && config.occlusionMask[0] ) 
				|| ( checkFlag( mask, TextureFlag::eTransmittance ) && config.transmittanceMask[0] ) )
			{
				result.push_back( unit );
			}
		}

		return result;
	}

	TextureFlagsArray Pass::getTextures( TextureFlags mask )const
	{
		auto units = getTextureUnits( mask );
		TextureFlagsArray result;

		for ( auto & unit : units )
		{
			result.push_back( { unit->getFlags(), unit->getId() } );
		}

		return result;
	}

	uint32_t Pass::getTextureUnitsCount( TextureFlags mask )const
	{
		return uint32_t( getTextureUnits( mask ).size() );
	}

	void Pass::onSssChanged( SubsurfaceScattering const & sss )
	{
		onChanged( *this );
	}
}
