#include "PhongDivider.hpp"

#include <Engine.hpp>
#include <Logger.hpp>

using namespace Castor;
using namespace Phong;

C3D_Phong_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version();
}

C3D_Phong_API Castor3D::ePLUGIN_TYPE GetType()
{
	return Castor3D::ePLUGIN_TYPE_DIVIDER;
}

C3D_Phong_API String GetName()
{
	return cuT( "Phong Divider" );
}

C3D_Phong_API Castor3D::Subdivider * CreateDivider()
{
	Castor3D::Subdivider * l_return( new Subdivider() );
	return l_return;
}

C3D_Phong_API void DestroyDivider( Castor3D::Subdivider * p_pDivider )
{
	delete p_pDivider;
}

C3D_Phong_API String GetDividerType()
{
	return cuT( "phong" );
}

C3D_Phong_API void OnLoad( Castor3D::Engine * p_engine )
{
}

C3D_Phong_API void OnUnload( Castor3D::Engine * p_engine )
{
}
