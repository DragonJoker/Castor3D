#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/material/Material.h"
#include "Castor3D/material/Pass.h"
#include "Castor3D/geometry/mesh/Submesh.h"
#include "Castor3D/material/MaterialManager.h"
#include "Castor3D/render_system/RenderSystem.h"
#include "Castor3D/main/Root.h"

using namespace Castor3D;
using namespace Castor::Utils;

//*********************************************************************************************

MaterialPtr MaterialLoader :: LoadFromFile( MaterialManager * p_pManager, const String & p_file)
{
	MaterialPtr l_pReturn;
	bool l_bResult;
	File l_file( p_file, File::eRead);

	size_t l_nameLength = 0;
	l_bResult = (l_file.Read( l_nameLength) == sizeof( size_t));

	if (l_bResult)
	{
		xchar * l_name = new xchar[l_nameLength+1];
		l_bResult = (l_file.ReadArray( l_name, l_nameLength) == l_nameLength);

		if (l_bResult)
		{
			l_name[l_nameLength] = 0;

			Logger::LogMessage( CU_T( "Reading material ") + String( l_name));

			l_pReturn = p_pManager->CreateMaterial( l_name);
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

		for (size_t i = 0 ; i < l_nbPasses && l_bResult ; i++)
		{
			PassPtr l_pass = l_pReturn->CreatePass();
			l_bResult = l_pass->Read( l_file);
		}
	}

	if ( ! l_bResult && l_pReturn != NULL)
	{
		l_pReturn.reset();
	}

	return l_pReturn;
}

bool MaterialLoader :: SaveToFile( File & p_file, MaterialPtr p_pMaterial)
{
	return p_pMaterial->Write( p_file);
}

//*********************************************************************************************

Material :: Material( Manager<Material> * p_pManager, const String & p_name, int p_iNbInitialPasses)
	:	Resource<Material>( p_pManager, p_name)
{
	for (int i = 0 ; i < p_iNbInitialPasses ; i++)
	{
		CreatePass();
	}
}

Material :: ~Material()
{
}

void Material :: Initialise()
{
	for (size_t i = 0 ; i < m_passes.size() ; i++)
	{
		m_passes[i]->Initialise();
	}
}

void Material :: Render( ePRIMITIVE_TYPE p_displayMode)
{
	for (size_t i = 0 ; i < m_passes.size() ; i++)
	{
		m_passes[i]->Render( p_displayMode);
	}
}

void Material :: Render2D()
{
	for (size_t i = 0 ; i < m_passes.size() ; i++)
	{
		m_passes[i]->Render2D();
	}
}

void Material :: EndRender()
{
	for (size_t i = 0 ; i < m_passes.size() ; i++)
	{
		m_passes[i]->EndRender();
	}
}

PassPtr Material :: CreatePass()
{
	PassPtr l_newPass( new Pass( this));
	m_passes.push_back( l_newPass);
	return l_newPass;
}

const PassPtr Material :: GetPass( unsigned int p_index)const
{
	CASTOR_ASSERT( p_index < m_passes.size());
	return m_passes[p_index];
}

PassPtr Material :: GetPass( unsigned int p_index)
{
	CASTOR_ASSERT( p_index < m_passes.size());
	return m_passes[p_index];
}

void Material :: DestroyPass( unsigned int p_index)
{
	CASTOR_ASSERT( p_index < m_passes.size());
	m_passes.erase( m_passes.begin() + p_index);
}

bool Material :: Write( File & p_file)const
{
	bool l_bReturn = p_file.WriteLine( "material " + m_strName + "\n") > 0;

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( "{\n") > 0;
	}

	size_t l_nbPasses = GetNbPasses();
	bool l_bFirst = true;

	for (size_t i = 0 ; i < l_nbPasses && l_bReturn ; i++)
	{
		if (l_bFirst)
		{
			l_bFirst = false;
		}
		else
		{
			p_file.WriteLine( "\n");
		}

		l_bReturn = GetPass( i)->Write( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( "}\n") > 0;
	}

	return l_bReturn;
}

bool Material :: Save( File & p_file)const
{
	bool l_bReturn = p_file.Write( m_strName);

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( m_passes.size()) == sizeof( size_t);
	}

	if (l_bReturn)
	{
		for (size_t i = 0 ; i < m_passes.size() && l_bReturn ; i++)
		{
			l_bReturn = m_passes[i]->Save( p_file);
		}
	}

	return l_bReturn;
}

bool Material :: Load( File & p_file)
{
	bool l_bReturn = p_file.Read( m_strName);
	size_t l_uiSize;

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( l_uiSize) == sizeof( size_t);
	}

	if (l_bReturn)
	{
		for (size_t i = 0 ; i < l_uiSize && l_bReturn ; i++)
		{
			PassPtr l_pPass = CreatePass();
			l_bReturn = l_pPass->Load( p_file);
		}
	}

	return l_bReturn;
}