#include "PrecompiledHeader.h"

#include "material/Module_Material.h"

#include "material/Material.h"
#include "material/Pass.h"
#include "geometry/mesh/Submesh.h"
#include "material/MaterialManager.h"
#include "render_system/RenderSystem.h"
#include "main/Root.h"

#include "Log.h"

using namespace Castor3D;
//*********************************************************************************************

Material * MaterialLoader :: LoadFromFileIO( const String & p_file)
{
	Material * l_pReturn = NULL;
	bool l_bResult;
	FileIO l_file( p_file, FileIO::eRead);

	size_t l_nameLength = 0;
	l_bResult = (l_file.Read<size_t>( l_nameLength) == sizeof( size_t));

	if (l_bResult)
	{
		Char * l_name = new Char[l_nameLength+1];
		l_bResult = (l_file.ReadArray<Char>( l_name, l_nameLength) == l_nameLength);

		if (l_bResult)
		{
			l_name[l_nameLength] = 0;

			Log::LogMessage( C3D_T( "Reading material ") + String( l_name));

			l_pReturn = MaterialManager::GetSingletonPtr()->CreateMaterial( l_name);
		}

		delete [] l_name;
	}

	size_t l_nbPasses = 0;

	if (l_bResult)
	{
		l_bResult = (l_file.Read<size_t>( l_nbPasses) == sizeof( size_t));
	}

	if (l_bResult)
	{
		Pass * l_pass;

		for (size_t i = 0 ; i < l_nbPasses && l_bResult ; i++)
		{
			l_pass = l_pReturn->CreatePass();
			l_bResult = l_pass->Read( l_file);

			if ( ! l_bResult)
			{
				delete l_pReturn;
				return NULL;
			}
		}
	}

	if ( ! l_bResult && l_pReturn != NULL)
	{
		delete l_pReturn;
		l_pReturn = NULL;
	}

	return l_pReturn;
}

bool MaterialLoader :: SaveToFileIO( FileIO * p_pFile, Material * p_material)
{
	bool l_bReturn = p_pFile->WriteLine( "material " + p_material->GetName() + "\n");

	if (l_bReturn)
	{
		l_bReturn = p_pFile->WriteLine( "{\n");
	}

	size_t l_nbPasses = p_material->GetNbPasses();
	bool l_bFirst = true;

	for (size_t i = 0 ; i < l_nbPasses && l_bReturn ; i++)
	{
		if (l_bFirst)
		{
			l_bFirst = false;
		}
		else
		{
			p_pFile->WriteLine( "\n");
		}

		l_bReturn = p_material->GetPass( i)->Write( p_pFile);
	}

	if (l_bReturn)
	{
		l_bReturn = p_pFile->WriteLine( "}\n");
	}

	return l_bReturn;
}

//*********************************************************************************************

Material :: Material( const String & p_name, int p_iNbInitialPasses)
	:	Resource( p_name)
{
	m_passes.clear();

	for (int i = 0 ; i < p_iNbInitialPasses ; i++)
	{
		CreatePass();
	}
}

Material :: ~Material()
{
	vector::deleteAll( m_passes);
}

bool Material :: SetName( const String & p_name)
{
	bool l_bReturn = false;
	if ( ! MaterialManager::GetSingletonPtr()->HasElement( p_name))
	{
		MaterialManager::GetSingletonPtr()->RemoveElement( m_name);
		m_name = p_name;
		MaterialManager::GetSingletonPtr()->AddElement( this);
		l_bReturn = true;
	}

	return l_bReturn;
}

void Material :: Initialise()
{
	vector::cycle( m_passes, & Pass::Initialise);
}

void Material :: Apply( Submesh * p_submesh, DrawType p_displayMode)
{
	vector::cycle( m_passes, & Pass::Apply, p_submesh, p_displayMode);
}

void Material :: Apply( DrawType p_displayMode)
{
	vector::cycle( m_passes, & Pass::Apply, p_displayMode);
}

void Material :: Remove()
{
	vector::cycle( m_passes, & Pass::Remove);
}

Pass * Material :: CreatePass()
{
	Pass * l_newPass = new Pass( RenderSystem::GetSingletonPtr()->CreatePassRenderer(), this);
	m_passes.push_back( l_newPass);
	return l_newPass;
}

Pass * Material :: GetPass( unsigned int p_index)
{
	if (p_index >= m_passes.size())
	{
		return NULL;
	}

	return m_passes[p_index];
}

void Material :: DestroyPass( unsigned int p_index)
{
	if (p_index < m_passes.size())
	{
		Pass * l_pass = m_passes[p_index];
		vector::eraseValue( m_passes, l_pass);
		delete l_pass;
	}
}