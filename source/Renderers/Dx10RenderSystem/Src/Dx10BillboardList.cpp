#include "Dx10BillboardList.hpp"
#include "Dx10RenderSystem.hpp"

using namespace Castor;
using namespace Castor3D;
using namespace Dx10Render;

//*************************************************************************************************

DxBillboardList::DxBillboardList( Castor3D::SceneRPtr p_pScene, DxRenderSystem * p_pRenderSystem )
	:	BillboardList( p_pScene, p_pRenderSystem )
{
}

DxBillboardList::~DxBillboardList()
{
}

bool DxBillboardList::DoInitialise()
{
	return true;
}

//*************************************************************************************************
