#include "Castor3D/Material/Pass/PBR/PbrPass.hpp"

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
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

//*************************************************************************************************

namespace castor
{
	template<>
	class TextWriter< castor3d::PbrPass >
		: public TextWriterT< castor3d::PbrPass >
	{
	public:
		TextWriter( String const & tabs
			, Path const & folder
			, String const & subfolder )
			: TextWriterT< castor3d::PbrPass >{ tabs }
			, m_folder{ folder }
			, m_subfolder{ subfolder }
		{
		}

		bool operator()( castor3d::PbrPass const & pass
			, StringStream & file )override
		{
			castor3d::log::info << tabs() << cuT( "Writing PbrPass " ) << std::endl;
			return writeNamedSub( file, "albedo", pass.getAlbedo() )
				&& write( file, "roughness", pass.getRoughness() )
				&& write( file, "metalness", pass.getMetalness() )
				&& writeNamedSub( file, "specular", pass.getSpecular() );
		}

	private:
		Path m_folder;
		String m_subfolder;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace pbr
	{
		namespace
		{
			static float constexpr DefaultMetalness = 0.0f;
			static castor::Point3f constexpr DefaultSpecular = { 0.0f, 0.0f, 0.0f };

			enum class Section
				: uint32_t
			{
				ePass = CU_MakeSectionName( 'M', 'T', 'R', 'G' ),
				eTextureUnit = CU_MakeSectionName( 'M', 'R', 'T', 'U' ),
				eTextureRemap = CU_MakeSectionName( 'M', 'R', 'T', 'R' ),
				eTextureRemapChannel = CU_MakeSectionName( 'M', 'R', 'T', 'C' ),
			};

			static castor::String const PassSectionName{ cuT( "pbr_pass" ) };
			static castor::String const TextureSectionName{ cuT( "pbr_texture_unit" ) };
			static castor::String const RemapSectionName{ cuT( "pbr_texture_remap" ) };
			static castor::String const RemapChannelSectionName{ cuT( "pbr_texture_remap_channel" ) };

			CU_ImplementAttributeParser( parserPassAlbedo )
			{
				auto & parsingContext = getParserContext( context );

				if ( !parsingContext.pass )
				{
					CU_ParsingError( cuT( "No Pass initialised." ) );
				}
				else if ( !params.empty() )
				{
					auto & phongPass = static_cast< PbrPass & >( *parsingContext.pass );
					castor::RgbColour value;
					params[0]->get( value );
					phongPass.setAlbedo( value );
				}
			}
			CU_EndAttribute()

				CU_ImplementAttributeParser( parserPassMetalness )
			{
				auto & parsingContext = getParserContext( context );

				if ( !parsingContext.pass )
				{
					CU_ParsingError( cuT( "No Pass initialised." ) );
				}
				else if ( !params.empty() )
				{
					auto & phongPass = static_cast< PbrPass & >( *parsingContext.pass );
					float value;
					params[0]->get( value );
					phongPass.setMetalness( value );
				}
			}
			CU_EndAttribute()

				CU_ImplementAttributeParser( parserPassRoughness )
			{
				auto & parsingContext = getParserContext( context );

				if ( !parsingContext.pass )
				{
					CU_ParsingError( cuT( "No Pass initialised." ) );
				}
				else if ( !params.empty() )
				{
					auto & phongPass = static_cast< PbrPass & >( *parsingContext.pass );
					float value;
					params[0]->get( value );
					phongPass.setRoughness( value );
				}
			}
			CU_EndAttribute()

				CU_ImplementAttributeParser( parserPassGlossiness )
			{
				auto & parsingContext = getParserContext( context );

				if ( !parsingContext.pass )
				{
					CU_ParsingError( cuT( "No Pass initialised." ) );
				}
				else if ( !params.empty() )
				{
					auto & phongPass = static_cast< PbrPass & >( *parsingContext.pass );
					float value;
					params[0]->get( value );
					phongPass.setGlossiness( value );
				}
			}
			CU_EndAttribute()

				CU_ImplementAttributeParser( parserPassSpecular )
			{
				auto & parsingContext = getParserContext( context );

				if ( !parsingContext.pass )
				{
					CU_ParsingError( cuT( "No Pass initialised." ) );
				}
				else if ( !params.empty() )
				{
					auto & phongPass = static_cast< PbrPass & >( *parsingContext.pass );
					castor::RgbColour value;
					params[0]->get( value );
					phongPass.setSpecular( value );
				}
			}
			CU_EndAttribute()

				CU_ImplementAttributeParser( parserUnitChannel )
			{
				auto & parsingContext = getParserContext( context );

				if ( !params.empty() )
				{
					uint32_t flags;
					params[0]->get( flags );
					auto textures = TextureFlags( uint16_t( flags ) );

					if ( checkFlag( textures, TextureFlag::eNormal ) )
					{
						parsingContext.textureConfiguration.normalMask[0] = 0x00FFFFFF;
					}

					if ( checkFlag( textures, TextureFlag::eOpacity ) )
					{
						parsingContext.textureConfiguration.opacityMask[0] = 0xFF000000;
					}

					if ( checkFlag( textures, TextureFlag::eHeight ) )
					{
						parsingContext.textureConfiguration.heightMask[0] = 0x00FF0000;
					}

					if ( checkFlag( textures, TextureFlag::eEmissive ) )
					{
						parsingContext.textureConfiguration.emissiveMask[0] = 0x00FFFFFF;
					}

					if ( checkFlag( textures, TextureFlag::eOcclusion ) )
					{
						parsingContext.textureConfiguration.occlusionMask[0] = 0x00FF0000;
					}

					if ( checkFlag( textures, TextureFlag::eTransmittance ) )
					{
						parsingContext.textureConfiguration.transmittanceMask[0] = 0xFF000000;
					}

					if ( checkFlag( textures, TextureFlag::eAlbedo ) )
					{
						parsingContext.textureConfiguration.colourMask[0] = 0x00FFFFFF;
					}

					if ( checkFlag( textures, TextureFlag::eMetalness ) )
					{
						parsingContext.textureConfiguration.metalnessMask[0] = 0x00FF0000;
					}

					if ( checkFlag( textures, TextureFlag::eRoughness ) )
					{
						parsingContext.textureConfiguration.roughnessMask[0] = 0x00FF0000;
					}

					if ( checkFlag( textures, TextureFlag::eSpecular ) )
					{
						parsingContext.textureConfiguration.specularMask[0] = 0x00FFFFFF;
					}
				}
			}
			CU_EndAttribute()

				CU_ImplementAttributeParser( parserUnitAlbedoMask )
			{
				auto & parsingContext = getParserContext( context );

				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( parsingContext.textureConfiguration.colourMask[0] );
				}
			}
			CU_EndAttribute()

				CU_ImplementAttributeParser( parserUnitMetalnessMask )
			{
				auto & parsingContext = getParserContext( context );

				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( parsingContext.textureConfiguration.metalnessMask[0] );
				}
			}
			CU_EndAttribute()

				CU_ImplementAttributeParser( parserUnitRoughnessMask )
			{
				auto & parsingContext = getParserContext( context );

				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( parsingContext.textureConfiguration.roughnessMask[0] );
				}
			}
			CU_EndAttribute()

