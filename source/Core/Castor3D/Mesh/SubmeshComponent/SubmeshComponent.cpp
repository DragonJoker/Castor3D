#include "Castor3D/Mesh/SubmeshComponent/SubmeshComponent.hpp"

namespace castor3d
{
	SubmeshComponent::SubmeshComponent( Submesh & submesh
		, castor::String const & type )
		: castor::OwnedBy< Submesh >{ submesh }
		, m_type{ type }
	{
	}

	bool SubmeshComponent::initialise()
	{
		if ( !m_initialised || m_dirty )
		{
			m_initialised = doInitialise();
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

	void SubmeshComponent::fill()
	{
		if ( m_initialised )
		{
			doFill();
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

	void SubmeshComponent::setMaterial( MaterialSPtr oldMaterial
		, MaterialSPtr newMaterial
		, bool update )
	{
	}
}
