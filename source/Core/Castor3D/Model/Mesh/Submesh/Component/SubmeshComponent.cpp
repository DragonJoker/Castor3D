#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp"

namespace castor3d
{
	SubmeshComponent::SubmeshComponent( Submesh & submesh
		, castor::String const & type
		, uint32_t id )
		: castor::OwnedBy< Submesh >{ submesh }
		, m_type{ type }
		, m_id{ id }
	{
	}

	bool SubmeshComponent::initialise( RenderDevice const & device )
	{
		if ( !m_initialised || m_dirty )
		{
			m_initialised = doInitialise( device );
		}

		return m_initialised;
	}

	void SubmeshComponent::cleanup( RenderDevice const & device )
	{
		if ( m_initialised )
		{
			doCleanup( device );
		}
	}

	void SubmeshComponent::upload()
	{
		if ( m_dirty )
		{
			doUpload();
			m_dirty = false;
		}
	}
}
