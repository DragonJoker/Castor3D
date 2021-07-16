#include "Castor3D/Material/Pass/PBR/MetallicRoughnessPbrPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/PBR/Shaders/GlslPbrLighting.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

//*************************************************************************************************

namespace castor
{
	template<>
	class TextWriter< castor3d::MetallicRoughnessPbrPass >
		: public TextWriterT< castor3d::MetallicRoughnessPbrPass >
	{
	public:
		TextWriter( String const & tabs
			, Path const & folder
			, String const & subfolder )
			: TextWriterT< castor3d::MetallicRoughnessPbrPass >{ tabs }
			, m_folder{ folder }
			, m_subfolder{ subfolder }
		{
		}

		bool operator()( castor3d::MetallicRoughnessPbrPass const & pass
			, StringStream & file )
		{
			castor3d::log::info << tabs() << cuT( "Writing MetallicRoughnessPbrPass " ) << std::endl;
			return writeNamedSub( file, "albedo", pass.getAlbedo() )
				&& write( file, "roughness", pass.getRoughness() )
				&& write( file, "metallic", pass.getMetallic() );
		}

	private:
		Path m_folder;
		String m_subfolder;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace pbrmr
	{
		enum class Section
			: uint32_t
		{
			ePass = CU_MakeSectionName( 'M', 'T', 'R', 'G' ),
			eTextureUnit = CU_MakeSectionName( 'M', 'R', 'T', 'U' ),
		};

		static castor::String const PassSectionName{ cuT( "pbrmr_pass" ) };
		static castor::String const TextureSectionName{ cuT( "pbrmr_texture_unit" ) };

		CU_ImplementAttributeParser( parserPassAlbedo )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & phongPass = static_cast< MetallicRoughnessPbrPass & >( *parsingContext->pass );
				castor::RgbColour value;
				params[0]->get( value );
				phongPass.setAlbedo( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassMetalness )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & phongPass = static_cast< MetallicRoughnessPbrPass & >( *parsingContext->pass );
				float value;
				params[0]->get( value );
				phongPass.setMetallic( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassRoughness )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & phongPass = static_cast< MetallicRoughnessPbrPass & >( *parsingContext->pass );
				float value;
				params[0]->get( value );
				phongPass.setRoughness( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitChannel )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->textureUnit )
			{
				CU_ParsingError( cuT( "No TextureUnit initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t flags;
				params[0]->get( flags );
				auto textures = TextureFlags( uint16_t( flags ) );

				if ( checkFlag( textures, TextureFlag::eNormal ) )
				{
					parsingContext->textureConfiguration.normalMask[0] = 0x00FFFFFF;
				}

				if ( checkFlag( textures, TextureFlag::eOpacity ) )
				{
					parsingContext->textureConfiguration.opacityMask[0] = 0xFF000000;
				}

				if ( checkFlag( textures, TextureFlag::eHeight ) )
				{
					parsingContext->textureConfiguration.heightMask[0] = 0x00FF0000;
				}

				if ( checkFlag( textures, TextureFlag::eEmissive ) )
				{
					parsingContext->textureConfiguration.emissiveMask[0] = 0x00FFFFFF;
				}

				if ( checkFlag( textures, TextureFlag::eOcclusion ) )
				{
					parsingContext->textureConfiguration.occlusionMask[0] = 0x00FF0000;
				}

				if ( checkFlag( textures, TextureFlag::eTransmittance ) )
				{
					parsingContext->textureConfiguration.transmittanceMask[0] = 0xFF000000;
				}

				if ( checkFlag( textures, TextureFlag::eAlbedo ) )
				{
					parsingContext->textureConfiguration.colourMask[0] = 0x00FFFFFF;
				}

				if ( checkFlag( textures, TextureFlag::eMetalness ) )
				{
					parsingContext->textureConfiguration.metalnessMask[0] = 0x00FF0000;
				}

				if ( checkFlag( textures, TextureFlag::eRoughness ) )
				{
					parsingContext->textureConfiguration.roughnessMask[0] = 0x00FF0000;
				}
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitAlbedoMask )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->textureUnit )
			{
				CU_ParsingError( cuT( "No TextureUnit initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext->textureConfiguration.colourMask[0] );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitMetalnessMask )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->textureUnit )
			{
				CU_ParsingError( cuT( "No TextureUnit initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext->textureConfiguration.metalnessMask[0] );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitRoughnessMask )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->textureUnit )
			{
				CU_ParsingError( cuT( "No TextureUnit initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext->textureConfiguration.roughnessMask[0] );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	castor::String const MetallicRoughnessPbrPass::Type = cuT( "metallic_roughness" );
	castor::String const MetallicRoughnessPbrPass::LightingModel = shader::PbrMRLightingModel::getName();

	MetallicRoughnessPbrPass::MetallicRoughnessPbrPass( Material & parent )
		: Pass{ parent, parent.getEngine()->getPassFactory().getNameId( Type ), PassFlag::eImageBasedLighting }
		, m_albedo{ m_dirty, castor::RgbColour::fromRGBA( 0xFFFFFFFF ) }
		, m_metalness{ m_dirty, 0.0f }
		, m_roughness{ m_dirty, 1.0f }
	{
	}

	MetallicRoughnessPbrPass::~MetallicRoughnessPbrPass()
	{
	}

	PassSPtr MetallicRoughnessPbrPass::create( Material & parent )
	{
		return std::make_shared< MetallicRoughnessPbrPass >( parent );
	}

	castor::AttributeParsersBySection MetallicRoughnessPbrPass::createParsers()
	{
		static UInt32StrMap const textureChannels = []()
		{
			UInt32StrMap result;
			result[getName( TextureFlag::eNormal, true )] = uint32_t( TextureFlag::eNormal );
			result[getName( TextureFlag::eHeight, true )] = uint32_t( TextureFlag::eHeight );
			result[getName( TextureFlag::eOpacity, true )] = uint32_t( TextureFlag::eOpacity );
			result[getName( TextureFlag::eEmissive, true )] = uint32_t( TextureFlag::eEmissive );
			result[getName( TextureFlag::eOcclusion, true )] = uint32_t( TextureFlag::eOcclusion );
			result[getName( TextureFlag::eTransmittance, true )] = uint32_t( TextureFlag::eTransmittance );
			result[getName( TextureFlag::eAlbedo, true )] = uint32_t( TextureFlag::eAlbedo );
			result[getName( TextureFlag::eMetalness, true )] = uint32_t( TextureFlag::eMetalness );
			result[getName( TextureFlag::eRoughness, true )] = uint32_t( TextureFlag::eRoughness );
			return result;
		}();

		castor::AttributeParsersBySection result;

		Pass::addParser( result, uint32_t( pbrmr::Section::ePass ), cuT( "albedo" ), pbrmr::parserPassAlbedo, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
		Pass::addParser( result, uint32_t( pbrmr::Section::ePass ), cuT( "metallic" ), pbrmr::parserPassMetalness, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addParser( result, uint32_t( pbrmr::Section::ePass ), cuT( "roughness" ), pbrmr::parserPassRoughness, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addParser( result, uint32_t( pbrmr::Section::eTextureUnit ), cuT( "albedo_mask" ), pbrmr::parserUnitAlbedoMask, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParser( result, uint32_t( pbrmr::Section::eTextureUnit ), cuT( "metalness_mask" ), pbrmr::parserUnitMetalnessMask, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParser( result, uint32_t( pbrmr::Section::eTextureUnit ), cuT( "roughness_mask" ), pbrmr::parserUnitRoughnessMask, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParser( result, uint32_t( pbrmr::Section::eTextureUnit ), cuT( "channel" ), pbrmr::parserUnitChannel, { castor::makeParameter< castor::ParameterType::eBitwiseOred32BitsCheckedText >( textureChannels ) } );
		Pass::addCommonParsers( uint32_t( pbrmr::Section::ePass )
			, uint32_t( pbrmr::Section::eTextureUnit )
			, result );

		return result;
	}

	castor::StrUInt32Map MetallicRoughnessPbrPass::createSections()
	{
		return
		{
			{ uint32_t( pbrmr::Section::ePass ), pbrmr::PassSectionName },
			{ uint32_t( pbrmr::Section::eTextureUnit ), pbrmr::TextureSectionName },
		};
	}

	void MetallicRoughnessPbrPass::accept( PassBuffer & buffer )const
	{
		auto f0 = castor::RgbColour{ 0.04f, 0.04f, 0.04f } *( 1.0f - getMetallic() ) + getAlbedo() * getMetallic();
		auto data = buffer.getData( getId() );

		data.colourDiv->r = getAlbedo().red();
		data.colourDiv->g = getAlbedo().green();
		data.colourDiv->b = getAlbedo().blue();
		data.colourDiv->a = getRoughness();
		data.specDiv->r = f0.red();
		data.specDiv->g = f0.green();
		data.specDiv->b = f0.blue();
		data.specDiv->a = getMetallic();

		doFillData( data );
	}

	uint32_t MetallicRoughnessPbrPass::getPassSectionID()const
	{
		return uint32_t( pbrmr::Section::ePass );
	}

	uint32_t MetallicRoughnessPbrPass::getTextureSectionID()const
	{
		return uint32_t( pbrmr::Section::eTextureUnit );
	}

	bool MetallicRoughnessPbrPass::writeText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< MetallicRoughnessPbrPass >{ tabs, folder, subfolder }( *this, file );
	}

	void MetallicRoughnessPbrPass::doInitialise()
	{
	}

	void MetallicRoughnessPbrPass::doCleanup()
	{
	}

	void MetallicRoughnessPbrPass::doAccept( PassVisitorBase & vis )
	{
		vis.visit( cuT( "Albedo" )
			, m_albedo );
		vis.visit( cuT( "Metalness" )
			, m_metalness );
		vis.visit( cuT( "Roughness" )
			, m_roughness );
	}

	void MetallicRoughnessPbrPass::doAccept( castor3d::TextureConfiguration & configuration
		, castor3d::PassVisitorBase & vis )
	{
		vis.visit( cuT( "Albedo" ), castor3d::TextureFlag::eAlbedo, configuration.colourMask, 3u );
		vis.visit( cuT( "Metalness" ), castor3d::TextureFlag::eMetalness, configuration.metalnessMask, 1u );
		vis.visit( cuT( "Roughness" ), castor3d::TextureFlag::eRoughness, configuration.roughnessMask, 1u );
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
			, offsetof( TextureConfiguration, metalnessMask )
			, 0x00FF0000
			, TextureFlag::eRoughness
			, offsetof( TextureConfiguration, roughnessMask )
			, 0x0000FF00
			, cuT( "MtlRgh" )
			, result );
	}
}
