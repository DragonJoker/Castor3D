#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/BillboardList.hpp"
#include "Castor3D/Buffer.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/ShaderProgram.hpp"
#include "Castor3D/ShaderObject.hpp"
#include "Castor3D/Material.hpp"
#include "Castor3D/Pass.hpp"
#include "Castor3D/Pipeline.hpp"
#include "Castor3D/ShaderManager.hpp"

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

BillboardList :: BillboardList( Scene * p_pScene, RenderSystem * p_pRenderSystem )
	:	MovableObject	( p_pScene, eMOVABLE_TYPE_BILLBOARD	)
	,	m_pRenderSystem	( p_pRenderSystem					)
	,	m_bNeedUpdate	( false								)
{
	BufferElementDeclaration l_vertexDeclarationElements[] = { BufferElementDeclaration( 0, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_3FLOATS ) };	
	m_pDeclaration = std::make_shared< BufferDeclaration >( l_vertexDeclarationElements );
	
	ShaderProgramBaseSPtr l_pProgram = p_pRenderSystem->GetEngine()->GetShaderManager().GetNewProgram();

	l_pProgram->CreateFrameVariable( ShaderProgramBase::MapAmbient,		eSHADER_TYPE_PIXEL );
	l_pProgram->CreateFrameVariable( ShaderProgramBase::MapColour,		eSHADER_TYPE_PIXEL );
	l_pProgram->CreateFrameVariable( ShaderProgramBase::MapDiffuse,		eSHADER_TYPE_PIXEL );
	l_pProgram->CreateFrameVariable( ShaderProgramBase::MapNormal,		eSHADER_TYPE_PIXEL );
	l_pProgram->CreateFrameVariable( ShaderProgramBase::MapSpecular,	eSHADER_TYPE_PIXEL );
	l_pProgram->CreateFrameVariable( ShaderProgramBase::MapOpacity,		eSHADER_TYPE_PIXEL );
	l_pProgram->CreateFrameVariable( ShaderProgramBase::MapGloss,		eSHADER_TYPE_PIXEL );
	l_pProgram->CreateFrameVariable( ShaderProgramBase::MapHeight,		eSHADER_TYPE_PIXEL );

	m_wpProgram = l_pProgram;
	ShaderObjectBaseSPtr l_pObject = l_pProgram->CreateObject( eSHADER_TYPE_GEOMETRY );
	l_pObject->SetInputType( eTOPOLOGY_POINTS );
	l_pObject->SetOutputType( eTOPOLOGY_TRIANGLE_STRIPS );
	l_pObject->SetOutputVtxCount( 4 );
}

BillboardList :: ~BillboardList()
{
}

bool BillboardList :: Initialise()
{
	VertexBufferSPtr l_pVtxBuffer;

	l_pVtxBuffer	= std::make_shared< VertexBuffer	>( m_pRenderSystem, &(*m_pDeclaration)[0], m_pDeclaration->Size() );
	l_pVtxBuffer->SetShared( l_pVtxBuffer );
	
	uint32_t l_uiStride = m_pDeclaration->GetStride();
	l_pVtxBuffer->Resize( uint32_t( m_arrayPositions.size() * l_uiStride ) );
	uint8_t * l_pBuffer = l_pVtxBuffer->data();

	for( Point3rArrayConstIt l_it = m_arrayPositions.begin(); l_it != m_arrayPositions.end(); ++l_it )
	{
		std::memcpy( l_pBuffer, l_it->const_ptr(), l_uiStride );
		l_pBuffer += l_uiStride;
	}
	
	m_pGeometryBuffers = m_pRenderSystem->CreateGeometryBuffers( l_pVtxBuffer, nullptr, nullptr );
	
	MaterialSPtr l_pMaterial = m_wpMaterial.lock();
	ShaderProgramBaseSPtr l_pProgram = m_wpProgram.lock();
	bool l_bReturn = false;
	
	if( l_pProgram && l_pMaterial )
	{
		l_pMaterial->Cleanup();
	
		for( PassPtrArrayConstIt l_it = l_pMaterial->Begin(); l_it != l_pMaterial->End(); ++l_it )
		{
			(*l_it)->SetShader( l_pProgram );
		}

		l_bReturn = DoInitialise();
	}

	if( l_bReturn )
	{
		l_pMaterial->Initialise();
		m_pDimensionsUniform->SetValue( Point2i( m_dimensions.width(), m_dimensions.height() ) );

		l_pVtxBuffer->Create();
		l_pVtxBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW, l_pProgram );
		m_pGeometryBuffers->Initialise();

		m_bNeedUpdate = false;
	}

	return l_bReturn;
}

void BillboardList :: Cleanup()
{
	m_pDimensionsUniform.reset();
	
	ShaderProgramBaseSPtr l_pProgram = m_wpProgram.lock();
	l_pProgram->Cleanup();

	m_pGeometryBuffers->GetVertexBuffer()->Cleanup();
	m_pGeometryBuffers->Cleanup();
	m_pGeometryBuffers.reset();
}

void BillboardList :: RemovePoint( uint32_t p_uiIndex )
{
	if( p_uiIndex < m_arrayPositions.size() )
	{
		m_arrayPositions.erase( m_arrayPositions.begin() + p_uiIndex );
		m_bNeedUpdate = true;
	}
}

void BillboardList :: AddPoint( Castor::Point3r const & p_ptPosition )
{
	m_arrayPositions.push_back( p_ptPosition );
	m_bNeedUpdate = true;
}

void BillboardList :: AddPoints( Castor::Point3rArray const & p_ptPositions )
{
	m_arrayPositions.insert( m_arrayPositions.end(), p_ptPositions.begin(), p_ptPositions.end() );
	m_bNeedUpdate = true;
}

void BillboardList :: Render()
{
	if( !m_bNeedUpdate )
	{
		Pipeline * l_pPipeline = m_pRenderSystem->GetPipeline();
		ShaderProgramBaseSPtr l_pProgram = m_wpProgram.lock();
		MaterialSPtr l_pMaterial = m_wpMaterial.lock();
		VertexBufferSPtr l_pVtxBuffer = m_pGeometryBuffers->GetVertexBuffer();
		uint32_t l_uiSize = l_pVtxBuffer->GetSize() / l_pVtxBuffer->GetDeclaration().GetStride();
		
		if( l_pProgram && l_pMaterial )
		{
			l_pPipeline->ApplyMatrices( *l_pProgram );
			l_pProgram->Begin( 0, 1 );
		
			for( PassPtrArrayConstIt l_it = l_pMaterial->Begin(); l_it != l_pMaterial->End(); ++l_it )
			{
				(*l_it)->Render( 0, 1 );
				m_pGeometryBuffers->Draw( eTOPOLOGY_POINTS, l_pProgram, l_uiSize, 0 );
				(*l_it)->EndRender();
			}
		
			l_pProgram->End();
		
		}
	}
}

void BillboardList :: SetMaterial( MaterialSPtr p_pMaterial )
{
	m_wpMaterial = p_pMaterial;
}

void BillboardList :: SetDimensions( Size const & p_dimensions )
{
	m_dimensions = p_dimensions;

	if( m_pDimensionsUniform )
	{
		m_pDimensionsUniform->SetValue( Point2i( m_dimensions.width(), m_dimensions.height() ) );
	}
}

//*************************************************************************************************