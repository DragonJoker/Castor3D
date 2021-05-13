#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp"

namespace castor3d
{
	SubmeshComponent::SubmeshComponent( Submesh & submesh
		, castor::String const & type )
		: castor::OwnedBy< Submesh >{ submesh }
		, m_type{ type }
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

	void SubmeshComponent::cleanup()
	{
		if ( m_initialised )
		{
			doCleanup();
		}
	}

	void SubmeshComponent::fill( RenderDevice const & device )
	{
		if ( m_initialised )
		{
			doFill( device );
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
