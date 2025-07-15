#include "Castor3D/Material/Pass/Component/Base/BlendComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace c3d
{
	template<>
	class TextWriter< BlendComponent >
		: public TextWriterT< BlendComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< BlendComponent >{ tabs }
		{
		}

		bool operator()( BlendComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, cuT( "colour_blend_mode" ), getName( object.getColourBlendMode() ), getName( BlendMode::eNoBlend ) )
				&& writeOpt( file, cuT( "alpha_blend_mode" ), getName( object.getAlphaBlendMode() ), getName( BlendMode::eNoBlend ) );
		}
	};
}

namespace c3d
{
	//*********************************************************************************************

	namespace bldcmp
	{
		static CU_ImplementAttributeParserBlock( parserPassAlphaBlendMode, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t mode = 0;
				params[0]->get( mode );
				auto & component = getPassComponent< BlendComponent >( *blockContext );
				component.setAlphaBlendMode( BlendMode( mode ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPassColourBlendMode, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t mode = 0;
				params[0]->get( mode );
				auto & component = getPassComponent< BlendComponent >( *blockContext );
				component.setColourBlendMode( BlendMode( mode ) );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void BlendComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "alpha_blend_mode" )
			, bldcmp::parserPassAlphaBlendMode
			, { makeParameter< ParameterType::eCheckedText, BlendMode >() } );
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "colour_blend_mode" )
			, bldcmp::parserPassColourBlendMode
			, { makeParameter< ParameterType::eCheckedText, BlendMode >() } );
	}

	//*********************************************************************************************

	String const BlendComponent::TypeName = C3D_MakePassBaseComponentName( "blend" );

	BlendComponent::BlendComponent( Pass & pass )
		: BaseDataPassComponentT< BlendData >{ pass, TypeName }
	{
	}

	void BlendComponent::accept( ConfigurationVisitorBase & vis )
	{
		static StringArray names{ cuT( "NoBlend" )
			, cuT( "Additive" )
			, cuT( "Multiplicative" )
			, cuT( "Interpolative" ) };
		vis.visit( cuT( "Colour blend mode" )
			, m_value.colourBlendMode
			, names
			, ConfigurationVisitorBase::OnEnumValueChangeT< BlendMode >( [this]( CU_UnusedParam( BlendMode, oldV ), BlendMode newV )
			{
				m_value.colourBlendMode = newV;
			} ) );
		vis.visit( cuT( "Alpha blend mode" )
			, m_value.alphaBlendMode
			, names
			, ConfigurationVisitorBase::OnEnumValueChangeT< BlendMode >( [this]( CU_UnusedParam( BlendMode, oldV ), BlendMode newV )
			{
				m_value.alphaBlendMode = newV;
			} ) );
	}

	PassComponentUPtr BlendComponent::doClone( Pass & pass )const
	{
		auto result = makeRawUnique< BlendComponent >( pass );
		result->setData( getData() );
		return PassComponentUPtr{ result.release() };
	}

	bool BlendComponent::doWriteText( String const & tabs
		, Path const & folder
		, String const & subfolder
		, StringStream & file )const
	{
		return TextWriter< BlendComponent >{ tabs }( *this, file );
	}

	//*********************************************************************************************
}
