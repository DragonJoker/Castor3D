#include "SubmeshComponent/BonesComponent.hpp"
#include "SubmeshComponent/BonesInstantiationComponent.hpp"
#include "SubmeshComponent/InstantiationComponent.hpp"

namespace castor3d
{
	//*********************************************************************************************

	template< typename T >
	inline void SubmeshComponentAdder< T >::add( std::shared_ptr< T > component
		, Submesh & submesh )
	{
		submesh.m_components.emplace( T::Name, component );
	}

	//*********************************************************************************************

	template<>
	struct SubmeshComponentAdder< BonesInstantiationComponent >
	{
		static inline void add( std::shared_ptr< BonesInstantiationComponent > component
			, Submesh & submesh )
		{
			submesh.m_components.emplace( BonesInstantiationComponent::Name, component );

			if ( submesh.m_instantiatedBones != component )
			{
				submesh.m_instantiatedBones = component;
			}
		}
	};

	//*********************************************************************************************

	template<>
	struct SubmeshComponentAdder< InstantiationComponent >
	{
		static inline void add( std::shared_ptr< InstantiationComponent > component
			, Submesh & submesh )
		{
			submesh.m_components.emplace( InstantiationComponent::Name, component );

			if ( submesh.m_instantiation != component )
			{
				submesh.m_instantiation = component;
			}
		}
	};

	//*********************************************************************************************

	template<>
	struct SubmeshComponentAdder< BonesComponent >
	{
		static inline void add( std::shared_ptr< BonesComponent > component
			, Submesh & submesh )
		{
			submesh.m_components.emplace( BonesComponent::Name, component );

			if ( !submesh.m_instantiatedBones )
			{
				submesh.addComponent( std::make_shared< BonesInstantiationComponent >( submesh
					, submesh.getInstantiation()
					, *component ) );
			}
		}
	};

	//*********************************************************************************************

	inline void Submesh::addPoints( std::vector< InterleavedVertex > const & p_vertices )
	{
		addPoints( p_vertices.data(), p_vertices.data() + p_vertices.size() );
	}

	template< size_t Count >
	inline void Submesh::addPoints( std::array< InterleavedVertex, Count > const & p_vertices )
	{
		addPoints( p_vertices.data(), p_vertices.data() + p_vertices.size() );
	}

	inline void Submesh::addFaceGroup( std::vector< FaceIndices > const & p_faces )
	{
		addFaceGroup( p_faces.data(), p_faces.data() + p_faces.size() );
	}

	template< size_t Count >
	inline void Submesh::addFaceGroup( std::array< FaceIndices, Count > const & p_faces )
	{
		addFaceGroup( p_faces.data(), p_faces.data() + p_faces.size() );
	}

	inline SkeletonSPtr Submesh::getSkeleton()const
	{
		return getParent().getSkeleton();
	}

	inline void Submesh::setDefaultMaterial( MaterialSPtr p_mat )
	{
		m_defaultMaterial = p_mat;
		setMaterial( nullptr, p_mat, false );
	}

	inline BufferElementGroupSPtr Submesh::operator[]( uint32_t p_index )const
	{
		REQUIRE( p_index < m_points.size() );
		return m_points[p_index];
	}

	inline BufferElementGroupSPtr Submesh::getPoint( uint32_t p_index )const
	{
		REQUIRE( p_index < m_points.size() );
		return m_points[p_index];
	}

	inline MaterialSPtr Submesh::getDefaultMaterial()const
	{
		return m_defaultMaterial.lock();
	}

	inline castor::CubeBox const & Submesh::getCollisionBox()const
	{
		return m_box;
	}

	inline castor::CubeBox & Submesh::getCollisionBox()
	{
		return m_box;
	}

	inline castor::SphereBox const & Submesh::getCollisionSphere()const
	{
		return m_sphere;
	}

	inline castor::SphereBox & Submesh::getCollisionSphere()
	{
		return m_sphere;
	}

	inline VertexPtrArray const & Submesh::getPoints()const
	{
		return m_points;
	}

	inline VertexPtrArray & Submesh::getPoints()
	{
		return m_points;
	}

	inline Face const & Submesh::getFace( uint32_t p_index )const
	{
		REQUIRE( p_index < m_faces.size() );
		return m_faces[p_index];
	}

	inline FaceArray const & Submesh::getFaces()const
	{
		return m_faces;
	}

	inline FaceArray & Submesh::getFaces()
	{
		return m_faces;
	}

	inline VertexBuffer const & Submesh::getVertexBuffer()const
	{
		return m_vertexBuffer;
	}

	inline VertexBuffer & Submesh::getVertexBuffer()
	{
		return m_vertexBuffer;
	}

	inline IndexBuffer const & Submesh::getIndexBuffer()const
	{
		return m_indexBuffer;
	}

	inline IndexBuffer & Submesh::getIndexBuffer()
	{
		return m_indexBuffer;
	}

	inline bool Submesh::isInitialised()const
	{
		return m_initialised;
	}

	inline Mesh const & Submesh::getParent()const
	{
		return m_parentMesh;
	}

	inline Mesh & Submesh::getParent()
	{
		return m_parentMesh;
	}

	inline uint32_t Submesh::getId()const
	{
		return m_id;
	}

	inline void Submesh::needsUpdate()
	{
		m_dirty = true;
	}

	template< uint32_t Count >
	void Submesh::addFaceGroup( FaceIndices( & p_faces )[Count] )
	{
		addFaceGroup( p_faces, Count );
	}

	inline bool Submesh::hasComponent( castor::String const & name )
	{
		return m_components.find( name ) != m_components.end();
	}

	inline void Submesh::addComponent( castor::String const & name
		, SubmeshComponentSPtr component )
	{
		m_components.emplace( name, component );
	}

	template< typename T >
	inline void Submesh::addComponent( std::shared_ptr< T > component )
	{
		SubmeshComponentAdder< T >::add( component, *this );
	}

	inline InstantiationComponent & Submesh::getInstantiation()
	{
		REQUIRE( m_instantiation );
		return *m_instantiation;
	}

	inline InstantiationComponent const & Submesh::getInstantiation()const
	{
		REQUIRE( m_instantiation );
		return *m_instantiation;
	}

	inline BonesInstantiationComponent & Submesh::getInstantiatedBones()
	{
		REQUIRE( m_instantiatedBones );
		return *m_instantiatedBones;
	}

	inline BonesInstantiationComponent const & Submesh::getInstantiatedBones()const
	{
		REQUIRE( m_instantiatedBones );
		return *m_instantiatedBones;
	}

	inline SubmeshComponentStrMap const & Submesh::getComponents()const
	{
		return m_components;
	}

	//*********************************************************************************************
}
