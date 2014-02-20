#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Material.hpp"
#include "Castor3D/Pass.hpp"
#include "Castor3D/SceneFileParser.hpp"

using namespace Castor3D;
using namespace Castor;

//*********************************************************************************************

Material::BinaryLoader :: BinaryLoader()
	:	Loader< Material, eFILE_TYPE_BINARY, BinaryFile >( File::eOPEN_MODE_DUMMY )
{
}

bool Material::BinaryLoader :: operator ()( Material & p_material, BinaryFile & p_file)
{
	uint32_t l_nbPasses = 0;
	String l_strName;
	p_material.Cleanup();
	bool l_bReturn = p_file.Read( l_strName);

	if (l_bReturn)
	{
		p_material.ChangeName( l_strName);
		l_bReturn = (p_file.Read( l_nbPasses ) == sizeof( l_nbPasses ) );
	}

	if (l_bReturn)
	{
		for( uint32_t i = 0; i < l_nbPasses && l_bReturn; i++)
		{
			PassSPtr l_pass = p_material.CreatePass();
			l_bReturn = Pass::BinaryLoader()( *l_pass, p_file );
		}
	}

	return l_bReturn;
}

bool Material::BinaryLoader :: operator ()( Material const & p_material, BinaryFile & p_file)
{
	bool l_bReturn = p_file.Write( p_material.GetName());

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( p_material.GetPassCount() ) == sizeof( uint32_t ) );
	}

	if (l_bReturn)
	{
		std::for_each( p_material.Begin(), p_material.End(), [&]( PassSPtr p_pPass )
		{
			l_bReturn = Pass::BinaryLoader()( *p_pPass, p_file );
		} );
	}

	return l_bReturn;
}

//*************************************************************************************************

Material::TextLoader :: TextLoader( File::eENCODING_MODE p_eEncodingMode )
	:	Loader< Material, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
{
}

bool Material::TextLoader :: operator ()( Material const & p_material, TextFile & p_file)
{
	bool l_bReturn = p_file.WriteText( cuT( "material " ) + p_material.GetName() + cuT( "\n" ) ) > 0;

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteText( cuT( "{\n" ) ) > 0;
	}

	bool l_bFirst = true;
	
	for( PassPtrArrayConstIt l_it = p_material.Begin(); l_it != p_material.End(); ++l_it )
	{
		if( l_bFirst )
		{
			l_bFirst = false;
		}
		else
		{
			p_file.WriteText( cuT( "\n" ) );
		}

		l_bReturn = Pass::TextLoader()( *(*l_it), p_file );
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteText( cuT( "}\n" ) ) > 0;
	}

	return l_bReturn;
}

//*********************************************************************************************

Material :: Material( Engine * p_pEngine, String const & p_name, int p_iNbInitialPasses)
	:	Resource<Material>( p_name )
	,	m_pEngine( p_pEngine )
{
	for (int i = 0; i < p_iNbInitialPasses; i++)
	{
		CreatePass();
	}
}

Material :: ~Material()
{
}

void Material :: Initialise()
{
	Logger::LogDebug( cuT( "Initialising material [" ) + GetName() + cuT( "]" ) );

	std::for_each( m_passes.begin(), m_passes.end(), [&]( PassSPtr p_pPass )
	{
		p_pPass->Initialise();
	} );
}

void Material :: Cleanup()
{
	std::for_each( m_passes.begin(), m_passes.end(), [&]( PassSPtr p_pPass )
	{
		p_pPass->Cleanup();
	} );
}

void Material :: Render()
{
	uint8_t l_byIndex = 0;
	uint8_t l_byCount = uint8_t( m_passes.size() );

	std::for_each( m_passes.begin(), m_passes.end(), [&]( PassSPtr p_pPass )
	{
		p_pPass->Render( l_byIndex++, l_byCount );
	} );
}

void Material :: Render2D()
{
	uint8_t l_byIndex = 0;
	uint8_t l_byCount = uint8_t( m_passes.size() );

	std::for_each( m_passes.begin(), m_passes.end(), [&]( PassSPtr p_pPass )
	{
		p_pPass->Render2D( l_byIndex++, l_byCount );
	} );
}

void Material :: EndRender()
{
	std::for_each( m_passes.begin(), m_passes.end(), [&]( PassSPtr p_pPass )
	{
		p_pPass->EndRender();
	} );
}

PassSPtr Material :: CreatePass()
{
	PassSPtr l_newPass = std::make_shared< Pass >( m_pEngine, this );
	m_passes.push_back( l_newPass);
	return l_newPass;
}

