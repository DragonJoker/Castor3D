#include "Dx9RenderSystem/PrecompiledHeader.h"

#include "Dx9RenderSystem/Dx9Pipeline.h"
#include "Dx9RenderSystem/Dx9ShaderProgram.h"
#include "Dx9RenderSystem/Dx9ShaderObject.h"
#include "Dx9RenderSystem/Dx9FrameVariable.h"
#include "Dx9RenderSystem/Dx9Context.h"
#include "Dx9RenderSystem/Dx9RenderSystem.h"
#include "Dx9RenderSystem/CgDx9ShaderProgram.h"
#include "Dx9RenderSystem/CgDx9FrameVariable.h"

using namespace Castor3D;

Dx9Pipeline::HandleConstantsMap Dx9Pipeline::sm_mapIdByProgram[eNbMatrixModes];
Dx9Pipeline::HandleConstantsMap Dx9Pipeline::sm_normalMatrixIDsByProgram;
Dx9Pipeline::HandleConstantsMap Dx9Pipeline::sm_pmvMatrixIDsByProgram;

Dx9Pipeline::ParameterProgramMap Dx9Pipeline::sm_mapIdByCgProgram[eNbMatrixModes];
Dx9Pipeline::ParameterProgramMap Dx9Pipeline::sm_normalMatrixIDsByCgProgram;
Dx9Pipeline::ParameterProgramMap Dx9Pipeline::sm_pmvMatrixIDsByCgProgram;

D3DVIEWPORT9 Dx9Pipeline::sm_viewport;

D3DTRANSFORMSTATETYPE g_matrixTypes[Pipeline::eNbMatrixModes] = { D3DTS_PROJECTION, D3DTS_VIEW, D3DTS_TEXTURE0, D3DTS_TEXTURE1, D3DTS_TEXTURE2, D3DTS_TEXTURE3 };
std::stack<D3DXMATRIX> g_matrix[Pipeline::eNbMatrixModes];
D3DXMATRIX g_matrixProjectionModelView;
D3DXMATRIX g_matrixNormal;
D3DXMATRIX g_matTransform;

