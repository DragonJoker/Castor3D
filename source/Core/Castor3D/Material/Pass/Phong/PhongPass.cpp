#include "Castor3D/Material/Pass/Phong/PhongPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Phong/Shaders/GlslPhongLighting.hpp"
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
	class TextWriter< castor3d::PhongPass >
		: public TextWriterT< castor3d::PhongPass >
	{
	public:
		TextWriter( String const & tabs
			, Path const & folder
			, String const & subfolder )
			: TextWriterT< castor3d::PhongPass >{ tabs }
			, m_folder{ folder }
			, m_subfolder{ subfolder }
		{
		}

		bool operator()( castor3d::PhongPass const & pass
			, StringStream & file )override
		{
			castor3d::log::info << tabs() << cuT( "Writing PhongPass " ) << std::endl;
			return writeNamedSub( file, "diffuse", pass.getDiffuse() )
				&& writeNamedSub( file, "specular", pass.getSpecular() )
				&& write( file, "ambient", pass.getAmbient() )
				&& write( file, "shininess", pass.getShininess() );
		}

	private:
		Path m_folder;
		String m_subfolder;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace phong
	{
		namespace
		{
			enum class Section
				: uint32_t
			{
				ePass = CU_MakeSectionName( 'P', 'H', 'O', 'N' ),
				eTextureUnit = CU_MakeSectionName( 'P', 'H', 'T', 'U' ),
			};

			static castor::String const PassSectionName{ cuT( "phong_pass" ) };
			static castor::String const TextureSectionName{ cuT( "phong_texture_unit" ) };

			CU_ImplementAttributeParser( parserPassDiffuse )
			{
				auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

				if ( !parsingContext.pass )
				{
					CU_ParsingError( cuT( "No Pass initialised." ) );
				}
				else if ( !params.empty() )
				{
					auto & phongPass = static_cast< PhongPass & >( *parsingContext.pass );
					castor::RgbColour value;
					params[0]->get( value );
					phongPass.setDiffuse( value );
				}
			}
			CU_EndAttribute()

			CU_ImplementAttributeParser( parserPassSpecular )
			{
				auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

				if ( !parsingContext.pass )
				{
					CU_ParsingError( cuT( "No Pass initialised." ) );
				}
				else if ( !params.empty() )
				{
					auto & phongPass = static_cast< PhongPass & >( *parsingContext.pass );
					castor::RgbColour value;
					params[0]->get( value );
					phongPass.setSpecular( value );
				}
			}
			CU_EndAttribute()

			CU_ImplementAttributeParser( parserPassAmbient )
			{
				auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

				if ( !parsingContext.pass )
				{
					CU_ParsingError( cuT( "No Pass initialised." ) );
				}
				else if ( !params.empty() )
				{
					auto & phongPass = static_cast< PhongPass & >( *parsingContext.pass );
					float value;
					params[0]->get( value );
					phongPass.setAmbient( value );
				}
			}
			CU_EndAttribute()

			CU_ImplementAttributeParser( parserPassShininess )
			{
				auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

				if ( !parsingContext.pass )
				{
					CU_ParsingError( cuT( "No Pass initialised." ) );
				}
				else if ( !params.empty() )
				{
					auto & phongPass = static_cast< PhongPass & >( *parsingContext.pass );
					float value;
					params[0]->get( value );
					phongPass.setShininess( value );
				}
			}
			CU_EndAttribute()

			CU_ImplementAttributeParser( parserUnitChannel )
			{
				auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

				if ( !parsingContext.textureUnit )
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

					if ( checkFlag( textures, TextureFlag::eDiffuse ) )
					{
						parsingContext.textureConfiguration.colourMask[0] = 0x00FFFFFF;
					}

					if ( checkFlag( textures, TextureFlag::eSpecular ) )
					{
						parsingContext.textureConfiguration.specularMask[0] = 0x00FFFFFF;
					}

					if ( checkFlag( textures, TextureFlag::eShininess ) )
					{
						parsingContext.textureConfiguration.glossinessMask[0] = 0x00FF0000;
					}
				}
			}
			CU_EndAttribute()

			CU_ImplementAttributeParser( parserUnitDiffuseMask )
			{
				auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

				if ( !parsingContext.textureUnit )
				{
					CU_ParsingError( cuT( "No TextureUnit initialised." ) );
				}
				else if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( parsingContext.textureConfiguration.colourMask[0] );
				}
			}
			CU_EndAttribute()

			CU_ImplementAttributeParser( parserUnitSpecularMask )
			{
				auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

				if ( !parsingContext.textureUnit )
				{
					CU_ParsingError( cuT( "No TextureUnit initialised." ) );
				}
				else if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( parsingContext.textureConfiguration.specularMask[0] );
				}
			}
			CU_EndAttribute()

			CU_ImplementAttributeParser( parserUnitShininessMask )
			{
				auto & parsingContext = static_cast< castor3d::SceneFileContext & >( context );

				if ( !parsingContext.textureUnit )
				{
					CU_ParsingError( cuT( "No TextureUnit initialised." ) );
				}
				else if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( parsingContext.textureConfiguration.glossinessMask[0] );
				}
			}
			CU_EndAttribute()
		}
	}

	//*********************************************************************************************

	castor::String const PhongPass::Type = cuT( "phong" );
	castor::String const PhongPass::LightingModel = shader::PhongLightingModel::getName();

	PhongPass::PhongPass( Material & parent
		, PassFlags initialFlags )
		: PhongPass{ parent, parent.getEngine()->getPassFactory().getNameId( Type ), initialFlags }
	{
	}

	PhongPass::PhongPass( Material & parent
		, PassTypeID typeID
		, PassFlags initialFlags )
		: Pass{ parent, typeID, initialFlags }
		, m_diffuse{ m_dirty, castor::RgbColour::fromRGBA( 0xFFFFFFFF ) }
		, m_specular{ m_dirty, castor::RgbColour::fromRGBA( 0xFFFFFFFF ) }
		, m_ambient{ m_dirty, 1.0f }
		, m_shininess{ m_dirty, { 50.0f, castor::makeRange( 0.0001f, MaxShininess ) } }
	{
	}

	PassSPtr PhongPass::create( Material & parent )
	{
		return std::make_shared< PhongPass >( parent );
	}

	castor::AttributeParsers PhongPass::createParsers()
	{
		return createParsers( uint32_t( phong::Section::ePass )
			, uint32_t( phong::Section::eTextureUnit ) );
	}

	castor::AttributeParsers PhongPass::createParsers( uint32_t mtlSectionID
		, uint32_t texSectionID )
	{
		static UInt32StrMap const textureChannels = []()
		{
			UInt32StrMap result;
			result[getName( TextureFlag::eNormal, false )] = uint32_t( TextureFlag::eNormal );
			result[getName( TextureFlag::eHeight, false )] = uint32_t( TextureFlag::eHeight );
			result[getName( TextureFlag::eOpacity, false )] = uint32_t( TextureFlag::eOpacity );
			result[getName( TextureFlag::eEmissive, false )] = uint32_t( TextureFlag::eEmissive );
			result[getName( TextureFlag::eOcclusion, false )] = uint32_t( TextureFlag::eOcclusion );
			result[getName( TextureFlag::eOcclusion, false )] = uint32_t( TextureFlag::eOcclusion );
			result[getName( TextureFlag::eTransmittance, false )] = uint32_t( TextureFlag::eTransmittance );
			result[getName( TextureFlag::eDiffuse, false )] = uint32_t( TextureFlag::eDiffuse );
			result[getName( TextureFlag::eSpecular, false )] = uint32_t( TextureFlag::eSpecular );
			result[getName( TextureFlag::eShininess, false )] = uint32_t( TextureFlag::eShininess );
			return result;
		}();

		castor::AttributeParsers result;

		Pass::addParser( result, mtlSectionID, cuT( "diffuse" ), phong::parserPassDiffuse, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
		Pass::addParser( result, mtlSectionID, cuT( "specular" ), phong::parserPassSpecular, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
		Pass::addParser( result, mtlSectionID, cuT( "ambient" ), phong::parserPassAmbient, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addParser( result, mtlSectionID, cuT( "shininess" ), phong::parserPassShininess, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addParser( result, texSectionID, cuT( "diffuse_mask" ), phong::parserUnitDiffuseMask, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParser( result, texSectionID, cuT( "specular_mask" ), phong::parserUnitSpecularMask, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParser( result, texSectionID, cuT( "shininess_mask" ), phong::parserUnitShininessMask, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParser( result, texSectionID, cuT( "channel" ), phong::parserUnitChannel, { castor::makeParameter< castor::ParameterType::eBitwiseOred32BitsCheckedText >( textureChannels ) } );
		Pass::addCommonParsers( mtlSectionID, texSectionID, result );

		return result;
	}

	castor::StrUInt32Map PhongPass::createSections()
	{
		return
		{
			{ uint32_t( phong::Section::ePass ), phong::PassSectionName },
			{ uint32_t( phong::Section::eTextureUnit ), phong::TextureSectionName },
		};
	}

	void PhongPass::fillBuffer( PassBuffer & buffer )const
	{
		auto data = buffer.getData( getId() );

		data.colourDiv->r = getDiffuse().red();
		data.colourDiv->g = getDiffuse().green();
		data.colourDiv->b = getDiffuse().blue();
		data.colourDiv->a = getAmbient();
		data.specDiv->r = getSpecular().red();
		data.specDiv->g = getSpecular().green();
		data.specDiv->b = getSpecular().blue();
		data.specDiv->a = getShininess().value();

		doFillData( data );
	}

	uint32_t PhongPass::getPassSectionID()const
	{
		return uint32_t( phong::Section::ePass );
	}

	uint32_t PhongPass::getTextureSectionID()const
	{
		return uint32_t( phong::Section::eTextureUnit );
	}

	bool PhongPass::writeText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< PhongPass >{ tabs, folder, subfolder }( *this, file );
	}

	void PhongPass::doAccept( PassVisitorBase & vis )
	{
		vis.visit( cuT( "Ambient" )
			, m_ambient );
		vis.visit( cuT( "Diffuse" )
			, m_diffuse );
		vis.visit( cuT( "Specular" )
			, m_specular );
		vis.visit( cuT( "Exponent" )
			, m_shininess );
	}

	void PhongPass::doAccept( TextureConfiguration & configuration
		, PassVisitorBase & vis )
	{
		vis.visit( cuT( "Diffuse" ), TextureFlag::eDiffuse, configuration.colourMask, 3u );
		vis.visit( cuT( "Specular" ), TextureFlag::eSpecular, configuration.specularMask, 3u );
		vis.visit( cuT( "Shininess" ), TextureFlag::eShininess, configuration.glossinessMask, 1u );
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

	//*********************************************************************************************
}
