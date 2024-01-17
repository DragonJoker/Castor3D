#include "Castor3D/Material/Pass/Component/Base/TwoSidedComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( castor3d, TwoSidedComponent )

namespace castor
{
	template<>
	class TextWriter< castor3d::TwoSidedComponent >
		: public TextWriterT< castor3d::TwoSidedComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< castor3d::TwoSidedComponent >{ tabs }
		{
		}

		bool operator()( castor3d::TwoSidedComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, cuT( "two_sided" ), object.isTwoSided(), false );
		}
	};
}

namespace castor3d
{
	//*********************************************************************************************

	namespace tws
	{
		static CU_ImplementAttributeParserBlock( parserPassTwoSided, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				bool value;
				params[0]->get( value );
				auto & component = getPassComponent< TwoSidedComponent >( *blockContext );
				component.setTwoSided( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void TwoSidedComponent::Plugin::createParsers( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		castor::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "two_sided" )
			, tws::parserPassTwoSided
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
	}

	//*********************************************************************************************

	castor::String const TwoSidedComponent::TypeName = C3D_MakePassBaseComponentName( "two_sided" );

	TwoSidedComponent::TwoSidedComponent( Pass & pass)
		: BaseDataPassComponentT< castor::AtomicGroupChangeTracked< bool > >{ pass, TypeName }
	{
	}

	void TwoSidedComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Two sided" ), m_value );
	}

	PassComponentUPtr TwoSidedComponent::doClone( Pass & pass )const
	{
		auto result = castor::makeUnique< TwoSidedComponent >( pass );
		result->setData( getData() );
		return castor::ptrRefCast< PassComponent >( result );
	}

	bool TwoSidedComponent::doWriteText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		return castor::TextWriter< TwoSidedComponent >{ tabs }( *this, file );
	}

	//*********************************************************************************************
}
