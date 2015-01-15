#include "LoopDivider.hpp"

#include <Engine.hpp>
#include <Logger.hpp>

C3D_Loop_API void GetRequiredVersion( Castor3D::Version & p_version )
{
	p_version = Castor3D::Version();
}

C3D_Loop_API Castor3D::ePLUGIN_TYPE GetType()
{
	return Castor3D::ePLUGIN_TYPE_DIVIDER;
}

C3D_Loop_API Castor::String GetName()
{
	return cuT( "Loop Divider" );
}

C3D_Loop_API Castor3D::Subdivider * CreateDivider()
{
	Castor3D::Subdivider * l_pReturn( new Loop::Subdivider() );
	return l_pReturn;
}

C3D_Loop_API void DestroyDivider( Castor3D::Subdivider * p_pDivider )
{
	delete p_pDivider;
}

C3D_Loop_API Castor::String GetDividerType()
{
	return cuT( "loop" );
}

C3D_Loop_API void OnLoad( Castor3D::Engine * p_engine )
{
	Castor::Logger::Initialise( p_engine->GetLoggerInstance() );
}

C3D_Loop_API void OnUnload( Castor3D::Engine * p_engine )
{
	Castor::Logger::Cleanup();
}
