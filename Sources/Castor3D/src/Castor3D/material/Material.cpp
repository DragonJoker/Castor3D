#include "PrecompiledHeader.h"

#include "material/Module_Material.h"

#include "material/Material.h"
#include "material/Pass.h"
#include "geometry/mesh/Submesh.h"
#include "material/MaterialManager.h"
#include "render_system/RenderSystem.h"
#include "main/Root.h"

using namespace Castor3D;
using namespace Castor::Utils;

//*********************************************************************************************

MaterialPtr MaterialLoader :: LoadFromFile( const String & p_file)
{
	MaterialPtr l_pReturn;
	bool l_bResult;
	File l_file( p_file, File::eRead);

	size_t l_nameLength = 0;
	l_bResult = (l_file.Read( l_nameLength) == sizeof( size_t));

	if (l_bResult)
	{
		Char * l_name = new Char[l_nameLength+1];
		l_bResult = (l_file.ReadArray( l_name, l_nameLength) == l_nameLength);

		if (l_bResult)
		{
			l_name[l_nameLength] = 0;

			Log::LogMessage( CU_T( "Reading material ") + String( l_name));

			l_pReturn = MaterialManager::CreateMaterial( l_name);
		}

		delete [] l_name;
	}

	size_t l_nbPasses = 0;

	if (l_bResult)
	{
		l_bResult = (l_file.Read( l_nbPasses) == sizeof( size_t));
	}

	if (l_bResult)
	{
		PassPtr l_pass;

		for (size_t i = 0 ; i < l_nbPasses && l_bResult ; i++)
		{
			l_pass = l_pReturn->CreatePass();
			l_bResult = l_pass->Read( l_file);
/*
			if ( ! l_bResult)
			{
				l_pReturn.reset();
				return NULL;
			}
*/
		}
	}

	if ( ! l_bResult && ! l_pReturn.null())
	{
		l_pReturn.reset();
	}

	return l_pReturn;
}

bool MaterialLoader :: SaveToFile( File & p_pFile, MaterialPtr p_material)
{
	bool l_bReturn = p_pFile.WriteLine( "material " + p_material->GetName() + "\n");

	if (l_bReturn)
	{
		l_bReturn = p_pFile.WriteLine( "{\n");
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
			p_pFile.WriteLine( "\n");
		}

		l_bReturn = p_material->GetPass( i)->Write( p_pFile);
	}

	if (l_bReturn)
	{
		l_bReturn = p_pFile.WriteLine( "}\n");
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
//	vector::deleteAll( m_passes);
}

bool Material :: SetName( const String & p_name)
{
	bool l_bReturn = false;

	if ( ! MaterialManager::HasElement( p_name))
	{
		MaterialPtr l_pThis = MaterialManager::RemoveElement( m_name);
		m_name = p_name;
		MaterialManager::AddElement( l_pThis);
		l_bReturn = true;
	}

	return l_bReturn;
}

void Material :: Initialise()
{
	for (size_t i = 0 ; i < m_passes.size() ; i++)
	{
		m_passes[i]->Initialise();
	}
//	vector::cycle( m_passes, & Pass::Initialise);
}

void Material :: Apply( eDRAW_TYPE p_displayMode)
{
	for (size_t i = 0 ; i < m_passes.size() ; i++)
	{
		m_passes[i]->Apply( p_displayMode);
	}
//	vector::cycle( m_passes, & Pass::Apply, p_submesh, p_displayMode);
}

void Material :: Apply2D( eDRAW_TYPE p_displayMode)
{
	for (size_t i = 0 ; i < m_passes.size() ; i++)
	{
		m_passes[i]->Apply2D( p_displayMode);
	}
//	vector::cycle( m_passes, & Pass::Apply, p_displayMode);
}

void Material :: Remove()
{
	for (size_t i = 0 ; i < m_passes.size() ; i++)
	{
		m_passes[i]->Remove();
	}
//	vector::cycle( m_passes, & Pass::Remove);
}

PassPtr Material :: CreatePass()
{
	PassPtr l_newPass = new Pass( this);
	m_passes.push_back( l_newPass);
	return l_newPass;
}

PassPtr Material :: GetPass( unsigned int p_index)
{
	PassPtr l_pReturn;

	if (p_index < m_passes.size())
	{
		l_pReturn = m_passes[p_index];
	}

	return l_pReturn;
}

void Material :: DestroyPass( unsigned int p_index)
{
	if (p_index < m_passes.size())
	{
		PassPtr l_pass = m_passes[p_index];
		vector::eraseValue( m_passes, l_pass);
		l_pass.reset();
//		delete l_pass;
	}
}