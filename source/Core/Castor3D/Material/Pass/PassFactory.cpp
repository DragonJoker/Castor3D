#include "Castor3D/Material/Pass/PassFactory.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

CU_ImplementSmartPtr( castor3d, PassFactory )

namespace castor3d
{
	PassFactory::PassFactory( Engine & engine )
		: castor::OwnedBy< Engine >{ engine }
		, PassFactoryBase{}
	{
	}

	void PassFactory::registerType( LightingModelID lightingModelId
		, PassRegisterInfo const & info )
	{
		if ( !isTypeRegistered( lightingModelId ) )
		{
			auto & entry = m_registered.emplace_back();
			entry.key = lightingModelId;
			entry.create = info.passCreator;
			entry.id = ++m_currentId;
			entry.name = info.lightingModel;
		}
	}

	PassUPtr PassFactory::create( Material & parent
		, LightingModelID lightingModelId )const
	{
		return create( lightingModelId, lightingModelId, parent );
	}

	PassUPtr PassFactory::create( Material & parent
		, Pass const & rhs )const
	{
		return castor::makeUnique< Pass >( parent, rhs );
	}

	LightingModelID PassFactory::getNameId( castor::String const & passType )const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&passType]( Entry const & lookup )
			{
				return lookup.name == passType;
			} );

		if ( it == m_registered.end() )
		{
			CU_Exception( "Unknown pass type." );
		}

		return it->key;
	}

	castor::String PassFactory::getIdName( LightingModelID lightingModelId )const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [lightingModelId]( Entry const & lookup )
			{
				return lookup.key == lightingModelId;
			} );

		if ( it == m_registered.end() )
		{
			CU_Exception( "Unknown pass type ID." );
		}

		return it->name;
	}
}
