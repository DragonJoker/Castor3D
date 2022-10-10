#include "Castor3D/Material/Pass/Component/Base/UntileComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::UntileComponent >
		: public TextWriterT< castor3d::UntileComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::UntileComponent >{ tabs }
		{
		}

		bool operator()( castor3d::UntileComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, cuT( "untile" ), object.isUntiling(), false );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace unt
	{
		static CU_ImplementAttributeParser( parserPassUntile )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value{ false };
				params[0]->get( value );
				auto & component = getPassComponent< UntileComponent >( parsingContext );
				component.setUntiling( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void UntileComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		Pass::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "untile" )
			, unt::parserPassUntile
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
	}

	//*********************************************************************************************

	castor::String const UntileComponent::TypeName = C3D_MakePassComponentName( "untile" );

	UntileComponent::UntileComponent( Pass & pass )
		: BaseDataPassComponentT< castor::AtomicGroupChangeTracked< bool > >{ pass, TypeName }
	{
	}

	void UntileComponent::accept( PassVisitorBase & vis )
	{
		vis.visit( cuT( "Untile" ), m_value );
	}

	PassComponentUPtr UntileComponent::doClone( Pass & pass )const
	{
		auto result = std::make_unique< UntileComponent >( pass );
		result->setData( getData() );
		return result;
	}

	bool UntileComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< UntileComponent >{ tabs }( *this, file );
	}

	//*********************************************************************************************
}
