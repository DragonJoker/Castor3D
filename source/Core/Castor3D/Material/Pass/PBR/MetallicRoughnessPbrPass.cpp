#include "Castor3D/Material/Pass/PBR/MetallicRoughnessPbrPass.hpp"

#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
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
			eMetallicRoughnessPass = CU_MakeSectionName( 'M', 'T', 'R', 'G' ),
		};

		static castor::String const Name{ cuT( "pbrmr_pass" ) };

		CU_ImplementAttributeParser( parserPassAlbdeo )
		{
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

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

		CU_ImplementAttributeParser( parserPassMetallic )
		{
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

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
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

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
	}

	//*********************************************************************************************

	castor::String const MetallicRoughnessPbrPass::Type = cuT( "metallic_roughness" );

	MetallicRoughnessPbrPass::MetallicRoughnessPbrPass( Material & parent )
		: Pass{ parent }
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
		castor::AttributeParsersBySection result;

		Pass::addParser( result, uint32_t( pbrmr::Section::eMetallicRoughnessPass ), cuT( "albedo" ), &pbrmr::parserPassAlbdeo, { castor::makeParameter< castor::ParameterType::eRgbColour >() } );
		Pass::addParser( result, uint32_t( pbrmr::Section::eMetallicRoughnessPass ), cuT( "metallic" ), &pbrmr::parserPassMetallic, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addParser( result, uint32_t( pbrmr::Section::eMetallicRoughnessPass ), cuT( "roughness" ), &pbrmr::parserPassRoughness, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		Pass::addCommonParsers( uint32_t( pbrmr::Section::eMetallicRoughnessPass ), result );

		return result;
	}

	castor::StrUInt32Map MetallicRoughnessPbrPass::createSections()
	{
		return
		{
			{ uint32_t( pbrmr::Section::eMetallicRoughnessPass ), pbrmr::Name },
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

	uint32_t MetallicRoughnessPbrPass::getSectionID()const
	{
		return uint32_t( pbrmr::Section::eMetallicRoughnessPass );
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

	void MetallicRoughnessPbrPass::doAccept( PipelineVisitorBase & vis )
	{
		vis.visit( cuT( "Albedo" )
			, m_albedo );
		vis.visit( cuT( "Metalness" )
			, m_metalness );
		vis.visit( cuT( "Roughness" )
			, m_roughness );
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
