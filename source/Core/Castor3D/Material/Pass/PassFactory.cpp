#include "Castor3D/Material/Pass/PassFactory.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

CU_ImplementCUSmartPtr( castor3d, PassFactory )

namespace castor3d
{
	PassFactory::PassFactory( Engine & engine )
		: castor::OwnedBy< Engine >{ engine }
		, PassFactoryBase{}
	{
	}

	PassFactory::~PassFactory()
	{
	}

	void PassFactory::registerType( LightingModelID lightingModelId
		, PassRegisterInfo info )
	{
		if ( !isTypeRegistered( lightingModelId ) )
		{
			auto & entry = m_registered.emplace_back();
			entry.key = lightingModelId;
			entry.create = info.passCreator;
			entry.id = ++m_currentId;
			entry.name = info.lightingModel;
			entry.hasIBLSupport = info.hasIBLSupport;
		}
	}

	PassSPtr PassFactory::create( LightingModelID lightingModelId
		, Material & parent )const
	{
		return create( lightingModelId, lightingModelId, parent );
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

	bool PassFactory::hasIBLSupport( LightingModelID lightingModelId )const
	{
		return m_registered.end() != std::find_if( m_registered.begin()
			, m_registered.end()
			, [lightingModelId]( Entry const & lookup )
			{
				return lookup.key == lightingModelId
					&& lookup.hasIBLSupport;
			} );
	}
}
