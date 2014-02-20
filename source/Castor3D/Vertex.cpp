#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Vertex.hpp"
#include "Castor3D/Submesh.hpp"

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

Vertex::TextLoader :: TextLoader( File::eENCODING_MODE p_eEncodingMode )
	:	Loader< Vertex, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
{
}

bool Vertex::TextLoader :: operator ()( Vertex & CU_PARAM_UNUSED( p_object ), TextFile & CU_PARAM_UNUSED( p_file ) )
{
	bool l_bReturn = false;
// 	bool l_bReturn = TextLoader< Point3r >()( p_object.GetNormal(), p_file );
//
// 	if( l_bReturn )
// 	{
// 		l_bReturn = TextLoader< Point3r >()( p_object.GetTangent(), p_file );
// 	}
//
// 	if( l_bReturn )
// 	{
// 		l_bReturn = TextLoader< Point2r >()( p_object.GetTexCoord(), p_file );
// 	}

	return l_bReturn;
}

bool Vertex::TextLoader :: operator ()( Vertex const & CU_PARAM_UNUSED( p_object ), TextFile & CU_PARAM_UNUSED( p_file ) )
{
	bool l_bReturn = false;
// 	bool l_bReturn = TextLoader< Point3r >()( p_object.GetNormal(), p_file );
//
// 	if( l_bReturn )
// 	{
// 		l_bReturn = TextLoader< Point3r >()( p_object.GetTangent(), p_file );
// 	}
//
// 	if( l_bReturn )
// 	{
// 		l_bReturn = TextLoader< Point2r >()( p_object.GetTexCoord(), p_file );
// 	}

	return l_bReturn;
}

//*************************************************************************************************

Vertex :: Vertex( BufferElementGroup & p_group )
	:	m_group	( p_group	)
{
}

Vertex :: Vertex( Vertex const & p_source )
	:	m_group	( p_source.m_group	)
{
}

Vertex & Vertex :: operator =( Vertex const & p_source )
{
	m_group	= p_source.m_group;
	return *this;
}

Vertex :: ~Vertex()
{
}

Coords3r Vertex :: GetPosition( void * p_pElement )
{
	return Coords3r( &reinterpret_cast< real * >( p_pElement )[sm_uiOffsetPos / sizeof( real )] );
}

Point3r Vertex :: GetPosition( void const * p_pElement )
{
	return Point3r( &reinterpret_cast< real const * >( p_pElement )[sm_uiOffsetPos / sizeof( real )] );
}

Coords3r Vertex :: GetNormal( void * p_pElement )
{
	return Coords3r( &reinterpret_cast< real * >( p_pElement )[sm_uiOffsetNml / sizeof( real )] );
}

Point3r Vertex :: GetNormal( void const * p_pElement )
{
	return Point3r( &reinterpret_cast< real const * >( p_pElement )[sm_uiOffsetNml / sizeof( real )] );
}

Coords3r Vertex :: GetTangent( void * p_pElement )
{
	return Coords3r( &reinterpret_cast< real * >( p_pElement )[sm_uiOffsetTan / sizeof( real )] );
}

Point3r Vertex :: GetTangent( void const * p_pElement )
{
	return Point3r( &reinterpret_cast< real const * >( p_pElement )[sm_uiOffsetTan / sizeof( real )] );
}

Coords3r Vertex :: GetBitangent( void * p_pElement )
{
	return Coords3r( &reinterpret_cast< real * >( p_pElement )[sm_uiOffsetBin / sizeof( real )] );
}

Point3r Vertex :: GetBitangent( void const * p_pElement )
{
	return Point3r( &reinterpret_cast< real const * >( p_pElement )[sm_uiOffsetBin / sizeof( real )] );
}

Coords3r Vertex :: GetTexCoord( void * p_pElement )
{
	return Coords3r( &reinterpret_cast< real * >( p_pElement )[sm_uiOffsetTex / sizeof( real )] );
}

Point3r Vertex :: GetTexCoord( void const * p_pElement )
{
	return Point3r( &reinterpret_cast< real const * >( p_pElement )[sm_uiOffsetTex / sizeof( real )] );
}

void Vertex :: SetPosition( void * p_pBuffer, real const * p_coord )
{
	std::memcpy( GetPosition( p_pBuffer ).ptr(), p_coord, sm_uiSizePos );
}

void Vertex :: SetPosition( void * p_pBuffer, real x, real y, real z )
{
	Coords3r l_pData = GetPosition( p_pBuffer );
	l_pData[0] = x;
	l_pData[1] = y;
	l_pData[2] = z;
}

void Vertex :: SetNormal( void * p_pBuffer, real const * p_coord )
{
	std::memcpy( GetNormal( p_pBuffer ).ptr(), p_coord, sm_uiSizeNml );
}

