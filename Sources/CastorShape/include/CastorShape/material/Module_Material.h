//******************************************************************************
#ifndef ___MODULE_NEWMATERIAL___
#define ___MODULE_NEWMATERIAL___
//******************************************************************************
#include "../main/Module_GUI.h"
/*
#include <Castor3D/material/Module_Material.h>
#include <Castor3D/shader/Module_Shader.h>
*/
//******************************************************************************

class CSPassPanel;
class CSNewMaterialDialog;
class CSMaterialPanel;
class CSMaterialsFrame;
class CSMaterialsListView;
class CSEnvironmentFrame;

enum NewMaterialDialogIDs
{
	nmdName,
	nmdOK,
	nmdCancel,
	nmdDiffuse,
	nmdAmbient,
	nmdSpecular,
	nmdEmissive,
	nmdShininess,
	nmdTexture,
	nmdTextureButton,
};

enum MaterialsFrameIDs
{
	mlfNewMaterial,
	mlfNewMaterialName,
	mlfDeleteMaterial,
	mlfMaterialsList,
};

enum MaterialPanelIDs
{
	mlpDeletePass,
	mlpMaterialName,
	mlpPass
};

enum EnvironmentFrameIDs
{
	efOK,
	efCancel,
	efRGBSrc0,
	efRGBSrc1,
	efRGBSrc2,
	efRGBOperand0,
	efRGBOperand1,
	efRGBOperand2,
	efAlphaSrc0,
	efAlphaSrc1,
	efAlphaSrc2,
	efAlphaOperand0,
	efAlphaOperand1,
	efAlphaOperand2,
};

//******************************************************************************
#endif
//******************************************************************************