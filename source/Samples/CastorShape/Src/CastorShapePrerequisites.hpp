/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
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
