#include "Castor3D/Material/Pass/Component/Base/BlendComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

CU_ImplementSmartPtr( castor3d, BlendComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::BlendComponent >
		: public TextWriterT< castor3d::BlendComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::BlendComponent >{ tabs }
		{
		}

		bool operator()( castor3d::BlendComponent const & object
			, StringStream & file )override
		{
			static const String blendModes[uint32_t( castor3d::BlendMode::eCount )] =
			{
				cuT( "none" ),
				cuT( "additive" ),
				cuT( "multiplicative" ),
				cuT( "interpolative" ),
				cuT( "a_buffer" ),
				cuT( "depth_peeling" ),
			};
			return writeOpt( file, cuT( "colour_blend_mode" ), blendModes[uint32_t( object.getColourBlendMode() )], blendModes[uint32_t( castor3d::BlendMode::eNoBlend )] )
				&& writeOpt( file, cuT( "alpha_blend_mode" ), blendModes[uint32_t( object.getAlphaBlendMode() )], blendModes[uint32_t( castor3d::BlendMode::eNoBlend )] );
		}
	};
}

namespace castor3d
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

	void BlendComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "alpha_blend_mode" )
			, bldcmp::parserPassAlphaBlendMode
			, { castor::makeParameter< castor::ParameterType::eCheckedText, BlendMode >() } );
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "colour_blend_mode" )
			, bldcmp::parserPassColourBlendMode
			, { castor::makeParameter< castor::ParameterType::eCheckedText, BlendMode >() } );
	}

	//*********************************************************************************************

	castor::String const BlendComponent::TypeName = C3D_MakePassBaseComponentName( "blend" );

	BlendComponent::BlendComponent( Pass & pass )
		: BaseDataPassComponentT< BlendData >{ pass, TypeName }
	{
	}

	void BlendComponent::accept( ConfigurationVisitorBase & vis )
	{
		static castor::StringArray names{ cuT( "NoBlend" )
			, cuT( "Additive" )
			, cuT( "Multiplicative" )
			, cuT( "Interpolative" ) };
		vis.visit( cuT( "Colour blend mode" )
			, m_value.colourBlendMode
			, names
			, ConfigurationVisitorBase::OnEnumValueChangeT< BlendMode >( [this]( BlendMode oldV, BlendMode newV )
			{
				m_value.colourBlendMode = newV;
			} ) );
		vis.visit( cuT( "Alpha blend mode" )
			, m_value.alphaBlendMode
			, names
			, ConfigurationVisitorBase::OnEnumValueChangeT< BlendMode >( [this]( BlendMode oldV, BlendMode newV )
			{
				m_value.alphaBlendMode = newV;
			} ) );
	}

	PassComponentUPtr BlendComponent::doClone( Pass & pass )const
	{
		auto result = castor::makeUnique< BlendComponent >( pass );
		result->setData( getData() );
		return castor::ptrRefCast< PassComponent >( result );
	}

	bool BlendComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< BlendComponent >{ tabs }( *this, file );
	}

	//*********************************************************************************************
}
