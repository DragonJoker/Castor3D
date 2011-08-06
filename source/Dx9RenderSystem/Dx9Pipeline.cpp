#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/Dx9Pipeline.hpp"
#include "Dx9RenderSystem/Dx9ShaderProgram.hpp"
#include "Dx9RenderSystem/Dx9ShaderObject.hpp"
#include "Dx9RenderSystem/Dx9FrameVariable.hpp"
#include "Dx9RenderSystem/Dx9Context.hpp"
#include "Dx9RenderSystem/Dx9RenderSystem.hpp"
#include "Dx9RenderSystem/CgDx9ShaderProgram.hpp"
#include "Dx9RenderSystem/CgDx9ShaderObject.hpp"
#include "Dx9RenderSystem/CgDx9FrameVariable.hpp"

#define USE_DX9_MATRIX 0
#define DEF_EXCLUSIVE 1

using namespace Castor3D;

void _printMatrix( float const * p_mtx)
{
	std::cout.precision( 5);
	for (int i = 0 ; i < 4 ; i++)
	{
		for (int j = 0 ; j < 4 ; j++)
		{
			std::cout << p_mtx[i * 4 + j] << " ";
		}

		std::cout << std::endl;
	}

	std::cout << std::endl;
}

Matrix4x4r Dx9Pipeline::NormalMatrix;

Dx9Pipeline::HandleConstantsMap Dx9Pipeline::sm_mapIdByProgram[eMODE_COUNT];
Dx9Pipeline::HandleConstantsMap Dx9Pipeline::sm_normalMatrixIDsByProgram;
Dx9Pipeline::HandleConstantsMap Dx9Pipeline::sm_pmvMatrixIDsByProgram;

Dx9Pipeline::ParameterProgramMap Dx9Pipeline::sm_mapIdByCgProgram[eMODE_COUNT];
Dx9Pipeline::ParameterProgramMap Dx9Pipeline::sm_normalMatrixIDsByCgProgram;
Dx9Pipeline::ParameterProgramMap Dx9Pipeline::sm_pmvMatrixIDsByCgProgram;

D3DVIEWPORT9 Dx9Pipeline::sm_viewport;

D3DTRANSFORMSTATETYPE g_matrixTypes[Pipeline::eMODE_COUNT] = { D3DTS_PROJECTION, D3DTS_VIEW, D3DTS_TEXTURE0, D3DTS_TEXTURE1, D3DTS_TEXTURE2, D3DTS_TEXTURE3 };
#if USE_DX9_MATRIX || ! DEF_EXCLUSIVE
std::stack<D3DXMATRIX> g_matrix[Pipeline::eMODE_COUNT];
D3DXMATRIX g_matrixProjectionModelView;
D3DXMATRIX g_matrixNormal;
D3DXMATRIX g_matTransform;
#endif

Dx9Pipeline :: Dx9Pipeline()
{
#if USE_DX9_MATRIX || ! DEF_EXCLUSIVE
	for (int i = 0 ; i < eMODE_COUNT ; i++)
	{
		D3DXMATRIX l_mtx(	1, 0, 0, 0,		0, 1, 0, 0,		0, 0, 1, 0,		0, 0, 0, 1);
		g_matrix[i].push( l_mtx);
	}
#endif
}

Dx9Pipeline :: ~Dx9Pipeline()
{
}

