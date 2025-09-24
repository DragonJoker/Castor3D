#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponentRegister.hpp"

CU_ImplementSmartPtr( c3d, SubmeshComponent )
CU_ImplementSmartPtr( c3d, SubmeshComponentPlugin )

namespace c3d
{
	//*********************************************************************************************

	bool hasAny( SubmeshComponentCombine const & lhs
		, SubmeshComponentFlag rhs )noexcept
	{
		auto it = lhs.flags.find( rhs );
		return it != lhs.flags.end();
	}

	bool hasAny( SubmeshComponentCombine const & lhs
		, Vector< SubmeshComponentFlag > const & rhs )noexcept
	{
		return std::any_of( rhs.begin()
			, rhs.end()
			, [&lhs]( SubmeshComponentFlag const & lookup )
			{
				return hasAny( lhs, lookup );
			} );
	}

	void remFlags( SubmeshComponentCombine & lhs
		, SubmeshComponentFlag rhs )noexcept
	{
		auto it = lhs.flags.find( rhs );

		if ( it != lhs.flags.end() )
		{
			lhs.flags.erase( it );
		}
	}

	void remFlags( SubmeshComponentCombine & lhs
		, SubmeshComponentFlagsSet const & rhs )noexcept
	{
		for ( auto flag : rhs )
		{
			remFlags( lhs, flag );
		}
	}

	void addFlags( SubmeshComponentCombine & lhs
		, SubmeshComponentFlag rhs )noexcept
	{
		lhs.flags.emplace( rhs );
	}

	void addFlags( SubmeshComponentCombine & lhs
		, SubmeshComponentFlagsSet const & rhs )noexcept
	{
		for ( auto flag : rhs )
		{
			addFlags( lhs, flag );
		}
	}

	bool contains( SubmeshComponentCombine const & cont
		, SubmeshComponentFlag test )noexcept
	{
		return cont.flags.find( test ) != cont.flags.end();
	}

	bool contains( SubmeshComponentCombine const & cont
		, SubmeshComponentCombine const & test )noexcept
	{
		if ( cont.baseId == test.baseId )
		{
			return true;
		}

		auto result = true;

		for ( auto flag : test.flags )
		{
			if ( result )
			{
				result = contains( cont, flag );
			}
		}

		return result;
	}

	Submesh * getComponentSubmesh( SubmeshComponent const & component )
	{
		return component.getOwner();
	}

	String const & getSubmeshComponentType( SubmeshComponent const & component )
	{
		return component.getType();
	}

	//*********************************************************************************************

	SubmeshComponentData::SubmeshComponentData( Submesh & submesh )
		: m_submesh{ submesh }
	{
	}

	bool SubmeshComponentData::initialise( RenderDevice const & device )
	{
		if ( !m_initialised || m_dirty )
		{
			m_initialised = doInitialise( device );
		}

		return m_initialised;
	}

	void SubmeshComponentData::cleanup( RenderDevice const & device )
	{
		if ( m_initialised )
		{
			doCleanup( device );
		}
	}

	void SubmeshComponentData::upload( UploadData & uploader )
	{
		if ( m_initialised && m_dirty )
		{
			doUpload( uploader );
			m_dirty = false;
		}
	}

	//*********************************************************************************************

	SubmeshComponent::SubmeshComponent( Submesh & submesh
		, String const & type
		, SubmeshComponentDataUPtr data
		, StringArray deps )
		: OwnedBy< Submesh >{ submesh }
		, m_data{ c3d::move( data ) }
		, m_type{ type }
		, m_dependencies{ c3d::move( deps ) }
		, m_id{ submesh.getComponentId( m_type ) }
		, m_plugin{ submesh.getComponentPlugin( m_id ) }
	{
	}

	void SubmeshComponent::initialiseRenderData()
	{
		auto const & engine = *getOwner()->getParent().getEngine();
		m_renderData = engine.getSubmeshComponentsRegister().createRenderData( *this );
	}

	bool SubmeshComponent::writeText( String const & tabs
		, StringStream & file )const
	{
		return doWriteText( tabs, file );
	}

	bool SubmeshComponent::writeBinary( BinaryChunk & chunk )const
	{
		return doWriteBinary( chunk );
	}

	//*********************************************************************************************
}
