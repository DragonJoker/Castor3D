#include "Castor3D/Material/Pass/PBR/SpecularGlossinessPbrPass.hpp"

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
	class TextWriter< castor3d::SpecularGlossinessPbrPass >
		: public TextWriterT< castor3d::SpecularGlossinessPbrPass >
	{
	public:
		TextWriter( String const & tabs
			, Path const & folder
			, String const & subfolder )
			: TextWriterT< castor3d::SpecularGlossinessPbrPass >{ tabs }
			, m_folder{ folder }
			, m_subfolder{ subfolder }
		{
		}

		bool operator()( castor3d::SpecularGlossinessPbrPass const & pass
			, StringStream & file )override
		{
			castor3d::log::info << tabs() << cuT( "Writing SpecularGlossinessPbrPass " ) << std::endl;
			return writeNamedSub( file, "albedo", pass.getDiffuse() )
				&& writeNamedSub( file, "specular", pass.getSpecular() )
				&& write( file, "glossiness", pass.getGlossiness() );
		}

	private:
		Path m_folder;
		String m_subfolder;
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace pbrsg
	{
		enum class Section
			: uint32_t
		{
			ePass = CU_MakeSectionName( 'S', 'P', 'G', 'L' ),
			eTextureUnit = CU_MakeSectionName( 'S', 'G', 'T', 'U' ),
		};

		static castor::String const PassSectionName{ cuT( "pbrsg_pass" ) };
		static castor::String const TextureSectionName{ cuT( "pbrsg_texture_unit" ) };

		CU_ImplementAttributeParser( parserPassDiffuse )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & phongPass = static_cast< SpecularGlossinessPbrPass & >( *parsingContext->pass );
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
				auto & phongPass = static_cast< SpecularGlossinessPbrPass & >( *parsingContext->pass );
				castor::RgbColour value;
				params[0]->get( value );
				phongPass.setSpecular( value );
			}
		}
		CU_EndAttribute()

		CU_ImplementAttributeParser( parserPassGlossiness )
		{
			auto parsingContext = std::static_pointer_cast< castor3d::SceneFileContext >( context );

			if ( !parsingContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				auto & phongPass = static_cast< SpecularGlossinessPbrPass & >( *parsingContext->pass );
				float value;
				params[0]->get( value );
				phongPass.setGlossiness( value );
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

				if ( checkFlag( textures, TextureFlag::eSpecular ) )
				{
					parsingContext->textureConfiguration.specularMask[0] = 0x00FFFFFF;
				}

				if ( checkFlag( textures, TextureFlag::eGlossiness ) )
				{
					parsingContext->textureConfiguration.glossinessMask[0] = 0x00FF0000;
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

		CU_ImplementAttributeParser( parserUnitGlossinessMask )
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

	castor::String const SpecularGlossinessPbrPass::Type = cuT( "specular_glossiness" );
	castor::String const SpecularGlossinessPbrPass::LightingModel = shader::PbrSGLightingModel::getName();

	SpecularGlossinessPbrPass::SpecularGlossinessPbrPass( Material & parent
		, PassFlags initialFlags )
		: SpecularGlossinessPbrPass{ parent, parent.getEngine()->getPassFactory().getNameId( Type ), initialFlags }
	{
	}

	SpecularGlossinessPbrPass::SpecularGlossinessPbrPass( Material & parent
		, PassTypeID typeID
		, PassFlags initialFlags )
		: Pass{ parent, typeID, initialFlags | PassFlag::eImageBasedLighting }
		, m_diffuse{ m_dirty, castor::RgbColour::fromRGBA( 0xFFFFFFFF ) }
		, m_specular{ m_dirty, castor::RgbColour::fromRGBA( 0xFFFFFFFF ) }
		, m_glossiness{ m_dirty, 0.0f }
	{
	}

	SpecularGlossinessPbrPass::~SpecularGlossinessPbrPass()
	{
	}

	PassSPtr SpecularGlossinessPbrPass::create( Material & parent )
	{
		return std::make_shared< SpecularGlossinessPbrPass >( parent );
	}

	castor::AttributeParsersBySection SpecularGlossinessPbrPass::createParsers()
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
			result[getName( TextureFlag::eAlbedo, false )] = uint32_t( TextureFlag::eAlbedo );
			result[getName( TextureFlag::eAlbedo, true )] = uint32_t( TextureFlag::eAlbedo );
			result[getName( TextureFlag::eSpecular, true )] = uint32_t( TextureFlag::eSpecular );
			result[getName( TextureFlag::eGlossiness, true )] = uint32_t( TextureFlag::eGlossiness );
			return result;
		}();

		castor::AttributeParsersBySection result;

		Pass::addParser( result, uint32_t( pbrsg::Section::ePass ), cuT( "diffuse" ), pbrsg::parserPassDiffuse, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
		Pass::addParser( result, uint32_t( pbrsg::Section::ePass ), cuT( "albedo" ), pbrsg::parserPassDiffuse, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
		Pass::addParser( result, uint32_t( pbrsg::Section::ePass ), cuT( "specular" ), pbrsg::parserPassSpecular, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
		Pass::addParser( result, uint32_t( pbrsg::Section::ePass ), cuT( "glossiness" ), pbrsg::parserPassGlossiness, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addParser( result, uint32_t( pbrsg::Section::eTextureUnit ), cuT( "albedo_mask" ), pbrsg::parserUnitAlbedoMask, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParser( result, uint32_t( pbrsg::Section::eTextureUnit ), cuT( "specular_mask" ), pbrsg::parserUnitSpecularMask, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParser( result, uint32_t( pbrsg::Section::eTextureUnit ), cuT( "glossiness_mask" ), pbrsg::parserUnitGlossinessMask, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		Pass::addParser( result, uint32_t( pbrsg::Section::eTextureUnit ), cuT( "channel" ), pbrsg::parserUnitChannel, { castor::makeParameter< castor::ParameterType::eBitwiseOred32BitsCheckedText >( textureChannels ) } );
		Pass::addCommonParsers( uint32_t( pbrsg::Section::ePass )
			, uint32_t( pbrsg::Section::eTextureUnit )
			, result );

		return result;
	}

	castor::StrUInt32Map SpecularGlossinessPbrPass::createSections()
	{
		return
		{
			{ uint32_t( pbrsg::Section::ePass ), pbrsg::PassSectionName },
			{ uint32_t( pbrsg::Section::eTextureUnit ), pbrsg::TextureSectionName },
		};
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

	uint32_t SpecularGlossinessPbrPass::getPassSectionID()const
	{
		return uint32_t( pbrsg::Section::ePass );
	}

	uint32_t SpecularGlossinessPbrPass::getTextureSectionID()const
	{
		return uint32_t( pbrsg::Section::eTextureUnit );
	}

	bool SpecularGlossinessPbrPass::writeText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< SpecularGlossinessPbrPass >{ tabs, folder, subfolder }( *this, file );
	}

	void SpecularGlossinessPbrPass::doInitialise()
	{
	}

	void SpecularGlossinessPbrPass::doCleanup()
	{
	}

	void SpecularGlossinessPbrPass::doAccept( PassVisitorBase & vis )
	{
		vis.visit( cuT( "Diffuse" )
			, m_diffuse );
		vis.visit( cuT( "Specular" )
			, m_specular );
		vis.visit( cuT( "Glossiness" )
			, m_glossiness );
	}

	void SpecularGlossinessPbrPass::doAccept( castor3d::TextureConfiguration & configuration
		, castor3d::PassVisitorBase & vis )
	{
		vis.visit( cuT( "Albedo" ), castor3d::TextureFlag::eAlbedo, configuration.colourMask, 3u );
		vis.visit( cuT( "Specular" ), castor3d::TextureFlag::eSpecular, configuration.specularMask, 3u );
		vis.visit( cuT( "Glossiness" ), castor3d::TextureFlag::eGlossiness, configuration.glossinessMask, 1u );
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
