#include "Castor3D/Material/Pass/PassFactory.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Shader/LightingModelFactory.hpp"

CU_ImplementSmartPtr( c3d, PassFactory )

namespace c3d
{
	PassFactory::PassFactory( Engine & engine )
		: OwnedBy< Engine >{ engine }
		, PassFactoryBase{}
	{
	}

	void PassFactory::registerType( PassRegisterInfo const & info )
	{
		if ( !isTypeRegistered( info.lightingModel ) )
		{
			auto & entry = m_registered.emplace_back();
			entry.key = info.lightingModel;
			entry.create = info.passCreator;
			entry.id = ++m_currentId;
		}
	}

	PassUPtr PassFactory::create( Material & parent
		, LightingModelID lightingModelId )const
	{
		return create( getEngine()->getLightingModelFactory().getBaseName( lightingModelId )
			, lightingModelId
			, parent );
	}

	PassUPtr PassFactory::create( Material & parent
		, Pass const & rhs )const
	{
		return makeUnique< Pass >( parent, rhs );
	}
}