void Dx9Pipeline :: InitFunctions()
{
	ShaderProgramBase * l_pProgram = Dx9RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram();

	sm_pfnMatrixMode					= & Dx9Pipeline::_matrixMode;
	sm_pfnLoadIdentity					= & Dx9Pipeline::_loadIdentity;
	sm_pfnPushMatrix					= & Dx9Pipeline::_pushMatrix;
	sm_pfnPopMatrix						= & Dx9Pipeline::_popMatrix;
	sm_pfnTranslate						= & Dx9Pipeline::_translate;
	sm_pfnRotate						= & Dx9Pipeline::_rotate;
	sm_pfnScale							= & Dx9Pipeline::_scale;
	sm_pfnMultMatrixMtx					= & Dx9Pipeline::_multMatrixMtx;
	sm_pfnMultMatrixPtr					= & Dx9Pipeline::_multMatrixPtr;
	sm_pfnPerspective					= & Dx9Pipeline::_perspective;
	sm_pfnFrustum						= & Dx9Pipeline::_frustum;
	sm_pfnOrtho							= & Dx9Pipeline::_ortho;
	sm_pfnProject						= & Dx9Pipeline::_project;
	sm_pfnUnProject						= & Dx9Pipeline::_unProject;

	sm_pfnApplyViewport					= & Dx9Pipeline::_applyViewport;
	sm_pfnApplyCurrentMatrix			= & Dx9Pipeline::_applyCurrentMatrix;

	if (l_pProgram && l_pProgram->GetType() == ShaderProgramBase::eSHADER_LANGUAGE_CG)
	{
		sm_pfnApplyProjection			= & Dx9Pipeline::_cgApplyProjection;
		sm_pfnApplyModelView			= & Dx9Pipeline::_cgApplyModelView;
		sm_pfnApplyModelNormal			= & Dx9Pipeline::_cgApplyModelNormal;
		sm_pfnApplyProjectionModelView	= & Dx9Pipeline::_cgApplyProjectionModelView;
	}
	else
	{
		sm_pfnApplyProjection			= & Dx9Pipeline::_applyProjection;
		sm_pfnApplyModelView			= & Dx9Pipeline::_applyModelView;
		sm_pfnApplyModelNormal			= & Dx9Pipeline::_applyModelNormal;
		sm_pfnApplyProjectionModelView	= & Dx9Pipeline::_applyProjectionModelView;
	}
}

bool Dx9Pipeline :: _matrixMode( eMODE p_eMode)
{
	return Pipeline::_matrixMode( p_eMode);
}

bool Dx9Pipeline :: _loadIdentity()
{
#if USE_DX9_MATRIX || (! DEF_EXCLUSIVE)
	g_matrix[sm_eCurrentMode].top() = D3DXMATRIX(	1, 0, 0, 0,		0, 1, 0, 0,		0, 0, 1, 0,		0, 0, 0, 1);
#endif
#if (! USE_DX9_MATRIX) || (! DEF_EXCLUSIVE)
	sm_matrix[sm_eCurrentMode].top().SetIdentity();
#endif
	return true;
}

bool Dx9Pipeline :: _pushMatrix()
{
#if USE_DX9_MATRIX || (! DEF_EXCLUSIVE)
	g_matrix[sm_eCurrentMode].push( g_matrix[sm_eCurrentMode].top());
#endif
#if (! USE_DX9_MATRIX) || (! DEF_EXCLUSIVE)
	sm_matrix[sm_eCurrentMode].push( sm_matrix[sm_eCurrentMode].top());
#endif
	return true;
}

bool Dx9Pipeline :: _popMatrix()
{
#if USE_DX9_MATRIX || (! DEF_EXCLUSIVE)
	g_matrix[sm_eCurrentMode].pop();
#endif
#if (! USE_DX9_MATRIX) || (! DEF_EXCLUSIVE)
	sm_matrix[sm_eCurrentMode].pop();
#endif
#if USE_DX9_MATRIX
	CheckDxError( Dx9RenderSystem::GetDevice()->SetTransform( g_matrixTypes[sm_eCurrentMode], & g_matrix[sm_eCurrentMode].top()), cuT( "Dx9Pipeline :: _popMatrix"), false);
#else
	CheckDxError( Dx9RenderSystem::GetDevice()->SetTransform( g_matrixTypes[sm_eCurrentMode], & D3DXMATRIX( sm_matrix[sm_eCurrentMode].top().const_ptr())), cuT( "Dx9Pipeline :: _popMatrix"), false);
#endif
	return true;
}

