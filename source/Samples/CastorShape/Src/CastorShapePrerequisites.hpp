/* See LICENSE file in root folder */
#ifndef ___CS_CASTOR_SHAPE_PREREQUISITES_H___
#define ___CS_CASTOR_SHAPE_PREREQUISITES_H___

#include <GuiCommonPrerequisites.hpp>

namespace CastorShape
{
	typedef enum eBMP
	{
		eBMP_FICHIER = GuiCommon::eBMP_COUNT,
		eBMP_AJOUTER,
		eBMP_PARAMETRES,
		eBMP_SCENES,
		eBMP_MATERIALS,
		eBMP_PROPERTIES,
		eBMP_COUNT,
	}	eBMP;

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

	// Main classes
	class CastorShapeApp;
	class MainFrame;
	class RenderPanel;

	// Objects creation classes
	class NewGeometryDialog;
	class NewConeDialog;
	class NewCubeDialog;
	class NewCylinderDialog;
	class NewIcosahedronDialog;
	class NewPlaneDialog;
	class NewSphereDialog;
	class NewTorusDialog;

	// Materials creation classes
	class NewMaterialDialog;
}

#endif
