#include "Castor3D/Material/Pass/Phong/BlinnPhongPass.hpp"

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
	class TextWriter< castor3d::BlinnPhongPass >
		: public TextWriterT< castor3d::BlinnPhongPass >
	{
	public:
		TextWriter( String const & tabs
			, Path const & folder
			, String const & subfolder )
			: TextWriterT< castor3d::BlinnPhongPass >{ tabs }
			, m_folder{ folder }
			, m_subfolder{ subfolder }
		{
		}

		bool operator()( castor3d::BlinnPhongPass const & pass
			, StringStream & file )
		{
			castor3d::log::info << tabs() << cuT( "Writing BlinnPhongPass " ) << std::endl;
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

	namespace blinn_phong
	{
		enum class Section
			: uint32_t
		{
			ePass = CU_MakeSectionName( 'B', 'L', 'P', 'N' ),
			eTextureUnit = CU_MakeSectionName( 'B', 'L', 'P', 'T' ),
		};

		static castor::String const PassSectionName{ cuT( "blinn_phong_pass" ) };
		static castor::String const TextureSectionName{ cuT( "blinn_phong_texture_unit" ) };

		CU_ImplementAttributeParser( parserPassDiffuse )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & phongPass = static_cast< BlinnPhongPass & >( *parsingContext->pass );
				castor::RgbColour value;
				params[0]->get( value );
				phongPass.setDiffuse( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassSpecular )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & phongPass = static_cast< BlinnPhongPass & >( *parsingContext->pass );
				castor::RgbColour value;
				params[0]->get( value );
				phongPass.setSpecular( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassAmbient )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & phongPass = static_cast< BlinnPhongPass & >( *parsingContext->pass );
				float value;
				params[0]->get( value );
				phongPass.setAmbient( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassShininess )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & phongPass = static_cast< BlinnPhongPass & >( *parsingContext->pass );
				float value;
				params[0]->get( value );
				phongPass.setShininess( value );
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

				if ( checkFlag( textures, TextureFlag::eDiffuse ) )
				{
					parsingContext->textureConfiguration.colourMask[0] = 0x00FFFFFF;
				}

				if ( checkFlag( textures, TextureFlag::eSpecular ) )
				{
					parsingContext->textureConfiguration.specularMask[0] = 0x00FFFFFF;
				}

				if ( checkFlag( textures, TextureFlag::eShininess ) )
				{
					parsingContext->textureConfiguration.glossinessMask[0] = 0x00FF0000;
				}
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitDiffuseMask )
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

		CU_ImplementAttributeParser( parserUnitSpecularMask )
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
				params[0]->get( parsingContext->textureConfiguration.specularMask[0] );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserUnitShininessMask )
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
				params[0]->get( parsingContext->textureConfiguration.glossinessMask[0] );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	castor::String const BlinnPhongPass::Type = cuT( "blinn_phong" );
	castor::String const BlinnPhongPass::LightingModel = shader::BlinnPhongLightingModel::getName();

	BlinnPhongPass::BlinnPhongPass( Material & parent
		, PassFlags initialFlags )
		: BlinnPhongPass{ parent, parent.getEngine()->getPassFactory().getNameId( Type ), initialFlags }
	{
	}

	BlinnPhongPass::BlinnPhongPass( Material & parent
		, PassTypeID typeID
		, PassFlags initialFlags )
		: PhongPass{ parent, typeID, initialFlags }
	{
	}

	BlinnPhongPass::~BlinnPhongPass()
	{
	}

	PassSPtr BlinnPhongPass::create( Material & parent )
	{
		return std::make_shared< BlinnPhongPass >( parent );
	}

	castor::AttributeParsersBySection BlinnPhongPass::createParsers()
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
		}( );

		castor::AttributeParsersBySection result;

		Pass::addParser( result, uint32_t( blinn_phong::Section::ePass ), cuT( "diffuse" ), blinn_phong::parserPassDiffuse, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
		Pass::addParser( result, uint32_t( blinn_phong::Section::ePass ), cuT( "specular" ), blinn_phong::parserPassSpecular, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
		Pass::addParser( result, uint32_t( blinn_phong::Section::ePass ), cuT( "ambient" ), blinn_phong::parserPassAmbient, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addParser( result, uint32_t( blinn_phong::Section::ePass ), cuT( "shininess" ), blinn_phong::parserPassShininess, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addParser( result, uint32_t( blinn_phong::Section::eTextureUnit ), cuT( "diffuse_mask" ), blinn_phong::parserUnitDiffuseMask, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParser( result, uint32_t( blinn_phong::Section::eTextureUnit ), cuT( "specular_mask" ), blinn_phong::parserUnitSpecularMask, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParser( result, uint32_t( blinn_phong::Section::eTextureUnit ), cuT( "shininess_mask" ), blinn_phong::parserUnitShininessMask, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParser( result, uint32_t( blinn_phong::Section::eTextureUnit ), cuT( "channel" ), blinn_phong::parserUnitChannel, { castor::makeParameter< castor::ParameterType::eBitwiseOred32BitsCheckedText >( textureChannels ) } );
		Pass::addCommonParsers( uint32_t( blinn_phong::Section::ePass )
			, uint32_t( blinn_phong::Section::eTextureUnit )
			, result );

		return result;
	}

	castor::StrUInt32Map BlinnPhongPass::createSections()
	{
		return
		{
			{ uint32_t( blinn_phong::Section::ePass ), blinn_phong::PassSectionName },
			{ uint32_t( blinn_phong::Section::eTextureUnit ), blinn_phong::TextureSectionName },
		};
	}

	uint32_t BlinnPhongPass::getPassSectionID()const
	{
		return uint32_t( blinn_phong::Section::ePass );
	}

	uint32_t BlinnPhongPass::getTextureSectionID()const
	{
		return uint32_t( blinn_phong::Section::eTextureUnit );
	}

	//*********************************************************************************************
}
