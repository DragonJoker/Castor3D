#ifndef ___CSGL_Matrix___
#define ___CSGL_Matrix___

#include <stack>

namespace Castor3D
{
	class C3D_API Pipeline : public MemoryTraced<Pipeline>
	{
	public:
		typedef enum
		{
			eModelView	=	0,
			eProjection	=	1,
			eTexture	=	2,
			eNbModes	=	3,
		}
		eMATRIX_MODE;

	protected:
		typedef void 	(__cdecl * VoidFunc)			();
		typedef bool 	(__cdecl * BoolFunc)			();
		typedef bool 	(__cdecl * MatrixModeFunc)		( eMATRIX_MODE p_eMode);
		typedef bool 	(__cdecl * TranslateFunc)		( const Point3r & p_translate);
		typedef bool 	(__cdecl * RotateFunc)			( const Quaternion & p_rotate);
		typedef bool 	(__cdecl * ScaleFunc)			( const Point3r & p_translate);
		typedef bool 	(__cdecl * MultMatrixMFunc)		( const Matrix4x4r & p_matrix);
		typedef bool 	(__cdecl * MultMatrixPFunc)		( const real * p_matrix);
		typedef bool 	(__cdecl * PerspectiveFunc)		( real p_rFOVY, real p_rRatio, real p_rNear, real p_rFar);
		typedef bool 	(__cdecl * OrthoFunc)			( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar);
		typedef bool	(__cdecl * ProjectFunc)			( const Point3r & p_ptObj, const Point4r & p_ptViewport, Point3r & p_ptResult);
		typedef bool	(__cdecl * UnProjectFunc)		( const Point3i & p_ptWin, const Point4r & p_ptViewport, Point3r & p_ptResult);
		typedef void 	(__cdecl * ApplyViewportFunc)	( int p_iWindowWidth, int p_iWindowHeight);

		typedef MatrixModeFunc		PMatrixModeFunc;
		typedef BoolFunc			PLoadIdentityFunc;
		typedef BoolFunc			PPushMatrixFunc;
		typedef BoolFunc			PPopMatrixFunc;
		typedef TranslateFunc		PTranslateFunc;
		typedef RotateFunc			PRotateFunc;
		typedef ScaleFunc			PScaleFunc;
		typedef MultMatrixMFunc		PMultMatrixMFunc;
		typedef MultMatrixPFunc		PMultMatrixPFunc;
		typedef PerspectiveFunc		PPerspectiveFunc;
		typedef OrthoFunc			POrthoFunc;
		typedef ProjectFunc			PProjectFunc;
		typedef UnProjectFunc		PUnProjectFunc;
		typedef VoidFunc			PApplyProjectionFunc;
		typedef VoidFunc			PApplyModelViewFunc;
		typedef VoidFunc			PApplyModelNormalFunc;
		typedef VoidFunc			PApplyProjectionModelViewFunc;
		typedef ApplyViewportFunc	PApplyViewportFunc;

	public:
		typedef Matrix4x4f matrix4x4;
		typedef Matrix3x3f matrix3x3;
		static matrix4x4 Identity;

	private:
		typedef std::stack<matrix4x4> MatrixStack;

	protected:
		static Pipeline * sm_singleton;
		static MatrixStack sm_matrix[eNbModes];
		static eMATRIX_MODE sm_eCurrentMode;
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
		static PProjectFunc						sm_pfnProject;
		static PUnProjectFunc					sm_pfnUnProject;
		static PApplyProjectionFunc				sm_pfnApplyProjection;
		static PApplyModelViewFunc				sm_pfnApplyModelView;
		static PApplyModelNormalFunc			sm_pfnApplyModelNormal;
		static PApplyProjectionModelViewFunc	sm_pfnApplyProjectionModelView;
		static PApplyViewportFunc				sm_pfnApplyViewport;

	protected:
		Pipeline();
		~Pipeline();

	public:
		static const matrix4x4 &	GetMatrix					( eMATRIX_MODE p_eMode);
		static bool 				MatrixMode					( eMATRIX_MODE p_eMode);
		static bool 				LoadIdentity				();
		static bool 				PushMatrix					();
		static bool 				PopMatrix					();
		static bool 				Translate					( const Point3r & p_translate);
		static bool 				Rotate						( const Quaternion & p_rotate);
		static bool 				Scale						( const Point3r & p_translate);
		static bool 				MultMatrix					( const Matrix4x4r & p_matrix);
		static bool 				MultMatrix					( const real * p_matrix);
		static bool 				Perspective					( real p_rFOVY, real p_rRatio, real p_rNear, real p_rFar);
		static bool 				Ortho						( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar);
		static bool 				Project						( const Point3r & p_ptObj, const Point4r & p_ptViewport, Point3r & p_ptResult);
		static bool 				UnProject					( const Point3i & p_ptWin, const Point4r & p_ptViewport, Point3r & p_ptResult);
		static void 				ApplyProjection				();
		static void 				ApplyProjectionModelView	();
		static void 				ApplyModelView				();
		static void 				ApplyModelNormal			();
		static void 				ApplyMatrices				();
		static void					ApplyViewport				( int p_iWindowWidth, int p_iWindowHeight);

	public:
		static bool 	_matrixMode					( eMATRIX_MODE p_eMode);
		static bool 	_loadIdentity				();
		static bool 	_pushMatrix					();
		static bool 	_popMatrix					();
		static bool 	_translate					( const Point3r & p_translate);
		static bool 	_rotate						( const Quaternion & p_rotate);
		static bool 	_scale						( const Point3r & p_translate);
		static bool 	_multMatrixMtx				( const Matrix4x4r & p_matrix);
		static bool 	_multMatrixPtr				( const real * p_matrix);
		static bool 	_perspective				( real p_rFOVY, real p_rRatio, real p_rNear, real p_rFar);
		static bool 	_ortho						( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar);
		static bool		_project					( const Point3r & p_ptObj, const Point4r & p_ptViewport, Point3r & p_ptResult);
		static bool		_unProject					( const Point3i & p_ptWin, const Point4r & p_ptViewport, Point3r & p_ptResult);
		static void 	_applyProjection			(){}
		static void 	_applyModelView				(){}
		static void 	_applyModelNormal			(){}
		static void 	_applyProjectionModelView	(){}
		static void 	_applyViewport				( int p_iWindowWidth, int p_iWindowHeight){}
	};
}

#endif