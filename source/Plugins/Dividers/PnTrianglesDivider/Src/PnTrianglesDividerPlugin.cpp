#include "PnTrianglesDivider.hpp"

#include <Engine.hpp>
#include <Log/Logger.hpp>

using namespace Castor;
using namespace PnTriangles;

C3D_PnTriangles_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version();
}

C3D_PnTriangles_API Castor3D::ePLUGIN_TYPE GetType()
{
	return Castor3D::ePLUGIN_TYPE_DIVIDER;
}

C3D_PnTriangles_API String GetName()
{
	return cuT( "PN-Triangles Divider" );
}

C3D_PnTriangles_API Castor3D::Subdivider * CreateDivider()
{
	Castor3D::Subdivider * l_return( new Subdivider() );
	return l_return;
}

C3D_PnTriangles_API void DestroyDivider( Castor3D::Subdivider * p_pDivider )
{
	delete p_pDivider;
}

C3D_PnTriangles_API String GetDividerType()
{
	return cuT( "pn_tri" );
}

C3D_PnTriangles_API void OnLoad( Castor3D::Engine * p_engine )
{
}

C3D_PnTriangles_API void OnUnload( Castor3D::Engine * p_engine )
{
}