				CU_ImplementAttributeParser( parserUnitGlossinessMask )
			{
				auto & parsingContext = getParserContext( context );

				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( parsingContext.textureConfiguration.glossinessMask[0] );
				}
			}
			CU_EndAttribute()

				CU_ImplementAttributeParser( parserUnitSpecularMask )
			{
				auto & parsingContext = getParserContext( context );

				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( parsingContext.textureConfiguration.specularMask[0] );
				}
			}
			CU_EndAttribute()

				CU_ImplementAttributeParser( parserSceneImportTexRemap )
			{
				auto & parsingContext = getParserContext( context );
				parsingContext.sceneImportConfig.textureRemaps.clear();
				parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.end();
			}
			CU_EndAttributePush( pbr::Section::eTextureRemap )

				CU_ImplementAttributeParser( parserTexRemapSpecular )
			{
				auto & parsingContext = getParserContext( context );
				parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( TextureFlag::eSpecular, TextureConfiguration{} ).first;
				parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
			}
			CU_EndAttributePush( pbr::Section::eTextureRemapChannel )

				CU_ImplementAttributeParser( parserTexRemapNormal )
			{
				auto & parsingContext = getParserContext( context );
				parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.emplace( TextureFlag::eNormal, TextureConfiguration{} ).first;
				parsingContext.sceneImportConfig.textureRemapIt->second = TextureConfiguration{};
			}
			CU_EndAttributePush( pbr::Section::eTextureRemapChannel )

				CU_ImplementAttributeParser( parserTexRemapAlbedoMask )
			{
				auto & parsingContext = getParserContext( context );

				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( parsingContext.sceneImportConfig.textureRemapIt->second.colourMask[0] );
				}
			}
			CU_EndAttribute()

				CU_ImplementAttributeParser( parserTexRemapMetalnessMask )
			{
				auto & parsingContext = getParserContext( context );

				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( parsingContext.sceneImportConfig.textureRemapIt->second.metalnessMask[0] );
				}
			}
			CU_EndAttribute()

				CU_ImplementAttributeParser( parserTexRemapRoughnessMask )
			{
				auto & parsingContext = getParserContext( context );

				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( parsingContext.sceneImportConfig.textureRemapIt->second.roughnessMask[0] );
				}
			}
			CU_EndAttribute()

				CU_ImplementAttributeParser( parserTexRemapGlossinessMask )
			{
				auto & parsingContext = getParserContext( context );

				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( parsingContext.sceneImportConfig.textureRemapIt->second.glossinessMask[0] );
				}
			}
			CU_EndAttribute()

				CU_ImplementAttributeParser( parserTexRemapSpecularMask )
			{
				auto & parsingContext = getParserContext( context );

				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( parsingContext.sceneImportConfig.textureRemapIt->second.specularMask[0] );
				}
			}
			CU_EndAttribute()
		}
	}

	//*********************************************************************************************

	castor::String const PbrPass::Type = cuT( "pbr" );
	castor::String const PbrPass::LightingModel = shader::PbrLightingModel::getName();

	PbrPass::PbrPass( Material & parent
		, PassFlags initialFlags )
		: PbrPass{ parent
		, parent.getEngine()->getPassFactory().getNameId( Type )
		, initialFlags }
	{
	}

	PbrPass::PbrPass( Material & parent
		, PassTypeID typeID
		, PassFlags initialFlags )
		: Pass{ parent
		, typeID
		, initialFlags | PassFlag::eImageBasedLighting }
		, m_albedo{ m_dirty, castor::RgbColour::fromRGBA( 0xFFFFFFFF ) }
		, m_roughness{ m_dirty, 1.0f }
		, m_metalness{ m_dirty, pbr::DefaultMetalness }
		, m_specular{ m_dirty
		, castor::RgbColour::fromComponents( pbr::DefaultSpecular->x
			, pbr::DefaultSpecular->y
			, pbr::DefaultSpecular->z ) }
	{
	}

	PassSPtr PbrPass::create( Material & parent )
	{
		return std::make_shared< PbrPass >( parent );
	}

	castor::AttributeParsers PbrPass::createParsers()
	{
		return createParsers( uint32_t( pbr::Section::ePass )
			, uint32_t( pbr::Section::eTextureUnit )
			, uint32_t( pbr::Section::eTextureRemap )
			, cuT( "pbr_texture_remap_config" )
			, uint32_t( pbr::Section::eTextureRemapChannel ) );
	}

	castor::AttributeParsers PbrPass::createParsers( uint32_t mtlSectionID
		, uint32_t texSectionID
		, uint32_t texRemapSectionID
		, castor::String const & texRemapSectionName
		, uint32_t remapChannelSectionID )
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
			result[getName( TextureFlag::eGlossiness, true )] = uint32_t( TextureFlag::eGlossiness );
			result[getName( TextureFlag::eSpecular, true )] = uint32_t( TextureFlag::eSpecular );
			return result;
		}( );

		castor::AttributeParsers result;

		Pass::addParser( result, mtlSectionID, cuT( "albedo" ), pbr::parserPassAlbedo, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
		Pass::addParser( result, mtlSectionID, cuT( "metalness" ), pbr::parserPassMetalness, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addParser( result, mtlSectionID, cuT( "roughness" ), pbr::parserPassRoughness, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addParser( result, mtlSectionID, cuT( "glossiness" ), pbr::parserPassGlossiness, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addParser( result, mtlSectionID, cuT( "specular" ), pbr::parserPassSpecular, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
		Pass::addParser( result, texSectionID, cuT( "albedo_mask" ), pbr::parserUnitAlbedoMask, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParser( result, texSectionID, cuT( "metalness_mask" ), pbr::parserUnitMetalnessMask, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParser( result, texSectionID, cuT( "roughness_mask" ), pbr::parserUnitRoughnessMask, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParser( result, texSectionID, cuT( "glossiness_mask" ), pbr::parserUnitGlossinessMask, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParser( result, texSectionID, cuT( "specular_mask" ), pbr::parserUnitSpecularMask, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParser( result, texSectionID, cuT( "channel" ), pbr::parserUnitChannel, { castor::makeParameter< castor::ParameterType::eBitwiseOred32BitsCheckedText >( textureChannels ) } );
		Pass::addCommonParsers( mtlSectionID, texSectionID, remapChannelSectionID, result );

		Pass::addParser( result, uint32_t( CSCNSection::eSceneImport ), texRemapSectionName, pbr::parserSceneImportTexRemap );

		Pass::addParser( result, texRemapSectionID, cuT( "specular" ), pbr::parserTexRemapSpecular );
		Pass::addParser( result, texRemapSectionID, cuT( "normal" ), pbr::parserTexRemapNormal );

		Pass::addParser( result, remapChannelSectionID, cuT( "albedo_mask" ), pbr::parserTexRemapAlbedoMask, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParser( result, remapChannelSectionID, cuT( "metalness_mask" ), pbr::parserTexRemapMetalnessMask, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParser( result, remapChannelSectionID, cuT( "roughness_mask" ), pbr::parserTexRemapRoughnessMask, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParser( result, remapChannelSectionID, cuT( "glossiness_mask" ), pbr::parserTexRemapGlossinessMask, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParser( result, remapChannelSectionID, cuT( "specular_mask" ), pbr::parserTexRemapSpecularMask, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );

		return result;
	}

	castor::StrUInt32Map PbrPass::createSections()
	{
		return
		{
			{ uint32_t( pbr::Section::ePass ), pbr::PassSectionName },
			{ uint32_t( pbr::Section::eTextureUnit ), pbr::TextureSectionName },
			{ uint32_t( pbr::Section::eTextureRemap ), pbr::RemapSectionName },
			{ uint32_t( pbr::Section::eTextureRemapChannel ), pbr::RemapChannelSectionName },
		};
	}

	void PbrPass::fillBuffer( PassBuffer & buffer )const
	{
		auto metalness = m_metalnessSet
			? getMetalness()
			: ( m_specularSet
				? float( castor::point::length( castor::Point3f{ getSpecular().constPtr() } ) )
				: pbr::DefaultMetalness );
		auto specular = m_specularSet
			? getSpecular()
			: ( castor::RgbColour{ 0.04f, 0.04f, 0.04f } *( 1.0f - metalness ) + getAlbedo() * metalness );
		auto data = buffer.getData( getId() );

		data.colourDiv->r = getAlbedo().red();
		data.colourDiv->g = getAlbedo().green();
		data.colourDiv->b = getAlbedo().blue();
		data.colourDiv->a = getRoughness();
		data.specDiv->r = specular.red();
		data.specDiv->g = specular.green();
		data.specDiv->b = specular.blue();
		data.specDiv->a = metalness;

		doFillData( data );
	}

	uint32_t PbrPass::getPassSectionID()const
	{
		return uint32_t( pbr::Section::ePass );
	}

	uint32_t PbrPass::getTextureSectionID()const
	{
		return uint32_t( pbr::Section::eTextureUnit );
	}

	bool PbrPass::writeText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< PbrPass >{ tabs, folder, subfolder }( *this, file );
	}

	void PbrPass::doAccept( PassVisitorBase & vis )
	{
		vis.visit( cuT( "Albedo" )
			, m_albedo );
		vis.visit( cuT( "Metalness" )
			, m_metalness );
		vis.visit( cuT( "Roughness" )
			, m_roughness );
		vis.visit( cuT( "Specular" )
			, m_specular );
	}

	void PbrPass::doAccept( castor3d::TextureConfiguration & configuration
		, castor3d::PassVisitorBase & vis )
	{
		vis.visit( cuT( "Albedo" ), castor3d::TextureFlag::eAlbedo, configuration.colourMask, 3u );
		vis.visit( cuT( "Metalness" ), castor3d::TextureFlag::eMetalness, configuration.metalnessMask, 1u );
		vis.visit( cuT( "Roughness" ), castor3d::TextureFlag::eRoughness, configuration.roughnessMask, 1u );
		vis.visit( cuT( "Glossiness" ), castor3d::TextureFlag::eGlossiness, configuration.glossinessMask, 1u );
		vis.visit( cuT( "Specular" ), castor3d::TextureFlag::eSpecular, configuration.specularMask, 3u );
	}

	void PbrPass::doPrepareTextures( TextureUnitPtrArray & result )
	{
		doJoinDifOpa( result, cuT( "AlbOpa" ) );
		doJoinMtlRgh( result );
		doJoinSpcGls( result );
	}

	PassSPtr PbrPass::doClone( Material & material )const
	{
		auto result = std::make_shared< PbrPass >( material );
		result->m_albedo = m_albedo.value();
		result->m_roughness = m_roughness.value();
		result->m_metalness = m_metalness.value();
		result->m_specular = m_specular.value();
		result->m_specularSet = m_specularSet;
		result->m_metalnessSet = m_metalnessSet;
		return result;
	}

	void PbrPass::doJoinMtlRgh( TextureUnitPtrArray & result )
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

	void PbrPass::doJoinSpcGls( TextureUnitPtrArray & result )
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