void Vertex :: SetNormal( void * p_pBuffer, real x, real y, real z )
{
	Coords3r l_pData = GetNormal( p_pBuffer );
	l_pData[0] = x;
	l_pData[1] = y;
	l_pData[2] = z;
}

void Vertex :: SetTangent( void * p_pBuffer, real const * p_coord )
{
	std::memcpy( GetTangent( p_pBuffer ).ptr(), p_coord, sm_uiSizeTan );
}

void Vertex :: SetTangent( void * p_pBuffer, real x, real y, real z )
{
	Coords3r l_pData = GetTangent( p_pBuffer );
	l_pData[0] = x;
	l_pData[1] = y;
	l_pData[2] = z;
}

void Vertex :: SetBitangent( void * p_pBuffer, real const * p_coord )
{
	std::memcpy( GetBitangent( p_pBuffer ).ptr(), p_coord, sm_uiSizeBin );
}

void Vertex :: SetBitangent( void * p_pBuffer, real x, real y, real z )
{
	Coords3r l_pData = GetBitangent( p_pBuffer );
	l_pData[0] = x;
	l_pData[1] = y;
	l_pData[2] = z;
}

void Vertex :: SetTexCoord( void * p_pBuffer, real const * p_coord )
{
	std::memcpy( GetTexCoord( p_pBuffer ).ptr(), p_coord, sm_uiSizeTex );
}

void Vertex :: SetTexCoord( void * p_pBuffer, real x, real y, real z )
{
	Coords3r l_pData = GetTexCoord( p_pBuffer );
	l_pData[0] = x;
	l_pData[1] = y;
	l_pData[2] = z;
}

//*************************************************************************************************

BonedVertex :: BonedVertex( BufferElementGroup & p_group )
	:	Vertex	( p_group	)
{
}

BonedVertex :: BonedVertex( BonedVertex const & p_source )
	:	Vertex	( p_source	)
{
}

BonedVertex :: BonedVertex( Vertex const & p_source )
	:	Vertex	( p_source	)
{
}

BonedVertex & BonedVertex :: operator =( BonedVertex const & p_source )
{
	Vertex::operator =( p_source );
	return *this;
}

BonedVertex & BonedVertex :: operator =( Vertex const & p_source )
{
	Vertex::operator =( p_source );
	return *this;
}

BonedVertex :: ~BonedVertex()
{
}

stVERTEX_BONE_DATA BonedVertex :: GetBones()const
{
	return *reinterpret_cast< stVERTEX_BONE_DATA const * >( &m_group.const_ptr()[sm_uiOffsetBon] );
}

void BonedVertex :: SetBones( stVERTEX_BONE_DATA * p_pData )
{
	std::memcpy( &m_group.ptr()[sm_uiOffsetBon], p_pData, sizeof( stVERTEX_BONE_DATA ) );
}

void BonedVertex :: SetBones( stVERTEX_BONE_DATA const & p_data )
{
	std::memcpy( &m_group.ptr()[sm_uiOffsetBon], &p_data, sizeof( stVERTEX_BONE_DATA ) );
}

stVERTEX_BONE_DATA BonedVertex :: GetBones( BufferElementGroupSPtr p_pGroup )
{
	return *reinterpret_cast< stVERTEX_BONE_DATA const * >( &p_pGroup->const_ptr()[sm_uiOffsetBon] );
}

stVERTEX_BONE_DATA BonedVertex :: GetBones( BufferElementGroup const & p_group )
{
	return *reinterpret_cast< stVERTEX_BONE_DATA const * >( &p_group.const_ptr()[sm_uiOffsetBon] );
}

void BonedVertex :: SetBones( BufferElementGroupSPtr p_pGroup, stVERTEX_BONE_DATA * p_pData )
{
	std::memcpy( &p_pGroup->ptr()[sm_uiOffsetBon], p_pData, sizeof( stVERTEX_BONE_DATA ) );
}

void BonedVertex :: SetBones( BufferElementGroupSPtr p_pGroup, stVERTEX_BONE_DATA const & p_data )
{
	std::memcpy( &p_pGroup->ptr()[sm_uiOffsetBon], &p_data, sizeof( stVERTEX_BONE_DATA ) );
}

void BonedVertex :: SetBones( BufferElementGroup & p_group, stVERTEX_BONE_DATA * p_pData )
{
	std::memcpy( &p_group.ptr()[sm_uiOffsetBon], p_pData, sizeof( stVERTEX_BONE_DATA ) );
}

void BonedVertex :: SetBones( BufferElementGroup & p_group, stVERTEX_BONE_DATA const & p_data )
{
	std::memcpy( &p_group.ptr()[sm_uiOffsetBon], &p_data, sizeof( stVERTEX_BONE_DATA ) );
}

//*************************************************************************************************