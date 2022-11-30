#include "Castor3D/Model/Mesh/Mesh.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"

CU_ImplementCUSmartPtr( castor3d, MeshCache )

namespace castor3d
{
	const castor::String ResourceCacheTraitsT< castor3d::Mesh, castor::String >::Name = cuT( "Mesh" );

	Mesh::Mesh( castor::String const & name, Scene & scene )
		: castor::Named{ name }
		, Animable{ *scene.getEngine() }
		, m_scene{ &scene }
		, m_modified{ false }
	{
	}

	Mesh::~Mesh()
	{
		CU_Assert( m_submeshes.empty(), "Did you forget to call Mesh::cleanup ?" );
	}

	void Mesh::initialise()
	{
		for ( auto submesh : m_submeshes )
		{
			submesh->initialise( getEngine()->getRenderSystem()->getRenderDevice() );
		}
	}

	void Mesh::cleanup()
	{
		Animable::cleanupAnimations();

		for ( auto submesh : m_submeshes )
		{
			submesh->cleanup( getEngine()->getRenderSystem()->getRenderDevice() );
		}

		m_submeshes.clear();
	}

	void Mesh::updateContainers()
	{
		if ( !m_submeshes.empty() )
		{
			m_box = m_submeshes[0]->getBoundingBox();

			for ( auto i = 1u; i < m_submeshes.size(); ++i )
			{
				m_box = m_box.getUnion( m_submeshes[i]->getBoundingBox() );
			}

			m_sphere.load( m_box );
		}
	}

	void Mesh::computeContainers()
	{
		for ( auto & submesh : m_submeshes )
		{
			submesh->computeContainers();
		}

		updateContainers();
	}

	uint32_t Mesh::getFaceCount()const
	{
		uint32_t nbFaces = 0;

		for ( auto submesh : m_submeshes )
		{
			nbFaces += submesh->getFaceCount();
		}

		return nbFaces;
	}

	uint32_t Mesh::getVertexCount()const
	{
		uint32_t nbFaces = 0;

		for ( auto submesh : m_submeshes )
		{
			nbFaces += submesh->getPointsCount();
		}

		return nbFaces;
	}

	SubmeshSPtr Mesh::getSubmesh( uint32_t index )const
	{
		SubmeshSPtr result;

		if ( index < m_submeshes.size() )
		{
			result = m_submeshes[index];
		}

		return result;
	}

	SubmeshSPtr Mesh::createSubmesh( SubmeshFlags const & flags )
	{
		auto submesh = std::make_shared< Submesh >( *this
			, getSubmeshCount()
			, flags );
		m_submeshes.push_back( submesh );
		return submesh;
	}

	void Mesh::deleteSubmesh( SubmeshRPtr submesh )
	{
		auto it = std::find_if( m_submeshes.begin()
			, m_submeshes.end()
			, [&submesh]( SubmeshSPtr lookup )
			{
				return submesh == lookup.get();
			} );

		if ( it != m_submeshes.end() )
		{
			m_submeshes.erase( it );
		}
	}

	void Mesh::deleteSubmesh( SubmeshSPtr & submesh )
	{
		auto it = std::find( m_submeshes.begin(), m_submeshes.end(), submesh );

		if ( it != m_submeshes.end() )
		{
			m_submeshes.erase( it );
			submesh.reset();
		}
	}

	void Mesh::computeNormals( bool reverted )
	{
		for ( auto submesh : m_submeshes )
		{
			submesh->computeNormals( reverted );
		}
	}

	void Mesh::setSkeleton( SkeletonRPtr skeleton )
	{
		m_skeleton = skeleton;
		m_skeleton->computeContainers( *this );
	}

	MeshAnimation & Mesh::createAnimation( castor::String const & name )
	{
		if ( !hasAnimation( name ) )
		{
			addAnimation( std::make_unique< MeshAnimation >( *this, name ) );
		}

		return doGetAnimation< MeshAnimation >( name );
	}

	void Mesh::removeAnimation( castor::String const & name )
	{
		if ( hasAnimation( name ) )
		{
			doRemoveAnimation( name );
		}
	}
}
