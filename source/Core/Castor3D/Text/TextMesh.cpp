#include "Castor3D/Model/Mesh/Mesh.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	Mesh::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< Mesh >{ tabs }
	{
	}

	bool Mesh::TextWriter::operator()( Mesh const & object
		, TextFile & file )
	{
		log::info << m_tabs << cuT( "Writing Mesh " ) << object.getName() << std::endl;
		auto result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "mesh \"" ) + object.getName() + cuT( "\"\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "\timport \"Meshes/" ) + object.getName() + cuT( ".cmsh\"\n" ) ) > 0;
		castor::TextWriter< Mesh >::checkError( result, "Mesh" );

		if ( result )
		{
			auto it = std::find_if( object.begin()
				, object.end()
				, []( SubmeshSPtr lookup )
				{
					return lookup->getDefaultMaterial() != nullptr;
				} );

			if ( it != object.end() )
			{
				if ( object.getSubmeshCount() == 1 )
				{
					result = file.writeText( m_tabs + cuT( "\tdefault_material \"" ) + object.getSubmesh( 0u )->getDefaultMaterial()->getName() + cuT( "\"\n" ) ) > 0;
				}
				else
				{
					result = file.writeText( m_tabs + cuT( "\tdefault_materials\n" ) ) > 0
						&& file.writeText( m_tabs + cuT( "\t{\n" ) ) > 0;

					for ( auto & submesh : object )
					{
						if ( submesh->getDefaultMaterial() )
						{
							result = file.writeText( m_tabs + cuT( "\t\tmaterial " ) + string::toString( submesh->getId(), std::locale{ "C" } ) + cuT( " \"" ) + submesh->getDefaultMaterial()->getName() + cuT( "\"\n" ) ) > 0;
						}
					}

					result = file.writeText( m_tabs + cuT( "\t}\n" ) ) > 0;
				}
			}
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*************************************************************************************************

	Mesh::Mesh( String const & name, Scene & scene )
		: Resource< Mesh >{ name }
		, Animable{ *scene.getEngine() }
		, m_scene{ &scene }
		, m_modified{ false }
	{
	}

	Mesh::~Mesh()
	{
		CU_Assert( m_submeshes.empty(), "Did you forget to call Mesh::cleanup ?" );
	}

	void Mesh::cleanup()
	{
		Animable::cleanupAnimations();

		for ( auto submesh : m_submeshes )
		{
			submesh->cleanup();
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

	SubmeshSPtr Mesh::createSubmesh()
	{
		auto submesh = std::make_shared< Submesh >( *this, getSubmeshCount() );
		m_submeshes.push_back( submesh );
		return submesh;
	}

	void Mesh::deleteSubmesh( SubmeshSPtr & submesh )
	{
		auto it = std::find( m_submeshes.begin(), m_submeshes.end(), submesh );

		if ( it != m_submeshes.end() )
		{
			m_submeshes.erase( it );
			submesh.reset();
			it = m_submeshes.end();
		}
	}

	void Mesh::computeNormals( bool reverted )
	{
		for ( auto submesh : m_submeshes )
		{
			submesh->computeNormals( reverted );
		}
	}

	void Mesh::setSkeleton( SkeletonSPtr skeleton )
	{
		m_skeleton = skeleton;
		m_skeleton->computeContainers( *this );
	}

	MeshAnimation & Mesh::createAnimation( castor::String const & name )
	{
		if ( !hasAnimation( name ) )
		{
			doAddAnimation( std::make_unique< MeshAnimation >( *this, name ) );
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

	//*************************************************************************************************
}