bool Dx9Pipeline :: _translate( Point3r const & p_translate)
{
#if USE_DX9_MATRIX || (! DEF_EXCLUSIVE)
	g_matrix[sm_eCurrentMode].top() = *D3DXMatrixTranslation( & g_matTransform, p_translate[0], p_translate[1], p_translate[2]) * g_matrix[sm_eCurrentMode].top();
#endif
#if (! USE_DX9_MATRIX) || (! DEF_EXCLUSIVE)
	MtxUtils::translate( sm_matrix[sm_eCurrentMode].top(), p_translate);
#endif
	return true;
}

bool Dx9Pipeline :: _rotate( Quaternion const & p_rotate)
{
	Point3r l_ptAxis;
	Angle l_angle;
	p_rotate.ToAxisAngle( l_ptAxis, l_angle);
#if USE_DX9_MATRIX || (! DEF_EXCLUSIVE)
	D3DXVECTOR3 l_vAxis( l_ptAxis.const_ptr());
	g_matrix[sm_eCurrentMode].top() = *D3DXMatrixRotationAxis( & g_matTransform, & l_vAxis, l_angle.Radians()) * g_matrix[sm_eCurrentMode].top();
#endif
#if (! USE_DX9_MATRIX) || (! DEF_EXCLUSIVE)
	MtxUtils::rotate( sm_matrix[sm_eCurrentMode].top(), p_rotate);
#endif
	return true;
}

bool Dx9Pipeline :: _scale( Point3r const & p_scale)
{
#if USE_DX9_MATRIX || (! DEF_EXCLUSIVE)
	g_matrix[sm_eCurrentMode].top() = *D3DXMatrixScaling( & g_matTransform, p_scale[0], p_scale[1], p_scale[2]) * g_matrix[sm_eCurrentMode].top();
#endif
#if (! USE_DX9_MATRIX) || (! DEF_EXCLUSIVE)
	MtxUtils::scale( sm_matrix[sm_eCurrentMode].top(), p_scale);
#endif
	return true;
}

bool Dx9Pipeline :: _multMatrixMtx( Matrix4x4r const & p_matrix)
{
#if USE_DX9_MATRIX || (! DEF_EXCLUSIVE)
	D3DXMatrixMultiply( & g_matrix[sm_eCurrentMode].top(), & D3DXMATRIX( p_matrix.const_ptr()), & g_matrix[sm_eCurrentMode].top());
#endif
#if (! USE_DX9_MATRIX) || (! DEF_EXCLUSIVE)
	sm_matrix[sm_eCurrentMode].top() = p_matrix * sm_matrix[sm_eCurrentMode].top();
#endif
#if USE_DX9_MATRIX
	CheckDxError( Dx9RenderSystem::GetDevice()->SetTransform( g_matrixTypes[sm_eCurrentMode], & g_matrix[sm_eCurrentMode].top()), cuT( "Dx9Pipeline :: _multMatrixMtx"), false);
#else
	CheckDxError( Dx9RenderSystem::GetDevice()->SetTransform( g_matrixTypes[sm_eCurrentMode], & D3DXMATRIX( sm_matrix[sm_eCurrentMode].top().const_ptr())), cuT( "Dx9Pipeline :: _multMatrixMtx"), false);
#endif
/*	Ok	*
	std::cout << "MultMatrix\nDirect3D : " << std::endl;
	_printMatrix( g_matrix[sm_eCurrentMode].top());
	std::cout << "Internal : " << std::endl;
	_printMatrix( sm_matrix[sm_eCurrentMode].top().const_ptr());
/**/
	return true;
}

bool Dx9Pipeline :: _multMatrixPtr( real const * p_matrix)
{
	return Dx9Pipeline::_multMatrixMtx( p_matrix);
}