const PassSPtr Material :: GetPass( uint32_t p_index)const
{
	CASTOR_ASSERT( p_index < m_passes.size());
	return m_passes[p_index];
}

PassSPtr Material :: GetPass( uint32_t p_index)
{
	CASTOR_ASSERT( p_index < m_passes.size());
	return m_passes[p_index];
}

void Material :: DestroyPass( uint32_t p_index)
{
	CASTOR_ASSERT( p_index < m_passes.size());
	m_passes.erase( m_passes.begin() + p_index);
}

bool Material :: HasAlphaBlending()const
{
	bool l_bReturn = true;
	uint32_t l_uiCount = GetPassCount();

	for( uint32_t i = 0; i < l_uiCount && l_bReturn; i++ )
	{
		l_bReturn = m_passes[i]->HasAlphaBlending();
	}

	return l_bReturn;
}

//*************************************************************************************************

MaterialManager :: MaterialManager( Engine * p_pEngine )
	:	 m_pEngine( p_pEngine )
{
}

MaterialManager :: ~MaterialManager()
{
	m_defaultMaterial.reset();
}

void MaterialManager :: Initialise()
{
	m_defaultMaterial = MaterialCollection::find( cuT( "DefaultMaterial" ) );

	if( !m_defaultMaterial )
	{
		m_defaultMaterial = std::make_shared< Material >( m_pEngine, cuT( "DefaultMaterial" ), 1 );
		m_defaultMaterial->GetPass( 0 )->SetDoubleFace( true );
		MaterialCollection::insert( cuT( "DefaultMaterial" ), m_defaultMaterial );
	}

	m_pEngine->PostEvent( std::make_shared< InitialiseEvent< Material > >( *m_defaultMaterial ) );
}

void MaterialManager :: Cleanup()
{
	MaterialCollection::lock();

	std::for_each( begin(), end(), [&]( std::pair< String, MaterialSPtr > p_pair )
	{
		m_pEngine->PostEvent( std::make_shared< CleanupEvent< Material > >( *p_pair.second ) );
	} );

	MaterialCollection::unlock();
}

void MaterialManager :: DeleteAll()
{
	m_defaultMaterial.reset();
	MaterialCollection::clear();
}

void MaterialManager :: GetNames( StringArray & l_names)
{
	MaterialCollection::lock();
	l_names.clear();
	MaterialCollectionConstIt l_it = begin();

	while (l_it != end())
	{
		l_names.push_back( l_it->first);
		l_it++;
	}

	MaterialCollection::unlock();
}

bool MaterialManager :: Write( TextFile & p_file)const
{
	MaterialCollection::lock();
	bool l_bReturn = true;
	MaterialCollectionConstIt l_it = begin();

	bool l_bFirst = true;

	while (l_bReturn && l_it != end())
	{
		if (l_bFirst)
		{
			l_bFirst = false;
		}
		else
		{
			p_file.WriteText( cuT( "\n" ) );
		}

		l_bReturn = Material::TextLoader()( * l_it->second, p_file);
		++l_it;
	}

	MaterialCollection::unlock();
	return l_bReturn;
}

bool MaterialManager :: Read( TextFile & p_file)
{
	SceneFileParser l_parser( m_pEngine );
	l_parser.ParseFile( p_file);
	return true;
}

bool MaterialManager :: Save( BinaryFile & p_file)const
{
	MaterialCollection::lock();
	uint32_t l_uiSize = uint32_t( size() );
	bool l_bReturn = p_file.Write( l_uiSize ) == sizeof( l_uiSize );
	MaterialCollectionConstIt l_it = begin();

	while( l_bReturn && l_it != end() )
	{
		l_bReturn = Material::BinaryLoader()( * l_it->second, p_file);
		++l_it;
	}

	MaterialCollection::unlock();
	return l_bReturn;
}

bool MaterialManager :: Load( BinaryFile & p_file)
{
	uint32_t l_uiSize;
	bool l_bReturn = p_file.Read( l_uiSize) == sizeof( uint32_t );

	for (uint32_t i = 0; i < l_uiSize && l_bReturn; i++)
	{
		MaterialSPtr l_pMaterial = MaterialCollection::find( cuEmptyString );

		if ( ! l_pMaterial)
		{
			l_pMaterial = std::make_shared< Material >( m_pEngine, cuEmptyString, 0 );
			MaterialCollection::insert( cuEmptyString, l_pMaterial );
		}

		l_bReturn = Material::BinaryLoader()( * l_pMaterial, p_file );
	}

	return l_bReturn;
}

//*************************************************************************************************