#include "MeshManager.hpp"

#include "Mesh.hpp"

using namespace Castor;

namespace Castor3D
{
	MeshManager::MeshManager( Engine & p_engine )
		: ResourceManager< Castor::String, Mesh >( p_engine )
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
			l_return = std::make_shared< Mesh >( p_name, *GetEngine() );
			m_factory.Create( p_type )->Generate( *l_return, p_arrayFaces, p_arraySizes );
			m_elements.insert( p_name, l_return );
			Logger::LogInfo( cuT( "MeshManager::Create - Created Mesh: " ) + p_name + cuT( "" ) );
		}
		else
		{
			l_return = m_elements.find( p_name );
			Logger::LogWarning( cuT( "MeshManager::Create - Duplicate Mesh: " ) + p_name );
		}

		return  l_return;
	}

	bool MeshManager::Save( BinaryFile & p_file )
	{
		std::unique_lock< Collection > l_lock( m_elements );
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
					l_return = Mesh::BinaryParser( l_path ).Fill( *l_it.second, p_file );
				}
			}
		}

		return l_return;
	}

	bool MeshManager::Load( BinaryFile & p_file )
	{
		std::unique_lock< Collection > l_lock( m_elements );
		uint32_t l_uiSize;
		bool l_return = p_file.Read( l_uiSize ) == sizeof( uint32_t );

		for ( uint32_t i = 0; i < l_uiSize && l_return; i++ )
		{
			MeshSPtr l_mesh = m_elements.find( cuEmptyString );

			if ( !l_mesh )
			{
				l_mesh = std::make_shared< Mesh >( cuEmptyString, *GetEngine() );
				l_return = Mesh::BinaryParser( p_file.GetFileFullPath() ).Parse( *l_mesh, p_file );
				m_elements.insert( cuEmptyString, l_mesh );
			}
			else
			{
				// Mesh already loaded, don't do anything.
				l_return = true;
			}

		}

		return l_return;
	}
}
