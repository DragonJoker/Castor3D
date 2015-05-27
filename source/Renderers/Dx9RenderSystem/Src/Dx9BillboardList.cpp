#include "Dx9BillboardList.hpp"
#include "Dx9RenderSystem.hpp"

using namespace Castor;
using namespace Castor3D;

namespace Dx9Render
{
	DxBillboardList::DxBillboardList( Castor3D::SceneSPtr p_pScene, DxRenderSystem * p_pRenderSystem )
		: BillboardList( p_pScene, p_pRenderSystem )
	{
	}

	DxBillboardList::~DxBillboardList()
	{
	}

	bool DxBillboardList::DoInitialise()
	{
		return true;
	}

	ShaderProgramBaseSPtr DxBillboardList::DoGetProgram( uint32_t p_flags )
	{
		return ShaderProgramBaseSPtr();
	}
}
