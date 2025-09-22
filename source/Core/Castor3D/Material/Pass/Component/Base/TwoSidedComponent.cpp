#include "Castor3D/Material/Pass/Component/Base/TwoSidedComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace c3d
{
	template<>
	class TextWriter< TwoSidedComponent >
		: public TextWriterT< TwoSidedComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< TwoSidedComponent >{ tabs }
		{
		}

		bool operator()( TwoSidedComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, cuT( "two_sided" ), object.isTwoSided(), false );
		}
	};

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

	void TwoSidedComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "two_sided" )
			, tws::parserPassTwoSided
			, { makeParameter< ParameterType::eBool >() } );
	}

	//*********************************************************************************************

	String const TwoSidedComponent::TypeName = C3D_MakePassBaseComponentName( "two_sided" );

	TwoSidedComponent::TwoSidedComponent( Pass & pass)
		: BaseDataPassComponentT< AtomicGroupChangeTracked< bool > >{ pass, TypeName }
	{
	}

	void TwoSidedComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Two sided" ), m_value );
	}

	PassComponentUPtr TwoSidedComponent::doClone( Pass & pass )const
	{
		auto result = makeRawUnique< TwoSidedComponent >( pass );
		result->setData( getData() );
		return passComponentCast( result );
	}

	bool TwoSidedComponent::doWriteText( String const & tabs
		, Path const & folder
		, String const & subfolder
		, StringStream & file )const
	{
		return TextWriter< TwoSidedComponent >{ tabs }( *this, file );
	}

	//*********************************************************************************************
}