Dx9Pipeline :: Dx9Pipeline()
{

	for (int i = 0 ; i < eNbMatrixModes ; i++)
	{
		D3DXMATRIX l_mtx(	1, 0, 0, 0,		0, 1, 0, 0,		0, 0, 1, 0,		0, 0, 0, 1);
		g_matrix[i].push( l_mtx);
	}
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

	if (l_pProgram != NULL && l_pProgram->GetType() == ShaderProgramBase::eCgShader)
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

bool Dx9Pipeline :: _matrixMode( eMATRIX_MODE p_eMode)
{
	return Pipeline::_matrixMode( p_eMode);
}

bool Dx9Pipeline :: _loadIdentity()
{
	g_matrix[sm_eCurrentMode].top() = D3DXMATRIX(	1, 0, 0, 0,		0, 1, 0, 0,		0, 0, 1, 0,		0, 0, 0, 1);
//	sm_matrix[sm_eCurrentMode].top().Identity();
	return true;
}

bool Dx9Pipeline :: _pushMatrix()
{
	g_matrix[sm_eCurrentMode].push( g_matrix[sm_eCurrentMode].top());
//	sm_matrix[sm_eCurrentMode].push( sm_matrix[sm_eCurrentMode].top());
	return true;
}

bool Dx9Pipeline :: _popMatrix()
{
	g_matrix[sm_eCurrentMode].pop();
//	sm_matrix[sm_eCurrentMode].pop();
//	CheckDxError( Dx9RenderSystem::GetDevice()->SetTransform( g_matrixTypes[sm_eCurrentMode], & g_matrix[sm_eCurrentMode].top()), CU_T( "Dx9Pipeline :: _popMatrix"), false);
	return true;
}

bool Dx9Pipeline :: _translate( const Point3r & p_translate)
{
	g_matrix[sm_eCurrentMode].top() = *D3DXMatrixTranslation( & g_matTransform, p_translate[0], p_translate[1], p_translate[2]) * g_matrix[sm_eCurrentMode].top();
//	MtxUtils::translate( sm_matrix[sm_eCurrentMode].top(), p_translate);
	return true;
}

bool Dx9Pipeline :: _rotate( const Quaternion & p_rotate)
{

	Point3r l_ptAxis;
	Angle l_angle;
	p_rotate.ToAxisAngle( l_ptAxis, l_angle);
	D3DXVECTOR3 l_vAxis( l_ptAxis.const_ptr());
	g_matrix[sm_eCurrentMode].top() = *D3DXMatrixRotationAxis( & g_matTransform, & l_vAxis, l_angle.Radians()) * g_matrix[sm_eCurrentMode].top();

//	MtxUtils::rotate( sm_matrix[sm_eCurrentMode].top(), p_rotate);
	return true;
}

bool Dx9Pipeline :: _scale( const Point3r & p_scale)
{
	g_matrix[sm_eCurrentMode].top() = *D3DXMatrixScaling( & g_matTransform, p_scale[0], p_scale[1], p_scale[2]) * g_matrix[sm_eCurrentMode].top();
//	MtxUtils::scale( sm_matrix[sm_eCurrentMode].top(), p_scale);
	return true;
}

bool Dx9Pipeline :: _multMatrixMtx( const Matrix4x4r & p_matrix)
{
	D3DXMatrixMultiply( & g_matrix[sm_eCurrentMode].top(), & D3DXMATRIX( p_matrix.const_ptr()), & g_matrix[sm_eCurrentMode].top());
//	sm_matrix[sm_eCurrentMode].top() = p_matrix * sm_matrix[sm_eCurrentMode].top();
	CheckDxError( Dx9RenderSystem::GetDevice()->SetTransform( g_matrixTypes[sm_eCurrentMode], & g_matrix[sm_eCurrentMode].top()), CU_T( "Dx9Pipeline :: _multMatrixMtx"), false);
	return true;
}

bool Dx9Pipeline :: _multMatrixPtr( const real * p_matrix)
{
	return Dx9Pipeline::_multMatrixMtx( p_matrix);
}

bool Dx9Pipeline :: _perspective( real p_rFOVY, real p_rRatio, real p_rNear, real p_rFar)
{
	D3DXMatrixPerspectiveFovLH( & g_matrix[eMatrixProjection].top(), p_rFOVY * Angle::DegreesToRadians, p_rRatio, p_rNear, p_rFar);
	CheckDxError( Dx9RenderSystem::GetDevice()->SetTransform( D3DTS_PROJECTION, & g_matrix[eMatrixProjection].top()), CU_T( "Dx9Pipeline :: _perspective"), false);
	return true;
}

bool Dx9Pipeline :: _frustum( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar)
{
	D3DXMatrixPerspectiveLH( & g_matrix[eMatrixProjection].top(), p_rRight - p_rLeft, p_rTop - p_rBottom, p_rNear, p_rFar);
	CheckDxError( Dx9RenderSystem::GetDevice()->SetTransform( D3DTS_PROJECTION, & g_matrix[eMatrixProjection].top()), CU_T( "Dx9Pipeline :: _perspective"), false);
//	MtxUtils::frustum( sm_matrix[eMatrixProjection].top(), p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
//	CheckDxError( Dx9RenderSystem::GetDevice()->SetTransform( D3DTS_PROJECTION, (D3DXMATRIX *)& MtxUtils::switch_hand( sm_matrix[eMatrixProjection].top())), CU_T( "Dx9Pipeline :: _perspective"), false);
	return true;
}

bool Dx9Pipeline :: _ortho( real p_rLeft, real p_rRight, real p_rBottom, real p_rTop, real p_rNear, real p_rFar)
{
	D3DXMatrixOrthoOffCenterLH( & g_matrix[eMatrixProjection].top(), p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
	CheckDxError( Dx9RenderSystem::GetDevice()->SetTransform( D3DTS_PROJECTION, & g_matrix[eMatrixProjection].top()), CU_T( "Dx9Pipeline :: _perspective"), false);
//	MtxUtils::ortho( sm_matrix[eMatrixProjection].top(), p_rLeft, p_rRight, p_rBottom, p_rTop, p_rNear, p_rFar);
//	CheckDxError( Dx9RenderSystem::GetDevice()->SetTransform( D3DTS_PROJECTION, (D3DXMATRIX *)& MtxUtils::switch_hand( sm_matrix[eMatrixProjection].top())), CU_T( "Dx9Pipeline :: _ortho"), false);
	return true;
}

bool Dx9Pipeline :: _project( const Point3r & p_ptObj, const Point4r & p_ptViewport, Point3r & p_ptResult)
{
	return Pipeline::_project( p_ptObj, p_ptViewport, p_ptResult);
}

bool Dx9Pipeline :: _unProject( const Point3i & p_ptWin, const Point4r & p_ptViewport, Point3r & p_ptResult)
{
	return Pipeline::_unProject( p_ptWin, p_ptViewport, p_ptResult);
}

void Dx9Pipeline :: _applyModelView()
{
	Dx9ShaderProgram * l_pProgram = Dx9RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram();

	if (l_pProgram->IsLinked())
	{
		_applyMatrix( eMatrixModelView, "ModelViewMatrix", l_pProgram);
	}
}

void Dx9Pipeline :: _applyProjection()
{
	Dx9ShaderProgram * l_pProgram = Dx9RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram();

	if (l_pProgram->IsLinked())
	{
		_applyMatrix( eMatrixProjection, "ProjectionMatrix", l_pProgram);
	}
}

void Dx9Pipeline :: _applyProjectionModelView()
{
	Dx9ShaderProgram * l_pProgram = Dx9RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram();

	if (l_pProgram->IsLinked())
	{
		g_matrixProjectionModelView = g_matrix[eMatrixProjection].top() * g_matrix[eMatrixModelView].top();

		ID3DXConstantTable * l_pConstants = static_pointer_cast<Dx9ShaderObject>( l_pProgram->GetVertexProgram())->GetInputConstants();
		HandleConstantsMap::iterator l_it = sm_pmvMatrixIDsByProgram.find( l_pConstants);

		if (l_it == sm_pmvMatrixIDsByProgram.end())
		{
			sm_pmvMatrixIDsByProgram[l_pConstants] = l_pConstants->GetConstantByName( NULL, "ProjectionModelViewMatrix");
			l_it = sm_pmvMatrixIDsByProgram.find( l_pConstants);
		}

		if (l_it->second != NULL)
		{
			CheckDxError( l_pConstants->SetMatrix( Dx9RenderSystem::GetDevice(), l_it->second, & g_matrixProjectionModelView), CU_T( "Dx9Pipeline :: _applyProjectionModelView - glUniformMatrix4fv"), false);
		}
	}
}

void Dx9Pipeline :: _applyModelNormal()
{
}

void Dx9Pipeline :: _applyViewport( int p_iWindowWidth, int p_iWindowHeight)
{
	sm_viewport.Width = p_iWindowWidth;
	sm_viewport.Height = p_iWindowHeight;
	CheckDxError( Dx9RenderSystem::GetDevice()->SetViewport( & sm_viewport), CU_T( "Dx9Pipeline :: _applyViewport"), false);
}

void Dx9Pipeline :: _applyMatrix( eMATRIX_MODE p_eMatrix, const char * p_szName, Dx9ShaderProgram * p_pProgram)
{
	ID3DXConstantTable * l_pConstants = static_pointer_cast<Dx9ShaderObject>( p_pProgram->GetVertexProgram())->GetInputConstants();
	HandleConstantsMap::iterator l_it = sm_mapIdByProgram[p_eMatrix].find( l_pConstants);

	if (l_it == sm_mapIdByProgram[p_eMatrix].end())
	{
		sm_mapIdByProgram[p_eMatrix][l_pConstants] = l_pConstants->GetConstantByName( NULL, p_szName);
		l_it = sm_mapIdByProgram[p_eMatrix].find( l_pConstants);
	}

	if (l_it->second != NULL)
	{
		CheckDxError( l_pConstants->SetMatrix( Dx9RenderSystem::GetDevice(), l_it->second, & g_matrix[p_eMatrix].top()), CU_T( "Dx9Pipeline :: _applyMatrix - glUniformMatrix4fv"), false);
	}
}

void Dx9Pipeline :: _cgApplyProjection()
{
	CgDx9ShaderProgram * l_pProgram = (CgDx9ShaderProgram *)( Dx9RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
		_cgApplyMatrix( eMatrixModelView, "ModelViewMatrix", l_pProgram);
	}
}

void Dx9Pipeline :: _cgApplyModelView()
{
	CgDx9ShaderProgram * l_pProgram = (CgDx9ShaderProgram *)( Dx9RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
		_cgApplyMatrix( eMatrixProjection, "ProjectionMatrix", l_pProgram);
	}
}

void Dx9Pipeline :: _cgApplyProjectionModelView()
{
	CgDx9ShaderProgram * l_pProgram = (CgDx9ShaderProgram *)( Dx9RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
		D3DXMatrixMultiply( & g_matrixProjectionModelView, & g_matrix[eMatrixProjection].top(), & g_matrix[eMatrixModelView].top());

		CGprogram l_cgProgram = l_pProgram->GetProgram( eVertexShader)->GetProgram();
		ParameterProgramMap::iterator l_it = sm_pmvMatrixIDsByCgProgram.find( l_cgProgram);

		if (l_it == sm_pmvMatrixIDsByCgProgram.end())
		{
			sm_pmvMatrixIDsByCgProgram[l_cgProgram] = cgGetNamedParameter( l_cgProgram, "ProjectionModelViewMatrix");
			CheckCgError( CU_T( "Dx9Pipeline :: _cgApplyProjectionModelView - cgGetNamedParameter"));
			l_it = sm_pmvMatrixIDsByCgProgram.find( l_cgProgram);
		}

		cgSetMatrixParameterfr( l_it->second, & g_matrixProjectionModelView._11);
		CheckCgError( CU_T( "Dx9Pipeline :: _cgApplyProjectionModelView - cgSetMatrixParameterfr"));
	}
}

void Dx9Pipeline :: _cgApplyModelNormal()
{
	CgDx9ShaderProgram * l_pProgram = (CgDx9ShaderProgram *)( Dx9RenderSystem::GetSingletonPtr()->GetCurrentShaderProgram());

	if (l_pProgram->IsLinked())
	{
//		sm_matrixNormal = sm_matrix[eMatrixModelView].top().GetMinor( 3, 3).GetInverse().GetTransposed();

		CGprogram l_cgProgram = l_pProgram->GetProgram( eVertexShader)->GetProgram();
		ParameterProgramMap::iterator l_it = sm_normalMatrixIDsByCgProgram.find( l_cgProgram);

		if (l_it == sm_normalMatrixIDsByCgProgram.end())
		{
			sm_normalMatrixIDsByCgProgram[l_cgProgram] = cgGetNamedParameter( l_cgProgram, "NormalMatrix");
			CheckCgError( CU_T( "Dx9Pipeline :: _cgApplyModelNormal - cgGetNamedParameter"));
			l_it = sm_normalMatrixIDsByCgProgram.find( l_cgProgram);
		}

		cgSetMatrixParameterfr( l_it->second, sm_matrixNormal.ptr());
		CheckCgError( CU_T( "Dx9Pipeline :: _cgApplyModelNormal - cgSetMatrixParameterfr"));
	}
}

void Dx9Pipeline :: _applyCurrentMatrix()
{
	CheckDxError( Dx9RenderSystem::GetDevice()->SetTransform( g_matrixTypes[sm_eCurrentMode], & g_matrix[sm_eCurrentMode].top()), CU_T( "Dx9Pipeline :: _applyCurrentMatrix"), false);
}

void Dx9Pipeline :: _cgApplyMatrix( eMATRIX_MODE p_eMatrix, const char * p_szName, CgDx9ShaderProgram * p_pProgram)
{
	CGprogram l_cgProgram = p_pProgram->GetProgram( eVertexShader)->GetProgram();
	ParameterProgramMap::iterator l_it = sm_mapIdByCgProgram[p_eMatrix].find( l_cgProgram);

	if (l_it == sm_mapIdByCgProgram[p_eMatrix].end())
	{
		sm_mapIdByCgProgram[p_eMatrix][l_cgProgram] = cgGetNamedParameter( l_cgProgram, p_szName);
		CheckCgError( CU_T( "Dx9Pipeline :: _cgApplyMatrix - cgGetNamedParameter"));
		l_it = sm_mapIdByCgProgram[p_eMatrix].find( l_cgProgram);
	}

	cgSetMatrixParameterfr( l_it->second, sm_matrix[p_eMatrix].top().ptr());
	CheckCgError( CU_T( "Dx9Pipeline :: _cgApplyMatrix - cgSetMatrixParameterfr"));
}