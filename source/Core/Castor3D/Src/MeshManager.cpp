#include "MeshManager.hpp"

using namespace Castor;

namespace Castor3D
{
	const String CachedObjectNamer< Mesh >::Name = cuT( "Mesh" );

	MeshManager::MeshManager( Scene & p_scene )
		: ResourceManager< Castor::String, Mesh, Scene >( p_scene )
	{
	}

	MeshManager::~MeshManager()
	{
	}

	MeshSPtr MeshManager::Create( String const & p_name, eMESH_TYPE p_type )
	{
		return Create( p_name, p_type, UIntArray(), RealArray() );
	}

	MeshSPtr MeshManager::Create( String const & p_name, eMESH_TYPE p_type, UIntArray const & p_arrayFaces )
	{
		return Create( p_name, p_type, p_arrayFaces, RealArray() );
	}

	MeshSPtr MeshManager::Create( String const & p_name, eMESH_TYPE p_type, UIntArray const & p_arrayFaces, RealArray const & p_arraySizes )
	{
		std::unique_lock< Collection > l_lock( m_elements );
		MeshSPtr l_return;

		if ( !m_elements.has( p_name ) )
		{
			l_return = std::make_shared< Mesh >( p_name, *GetScene() );
			m_factory.Create( p_type )->Generate( *l_return, p_arrayFaces, p_arraySizes );
			m_elements.insert( p_name, l_return );
			Castor::Logger::LogInfo( Castor::StringStream() << INFO_MANAGER_CREATED_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
		}
		else
		{
			l_return = m_elements.find( p_name );
			Castor::Logger::LogWarning( Castor::StringStream() << WARNING_MANAGER_DUPLICATE_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
		}

		return  l_return;
	}
}
