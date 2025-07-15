#include "Castor3D/Material/Pass/Component/Base/PickableComponent.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace c3d
{
	template<>
	class TextWriter< PickableComponent >
		: public TextWriterT< PickableComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< PickableComponent >{ tabs }
		{
		}

		bool operator()( PickableComponent const & object
			, StringStream & file )override
		{
			return writeOpt( file, cuT( "pickable" ), object.isPickable(), true );
		}
	};

	//*********************************************************************************************

	namespace tws
	{
		static CU_ImplementAttributeParserBlock( parserPassPickable, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else if ( !params.empty() )
			{
				bool value;
				params[0]->get( value );
				auto & component = getPassComponent< PickableComponent >( *blockContext );
				component.setPickable( value );
			}
		}
		CU_EndAttribute()
	}

	//*********************************************************************************************

	void PickableComponent::Plugin::createParsers( AttributeParsers & parsers
		, ChannelFillers & channelFillers )const
	{
		c3d::addParserT( parsers
			, CSCNSection::ePass
			, cuT( "pickable" )
			, tws::parserPassPickable
			, { makeParameter< ParameterType::eBool >() } );
	}

	//*********************************************************************************************

	String const PickableComponent::TypeName = C3D_MakePassBaseComponentName( "pickable" );

	PickableComponent::PickableComponent( Pass & pass
		, bool pickable )
		: BaseDataPassComponentT< AtomicGroupChangeTracked< bool > >{ pass, TypeName, {}, pickable }
	{
	}

	void PickableComponent::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Pickable" ), m_value );
	}

	PassComponentUPtr PickableComponent::doClone( Pass & pass )const
	{
		auto result = makeRawUnique< PickableComponent >( pass );
		result->setData( getData() );
		return PassComponentUPtr{ result.release() };
	}

	bool PickableComponent::doWriteText( String const & tabs
		, Path const & folder
		, String const & subfolder
		, StringStream & file )const
	{
		return TextWriter< PickableComponent >{ tabs }( *this, file );
	}

	//*********************************************************************************************
}
