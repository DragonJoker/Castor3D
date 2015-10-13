#include "MeshManager.hpp"

#include "Mesh.hpp"

using namespace Castor;

namespace Castor3D
{
	MeshManager::MeshManager( Engine & p_engine )
		: Manager< Castor::String, Mesh >( p_engine )
	{
	}

	MeshManager::~MeshManager()
	{
	}

	MeshSPtr MeshManager::Create( String const & p_strMeshName, eMESH_TYPE p_type )
	{
		return Create( p_strMeshName, p_type, UIntArray(), RealArray() );
	}

	MeshSPtr MeshManager::Create( String const & p_strMeshName, eMESH_TYPE p_type, UIntArray const & p_arrayFaces )
	{
		return Create( p_strMeshName, p_type, p_arrayFaces, RealArray() );
	}

	MeshSPtr MeshManager::Create( String const & p_strMeshName, eMESH_TYPE p_type, UIntArray const & p_arrayFaces, RealArray const & p_arraySizes )
	{
		m_elements.lock();
		MeshSPtr l_pReturn = m_elements.find( p_strMeshName );

		if ( !l_pReturn )
		{
			l_pReturn = std::make_shared< Mesh >( *GetOwner(), p_strMeshName );
			m_factory.Create( p_type )->Generate( *l_pReturn, p_arrayFaces, p_arraySizes );
			m_elements.insert( p_strMeshName, l_pReturn );
			Logger::LogInfo( cuT( "Engine::CreateMesh - Mesh [" ) + p_strMeshName + cuT( "] - Created" ) );
		}
		else
		{
			Logger::LogWarning( cuT( "Engine::CreateMesh - Can't create Mesh [" ) + p_strMeshName + cuT( "] - Another mesh with the same name already exists" ) );
		}

		m_elements.unlock();
		return  l_pReturn;
	}

	bool MeshManager::Save( BinaryFile & p_file )
	{
		m_elements.lock();
		Path l_path = p_file.GetFileFullPath();
		bool l_return = true;

		if ( l_path.GetExtension() == cuT( "cmsh" ) )
		{
			l_return = p_file.Write( uint32_t( m_elements.size() ) ) == sizeof( uint32_t );

			for ( auto l_it : m_elements )
			{
				if ( l_return )
				{
					l_return = Mesh::BinaryParser( l_path ).Fill( *l_it.second, p_file );
				}
			}
		}
		else
		{
			l_path = l_path.GetPath() / l_path.GetFileName() + cuT( ".cmsh" );
			BinaryFile l_file( l_path, File::eOPEN_MODE_WRITE );
			l_return = l_file.Write( uint32_t( m_elements.size() ) ) == sizeof( uint32_t );

			for ( auto l_it : m_elements )
			{
				if ( l_return )
				{
					l_return = Mesh::BinaryParser( l_path ).Fill( *l_it.second, p_file);
				}
			}
		}

		m_elements.unlock();
		return l_return;
	}

	bool MeshManager::Load( BinaryFile & p_file )
	{
		uint32_t l_uiSize;
		bool l_return = p_file.Read( l_uiSize ) == sizeof( uint32_t );

		for ( uint32_t i = 0; i < l_uiSize && l_return; i++ )
		{
			MeshSPtr l_pMesh = m_elements.find( cuEmptyString );

			if ( !l_pMesh )
			{
				l_pMesh = std::make_shared< Mesh >( *GetOwner(), cuEmptyString );
				m_elements.insert( cuEmptyString, l_pMesh );
			}

			l_return = Mesh::BinaryParser( p_file.GetFileFullPath() ).Parse( *l_pMesh, p_file );
		}

		return l_return;
	}
}