bool Dx9Pipeline :: _perspective( Angle const & p_aFOVY, real p_rRatio, real p_rNear, real p_rFar)
{
#if USE_DX9_MATRIX || (! DEF_EXCLUSIVE)
	D3DXMatrixPerspectiveFovLH( & g_matrix[eMODE_PROJECTION].top(), p_rFOVY * Angle::DegreesToRadians, p_rRatio, p_rNear, p_rFar);
#endif
#if (! USE_DX9_MATRIX) || (! DEF_EXCLUSIVE)
	MtxUtils::perspective( sm_matrix[eMODE_PROJECTION].top(), p_aFOVY, p_rRatio, p_rNear, p_rFar);
#endif
#if USE_DX9_MATRIX
	CheckDxError( Dx9RenderSystem::GetDevice()->SetTransform( D3DTS_PROJECTION, & g_matrix[eMODE_PROJECTION].top()), cuT( "Dx9Pipeline :: _perspective"), false);
#else
	CheckDxError( Dx9RenderSystem::GetDevice()->SetTransform( D3DTS_PROJECTION, & D3DXMATRIX( sm_matrix[eMODE_PROJECTION].top().const_ptr())), cuT( "Dx9Pipeline :: _perspective"), false);
#endif
/*	Ok	*
	std::cout << "Perspective\nDirect3D : " << std::endl;
	_printMatrix( & g_matrix[eMatrixProjection].top()._11);
	std::cout << "Internal : " << std::endl;
	_printMatrix( sm_matrix[eMatrixProjection].top().const_ptr());
/**/
	return true;
}

