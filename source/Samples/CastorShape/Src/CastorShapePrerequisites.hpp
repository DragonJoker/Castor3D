/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CS_CASTOR_SHAPE_PREREQUISITES_H___
#define ___CS_CASTOR_SHAPE_PREREQUISITES_H___

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
