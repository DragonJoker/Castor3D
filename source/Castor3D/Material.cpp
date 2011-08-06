#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Material.hpp"
#include "Castor3D/Pass.hpp"
#include "Castor3D/Submesh.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/Root.hpp"

using namespace Castor3D;
using namespace Castor::Utils;

//*********************************************************************************************

bool Loader<Material> :: Load( Material & p_material, File & p_file)
{
	size_t l_nbPasses = 0;
	String l_strName;
	p_material.Cleanup();
	bool l_bReturn = p_file.Read( l_strName);

	if (l_bReturn)
	{
		p_material.SetName( l_strName);
		l_bReturn = (p_file.Read( l_nbPasses) == sizeof( size_t));
	}

	if (l_bReturn)
	{
		for (size_t i = 0 ; i < l_nbPasses && l_bReturn ; i++)
		{
			PassPtr l_pass = p_material.CreatePass();
			l_bReturn = Loader<Pass>::Load( * l_pass, p_file);
		}
	}

	return l_bReturn;
}

bool Loader<Material> :: Save( const Material & p_material, File & p_file)
{
	bool l_bReturn = p_file.Write( p_material.GetName());

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( p_material.GetNbPasses()) == sizeof( size_t));
	}

	if (l_bReturn)
	{
		for (size_t i = 0 ; i < p_material.GetNbPasses() && l_bReturn ; i++)
		{
			l_bReturn = Loader<Pass>::Save( * p_material.GetPass( i), p_file);
		}
	}

	return l_bReturn;
}

bool Loader<Material> :: Write( const Material & p_material, File & p_file)
{
	bool l_bReturn = p_file.WriteLine( "material " + p_material.GetName() + "\n") > 0;

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( "{\n") > 0;
	}

	size_t l_nbPasses = p_material.GetNbPasses();
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

		l_bReturn = Loader<Pass>::Write( * p_material.GetPass( i), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( "}\n") > 0;
	}

	return l_bReturn;
}

//*********************************************************************************************

Material :: Material( String const & p_name, int p_iNbInitialPasses)
	:	Resource<Material>( p_name)
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
	CASTOR_TRACK;
	for (size_t i = 0 ; i < m_passes.size() ; i++)
	{
		m_passes[i]->Initialise();
	}
}

void Material :: Cleanup()
{
	CASTOR_TRACK;
	for (size_t i = 0 ; i < m_passes.size() ; i++)
	{
		m_passes[i]->Cleanup();
	}

	m_passes.clear();
}

void Material :: Render( ePRIMITIVE_TYPE p_displayMode)
{
	CASTOR_TRACK;
	for (size_t i = 0 ; i < m_passes.size() ; i++)
	{
		m_passes[i]->Render( p_displayMode);
	}
}

void Material :: Render2D()
{
	CASTOR_TRACK;
	for (size_t i = 0 ; i < m_passes.size() ; i++)
	{
		m_passes[i]->Render2D();
	}
}

void Material :: EndRender()
{
	CASTOR_TRACK;
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