bool Dx9Pipeline :: _frustum( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar)
{
#if USE_DX9_MATRIX || (! DEF_EXCLUSIVE)
	D3DXMatrixPerspectiveLH( & g_matrix[eMODE_PROJECTION].top(), p_rRight - p_rLeft, p_rTop - p_rBottom, p_rNear, p_rFar);
#endif
#if (! USE_DX9_MATRIX) || (! DEF_EXCLUSIVE)
	MtxUtils::frustum( sm_matrix[eMODE_PROJECTION].top(), p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
#endif
#if USE_DX9_MATRIX
	CheckDxError( Dx9RenderSystem::GetDevice()->SetTransform( D3DTS_PROJECTION, & g_matrix[eMODE_PROJECTION].top()), cuT( "Dx9Pipeline :: _frustum"), false);
#else
	CheckDxError( Dx9RenderSystem::GetDevice()->SetTransform( D3DTS_PROJECTION, & D3DXMATRIX( sm_matrix[eMODE_PROJECTION].top().const_ptr())), cuT( "Dx9Pipeline :: _frustum"), false);
#endif
/*	Ok	*
	std::cout << "Frustum\nDirect3D : " << std::endl;
	_printMatrix( & g_matrix[eMatrixProjection].top()._11);
	std::cout << "Internal : " << std::endl;
	_printMatrix( sm_matrix[eMatrixProjection].top().const_ptr());
/**/
	return true;
}

bool Dx9Pipeline :: _ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar)
{
#if USE_DX9_MATRIX || (! DEF_EXCLUSIVE)
	D3DXMatrixOrthoOffCenterLH( & g_matrix[eMODE_PROJECTION].top(), p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
#endif
#if (! USE_DX9_MATRIX) || (! DEF_EXCLUSIVE)
	MtxUtils::ortho( sm_matrix[eMODE_PROJECTION].top(), p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
#endif
#if USE_DX9_MATRIX
	CheckDxError( Dx9RenderSystem::GetDevice()->SetTransform( D3DTS_PROJECTION, & g_matrix[eMODE_PROJECTION].top()), cuT( "Dx9Pipeline :: _perspective"), false);
#else
	CheckDxError( Dx9RenderSystem::GetDevice()->SetTransform( D3DTS_PROJECTION, & D3DXMATRIX( sm_matrix[eMODE_PROJECTION].top().const_ptr())), cuT( "Dx9Pipeline :: _ortho"), false);
#endif
/*	Ok	*
	std::cout << "Ortho\nDirect3D : " << std::endl;
	_printMatrix( & g_matrix[eMatrixProjection].top()._11);
	std::cout << "Internal : " << std::endl;
	_printMatrix( sm_matrix[eMatrixProjection].top().const_ptr());
/**/
	return true;
}

bool Dx9Pipeline :: _project( Point3r const & p_ptObj, Point4r const & p_ptViewport, Point3r & p_ptResult)
{
	return Pipeline::_project( p_ptObj, p_ptViewport, p_ptResult);
}

bool Dx9Pipeline :: _unProject( const Point3i & p_ptWin, Point4r const & p_ptViewport, Point3r & p_ptResult)
{
	return Pipeline::_unProject( p_ptWin, p_ptViewport, p_ptResult);
}

void Dx9Pipeline :: _applyModelView()
{
	Dx9ShaderProgram * l_pProgram = Dx9RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram();

	if (l_pProgram && l_pProgram->IsLinked())
	{
		_applyMatrix( eMODE_MODELVIEW, "ModelViewMatrix", l_pProgram);
	}
}

void Dx9Pipeline :: _applyProjection()
{
	Dx9ShaderProgram * l_pProgram = Dx9RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram();

	if (l_pProgram && l_pProgram->IsLinked())
	{
		_applyMatrix( eMODE_PROJECTION, "ProjectionMatrix", l_pProgram);
	}
}

void Dx9Pipeline :: _applyProjectionModelView()
{
	Dx9ShaderProgram * l_pProgram = Dx9RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram();

	if (l_pProgram && l_pProgram->IsLinked())
	{
#if USE_DX9_MATRIX || (! DEF_EXCLUSIVE)
		D3DXMatrixMultiply( & g_matrixProjectionModelView, & g_matrix[eMODE_PROJECTION].top(), & g_matrix[eMODE_MODELVIEW].top());
#endif
#if (! USE_DX9_MATRIX) || (! DEF_EXCLUSIVE)
		sm_matrixProjectionModelView = sm_matrix[eMODE_PROJECTION].top() * sm_matrix[eMODE_MODELVIEW].top();
#endif

		ID3DXConstantTable * l_pConstants = static_pointer_cast<Dx9ShaderObject>( l_pProgram->GetProgram( eSHADER_TYPE_VERTEX))->GetInputConstants();
		HandleConstantsMap::iterator l_it = sm_pmvMatrixIDsByProgram.find( l_pConstants);

		if (l_it == sm_pmvMatrixIDsByProgram.end())
		{
			sm_pmvMatrixIDsByProgram[l_pConstants] = l_pConstants->GetConstantByName( nullptr, "ProjectionModelViewMatrix");
			l_it = sm_pmvMatrixIDsByProgram.find( l_pConstants);
		}

		if (l_it->second)
		{
#if USE_DX9_MATRIX
			CheckDxError( l_pConstants->SetMatrix( Dx9RenderSystem::GetDevice(), l_it->second, & g_matrixProjectionModelView), cuT( "Dx9Pipeline :: _applyProjectionModelView - SetMatrix"), false);
#else
			CheckDxError( l_pConstants->SetMatrix( Dx9RenderSystem::GetDevice(), l_it->second, & D3DXMATRIX( sm_matrixProjectionModelView.const_ptr())), cuT( "Dx9Pipeline :: _applyProjectionModelView - SetMatrix"), false);
#endif
		}
	}
}

void Dx9Pipeline :: _applyModelNormal()
{
	Dx9ShaderProgram * l_pProgram = Dx9RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram();

	if (l_pProgram && l_pProgram->IsLinked())
	{
		NormalMatrix = sm_matrix[eMODE_MODELVIEW].top().GetInverse().GetTransposed();

		ID3DXConstantTable * l_pConstants = static_pointer_cast<Dx9ShaderObject>( l_pProgram->GetProgram( eSHADER_TYPE_VERTEX))->GetInputConstants();
		HandleConstantsMap::iterator l_it = sm_normalMatrixIDsByProgram.find( l_pConstants);

		if (l_it == sm_normalMatrixIDsByProgram.end())
		{
			sm_normalMatrixIDsByProgram[l_pConstants] = l_pConstants->GetConstantByName( nullptr, "NormalMatrix");
			l_it = sm_normalMatrixIDsByProgram.find( l_pConstants);
		}

		if (l_it->second)
		{
#if USE_DX9_MATRIX
			CheckDxError( l_pConstants->SetMatrix( Dx9RenderSystem::GetDevice(), l_it->second, & NormalMatrix), cuT( "Dx9Pipeline :: _applyModelNormal - SetMatrix"), false);
#else
			CheckDxError( l_pConstants->SetMatrix( Dx9RenderSystem::GetDevice(), l_it->second, & D3DXMATRIX( NormalMatrix.const_ptr())), cuT( "Dx9Pipeline :: _applyModelNormal - SetMatrix"), false);
#endif
		}
	}
}

void Dx9Pipeline :: _applyViewport( int p_iWindowWidth, int p_iWindowHeight)
{
	sm_viewport.Width = GetSystemMetrics(SM_CXFULLSCREEN);
	sm_viewport.Height = GetSystemMetrics(SM_CYFULLSCREEN);
	CheckDxError( Dx9RenderSystem::GetDevice()->SetViewport( & sm_viewport), cuT( "Dx9Pipeline :: _applyViewport"), false);
}

void Dx9Pipeline :: _applyMatrix( eMODE p_eMatrix, char const * p_szName, Dx9ShaderProgram * p_pProgram)
{
	ID3DXConstantTable * l_pConstants = static_pointer_cast<Dx9ShaderObject>( p_pProgram->GetProgram( eSHADER_TYPE_VERTEX))->GetInputConstants();
	HandleConstantsMap::iterator l_it = sm_mapIdByProgram[p_eMatrix].find( l_pConstants);

	if (l_it == sm_mapIdByProgram[p_eMatrix].end())
	{
		sm_mapIdByProgram[p_eMatrix][l_pConstants] = l_pConstants->GetConstantByName( nullptr, p_szName);
		l_it = sm_mapIdByProgram[p_eMatrix].find( l_pConstants);
	}

	if (l_it->second)
	{
#if USE_DX9_MATRIX
		CheckDxError( l_pConstants->SetMatrix( Dx9RenderSystem::GetDevice(), l_it->second, & g_matrix[p_eMatrix].top()), cuT( "Dx9Pipeline :: _applyMatrix - glUniformMatrix4fv"), false);
#else
		CheckDxError( l_pConstants->SetMatrix( Dx9RenderSystem::GetDevice(), l_it->second, & D3DXMATRIX( sm_matrix[p_eMatrix].top().const_ptr())), cuT( "Dx9Pipeline :: _applyMatrix - glUniformMatrix4fv"), false);
#endif
	}
}

void Dx9Pipeline :: _cgApplyProjection()
{
	CgDx9ShaderProgram * l_pProgram = (CgDx9ShaderProgram *)( Dx9RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
		_cgApplyMatrix( eMODE_MODELVIEW, "ModelViewMatrix", l_pProgram);
	}
}

void Dx9Pipeline :: _cgApplyModelView()
{
	CgDx9ShaderProgram * l_pProgram = (CgDx9ShaderProgram *)( Dx9RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
		_cgApplyMatrix( eMODE_PROJECTION, "ProjectionMatrix", l_pProgram);
	}
}

void Dx9Pipeline :: _cgApplyProjectionModelView()
{
	CgDx9ShaderProgram * l_pProgram = (CgDx9ShaderProgram *)( Dx9RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
#if USE_DX9_MATRIX || (! DEF_EXCLUSIVE)
		D3DXMatrixMultiply( & g_matrixProjectionModelView, & g_matrix[eMODE_PROJECTION].top(), & g_matrix[eMODE_MODELVIEW].top());
#endif
#if (! USE_DX9_MATRIX) || (! DEF_EXCLUSIVE)
		sm_matrixProjectionModelView = sm_matrix[eMODE_PROJECTION].top() * sm_matrix[eMODE_MODELVIEW].top();
#endif

		CGprogram l_cgProgram = static_pointer_cast<CgDx9ShaderObject>( l_pProgram->GetProgram( eSHADER_TYPE_VERTEX))->GetProgram();
		ParameterProgramMap::iterator l_it = sm_pmvMatrixIDsByCgProgram.find( l_cgProgram);

		if (l_it == sm_pmvMatrixIDsByCgProgram.end())
		{
			sm_pmvMatrixIDsByCgProgram[l_cgProgram] = cgGetNamedParameter( l_cgProgram, "ProjectionModelViewMatrix");
			CheckCgError( cuT( "Dx9Pipeline :: _cgApplyProjectionModelView - cgGetNamedParameter"));
			l_it = sm_pmvMatrixIDsByCgProgram.find( l_cgProgram);
		}

#if USE_DX9_MATRIX
		cgSetMatrixParameterfr( l_it->second, & g_matrixProjectionModelView._11);
#else
		cgSetMatrixParameterfr( l_it->second, sm_matrixProjectionModelView.const_ptr());
#endif
		CheckCgError( cuT( "Dx9Pipeline :: _cgApplyProjectionModelView - cgSetMatrixParameterfr"));
	}
}

void Dx9Pipeline :: _cgApplyModelNormal()
{
	CgDx9ShaderProgram * l_pProgram = (CgDx9ShaderProgram *)( Dx9RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
		sm_matrixNormal = sm_matrix[eMODE_MODELVIEW].top().GetMinor( 3, 3).GetInverse().GetTransposed();

		CGprogram l_cgProgram = static_pointer_cast<CgDx9ShaderObject>( l_pProgram->GetProgram( eSHADER_TYPE_VERTEX))->GetProgram();
		ParameterProgramMap::iterator l_it = sm_normalMatrixIDsByCgProgram.find( l_cgProgram);

		if (l_it == sm_normalMatrixIDsByCgProgram.end())
		{
			sm_normalMatrixIDsByCgProgram[l_cgProgram] = cgGetNamedParameter( l_cgProgram, "NormalMatrix");
			CheckCgError( cuT( "Dx9Pipeline :: _cgApplyModelNormal - cgGetNamedParameter"));
			l_it = sm_normalMatrixIDsByCgProgram.find( l_cgProgram);
		}

		cgSetMatrixParameterfr( l_it->second, sm_matrixNormal.ptr());
		CheckCgError( cuT( "Dx9Pipeline :: _cgApplyModelNormal - cgSetMatrixParameterfr"));
	}
}

void Dx9Pipeline :: _applyCurrentMatrix()
{
#if USE_DX9_MATRIX
	CheckDxError( Dx9RenderSystem::GetDevice()->SetTransform( g_matrixTypes[sm_eCurrentMode], & g_matrix[sm_eCurrentMode].top()), cuT( "Dx9Pipeline :: _applyCurrentMatrix"), false);
#else
	CheckDxError( Dx9RenderSystem::GetDevice()->SetTransform( g_matrixTypes[sm_eCurrentMode], & D3DXMATRIX( sm_matrix[sm_eCurrentMode].top().const_ptr())), cuT( "Dx9Pipeline :: _applyCurrentMatrix"), false);
#endif
}

void Dx9Pipeline :: _cgApplyMatrix( eMODE p_eMatrix, char const * p_szName, CgDx9ShaderProgram * p_pProgram)
{
	CGprogram l_cgProgram = static_pointer_cast<CgDx9ShaderObject>( p_pProgram->GetProgram( eSHADER_TYPE_VERTEX))->GetProgram();
	ParameterProgramMap::iterator l_it = sm_mapIdByCgProgram[p_eMatrix].find( l_cgProgram);

	if (l_it == sm_mapIdByCgProgram[p_eMatrix].end())
	{
		sm_mapIdByCgProgram[p_eMatrix][l_cgProgram] = cgGetNamedParameter( l_cgProgram, p_szName);
		CheckCgError( cuT( "Dx9Pipeline :: _cgApplyMatrix - cgGetNamedParameter"));
		l_it = sm_mapIdByCgProgram[p_eMatrix].find( l_cgProgram);
	}

#if USE_DX9_MATRIX
	cgSetMatrixParameterfr( l_it->second, & g_matrix[p_eMatrix].top()._11);
#else
	cgSetMatrixParameterfr( l_it->second, sm_matrix[p_eMatrix].top().ptr());
#endif
	CheckCgError( cuT( "Dx9Pipeline :: _cgApplyMatrix - cgSetMatrixParameterfr"));
}
