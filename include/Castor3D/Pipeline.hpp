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
#ifndef ___C3D_Pipeline___
#define ___C3D_Pipeline___

#include "Prerequisites.hpp"

namespace Castor3D
{
	//! Implementation of the rendering pipeline
	/*!
	Defines the various matrices, applies the numerous transformation it can support and applies them
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class C3D_API Pipeline : public MemoryTraced<Pipeline>
	{
	public:
		typedef enum
		{	eMODE_PROJECTION
		,	eMODE_MODELVIEW
		,	eMODE_TEXTURE0
		,	eMODE_TEXTURE1
		,	eMODE_TEXTURE2
		,	eMODE_TEXTURE3
		,	eMODE_COUNT
		}	eMODE;

	protected:
		typedef void 	VoidFunc				();
		typedef bool 	BoolFunc				();
		typedef bool 	MatrixModeFunc			( eMODE p_eMode);
		typedef bool 	TranslateFunc			( Point3r const & p_translate);
		typedef bool 	RotateFunc				( Quaternion const & p_rotate);
		typedef bool 	ScaleFunc				( Point3r const & p_translate);
		typedef bool 	MultMatrixMFunc			( Matrix4x4r const & p_matrix);
		typedef bool 	MultMatrixPFunc			( real const * p_matrix);
		typedef bool 	PerspectiveFunc			( Angle const & p_aFOVY, real p_rRatio, real p_rNear, real p_rFar);
		typedef bool 	OrthoFunc				( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar);
		typedef bool	ProjectFunc				( Point3r const & p_ptObj, Point4r const & p_ptViewport, Point3r & p_ptResult);
		typedef bool	UnProjectFunc			( const Point3i & p_ptWin, Point4r const & p_ptViewport, Point3r & p_ptResult);
		typedef bool	PickMatrixFunc			( real x, real y, real width, real height, int viewport[4]);
		typedef void 	ApplyViewportFunc		( int p_iWindowWidth, int p_iWindowHeight);

		typedef MatrixModeFunc			*	PMatrixModeFunc;
		typedef BoolFunc				*	PLoadIdentityFunc;
		typedef BoolFunc				*	PPushMatrixFunc;
		typedef BoolFunc				*	PPopMatrixFunc;
		typedef TranslateFunc			*	PTranslateFunc;
		typedef RotateFunc				*	PRotateFunc;
		typedef ScaleFunc				*	PScaleFunc;
		typedef MultMatrixMFunc			*	PMultMatrixMFunc;
		typedef MultMatrixPFunc			*	PMultMatrixPFunc;
		typedef PerspectiveFunc			*	PPerspectiveFunc;
		typedef OrthoFunc				*	POrthoFunc;
		typedef OrthoFunc				*	PFrustumFunc;
		typedef ProjectFunc				*	PProjectFunc;
		typedef UnProjectFunc			*	PUnProjectFunc;
		typedef	PickMatrixFunc			*	PPickMatrixFunc;
		typedef VoidFunc				*	PApplyProjectionFunc;
		typedef VoidFunc				*	PApplyModelViewFunc;
		typedef VoidFunc				*	PApplyModelNormalFunc;
		typedef VoidFunc				*	PApplyProjectionModelViewFunc;
		typedef ApplyViewportFunc		*	PApplyViewportFunc;
		typedef VoidFunc				*	PApplyCurrentMatrixFunc;

	public:
		typedef Matrix4x4f matrix4x4;
		typedef Matrix3x3f matrix3x3;
		static matrix4x4 Identity;

	private:
		typedef std::stack<matrix4x4> MatrixStack;

	protected:
		static Pipeline * sm_singleton;
		static MatrixStack sm_matrix[eMODE_COUNT];
		static eMODE sm_eCurrentMode;
		static matrix3x3 sm_matrixNormal;
		static matrix4x4 sm_matrixProjectionModelView;

		static PMatrixModeFunc					sm_pfnMatrixMode;
		static PLoadIdentityFunc				sm_pfnLoadIdentity;
		static PPushMatrixFunc					sm_pfnPushMatrix;
		static PPopMatrixFunc					sm_pfnPopMatrix;
		static PTranslateFunc					sm_pfnTranslate;
		static PRotateFunc						sm_pfnRotate;
		static PScaleFunc						sm_pfnScale;
		static PMultMatrixMFunc					sm_pfnMultMatrixMtx;
		static PMultMatrixPFunc					sm_pfnMultMatrixPtr;
		static PPerspectiveFunc					sm_pfnPerspective;
		static POrthoFunc						sm_pfnOrtho;
		static PFrustumFunc						sm_pfnFrustum;
		static PProjectFunc						sm_pfnProject;
		static PUnProjectFunc					sm_pfnUnProject;
		static PPickMatrixFunc					sm_pfnPickMatrix;
		static PApplyProjectionFunc				sm_pfnApplyProjection;
		static PApplyModelViewFunc				sm_pfnApplyModelView;
		static PApplyModelNormalFunc			sm_pfnApplyModelNormal;
		static PApplyProjectionModelViewFunc	sm_pfnApplyProjectionModelView;
		static PApplyViewportFunc				sm_pfnApplyViewport;
		static PApplyCurrentMatrixFunc			sm_pfnApplyCurrentMatrix;

	protected:
		Pipeline();
		~Pipeline();

	public:
		static const matrix4x4 &	GetMatrix					( eMODE p_eMode);
		static bool 				MatrixMode					( eMODE p_eMode);
		static bool 				LoadIdentity				();
		static bool 				PushMatrix					();
		static bool 				PopMatrix					();
		static bool 				Translate					( Point3r const & p_translate);
		static bool 				Rotate						( Quaternion const & p_rotate);
		static bool 				Scale						( Point3r const & p_scale);
		static bool 				MultMatrix					( Matrix4x4r const & p_matrix);
		static bool 				MultMatrix					( real const * p_matrix);
		static bool 				Perspective					( Angle const & p_aFOVY, real p_rRatio, real p_rNear, real p_rFar);
		static bool 				Frustum						( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar);
		static bool 				Ortho						( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar);
		static bool 				Project						( Point3r const & p_ptObj, Point4r const & p_ptViewport, Point3r & p_ptResult);
		static bool 				UnProject					( const Point3i & p_ptWin, Point4r const & p_ptViewport, Point3r & p_ptResult);
		static bool 				PickMatrix					( real x, real y, real width, real height, int viewport[4]);
		static void 				ApplyProjection				();
		static void 				ApplyProjectionModelView	();
		static void 				ApplyModelView				();
		static void 				ApplyModelNormal			();
		static void 				ApplyMatrices				();
		static void					ApplyViewport				( int p_iWindowWidth, int p_iWindowHeight);
		static void					ApplyCurrentMatrix			();

	public:
		static bool 	_matrixMode					( eMODE p_eMode);
		static bool 	_loadIdentity				();
		static bool 	_pushMatrix					();
		static bool 	_popMatrix					();
		static bool 	_translate					( Point3r const & p_translate);
		static bool 	_rotate						( Quaternion const & p_rotate);
		static bool 	_scale						( Point3r const & p_scale);
		static bool 	_multMatrixMtx				( Matrix4x4r const & p_matrix);
		static bool 	_multMatrixPtr				( real const * p_matrix);
		static bool 	_perspective				( Angle const & p_aFOVY, real p_rRatio, real p_rNear, real p_rFar);
		static bool		_frustum					( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar);
		static bool 	_ortho						( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar);
		static bool		_project					( Point3r const & p_ptObj, Point4r const & p_ptViewport, Point3r & p_ptResult);
		static bool		_unProject					( const Point3i & p_ptWin, Point4r const & p_ptViewport, Point3r & p_ptResult);
		static bool		_pickMatrix					( real x, real y, real width, real height, int viewport[4]);
		static void 	_applyProjection			(){}
		static void 	_applyModelView				(){}
		static void 	_applyModelNormal			(){}
		static void 	_applyProjectionModelView	(){}
		static void 	_applyViewport				( int p_iWindowWidth, int p_iWindowHeight){}
		static void 	_applyCurrentMatrix			(){}
	};
}

#endif
