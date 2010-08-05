//******************************************************************************
#ifndef ___MODULE_GUI___
#define ___MODULE_GUI___
//******************************************************************************

class CSCastorShape;
class CSMainFrame;
class CSRenderPanel;

enum MainFrameMenuBarIDs
{
	mbExit,
	mbSaveScene,
	mLoadScene,
	mbNewGeometry,
	mbNewCone,
	mbNewCube,
	mbNewCylinder,
	mbNewIcosaedron,
	mbNewPlane,
	mbNewSphere,
	mbNewTorus,
	mbNewMaterial,
	mbGeometries,
	mbMaterials,
	mbSelectGeometries,
	mbSelectPoints,
	mbCloneSelection,
	mbSubdividePNTriangles,
	mbSubdivideLoop,
	mbSelectNone,
	mbSelect,
	mbModify,
	mbNone,
	mbRender
};

enum MainFrameIDs
{
	mainFrame,
	frame3d,
	frame2d,
	csTimer,
	mfNewGeometry,
	mfNewMaterial,
	mfGeometries,
	mfMaterials
};

typedef enum SelectionType
{
	stVertex,
	stFace,
	stSubmesh,
	stGeometry,
	stAll,
	stClone,
	stNone
} SelectionType;

typedef enum ActionType
{
	atSelect,
	atModify,
	atNone
} ActionType;

struct MaterialInfos
{
	float m_ambient[3];
	float m_emissive[3];
};

#ifndef _UNICODE
#	if C3D_UNICODE
#		error "CastorShape compiling in ASCII mode while Castor3D is in UNICODE mode"
#	endif
#else
#	if ! C3D_UNICODE
#		error "CastorShape compiling in UNICODE mode while Castor3D is in ASCII mode"
#	endif
#endif

//******************************************************************************
#endif
//******************************************************************************
