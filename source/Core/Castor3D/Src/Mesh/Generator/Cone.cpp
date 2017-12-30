#include "Cone.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Miscellaneous/Parameter.hpp"

using namespace castor;

namespace castor3d
{
	Cone::Cone()
		: MeshGenerator( cuT( "cone" ) )
		, m_nbFaces( 0 )
		, m_height( 0 )
		, m_radius( 0 )
	{
	}

	Cone::~Cone()
	{
	}

	MeshGeneratorSPtr Cone::create()
	{
		return std::make_shared< Cone >();
	}

	void Cone::doGenerate( Mesh & p_mesh, Parameters const & p_parameters )
	{
		String param;

		if ( p_parameters.get( cuT( "faces" ), param ) )
		{
			m_nbFaces = string::toUInt( param );
		}

		if ( p_parameters.get( cuT( "radius" ), param ) )
		{
			m_radius = string::toFloat( param );
		}

		if ( p_parameters.get( cuT( "height" ), param ) )
		{
			m_height = string::toFloat( param );
		}

		if ( m_nbFaces >= 2 && m_height > std::numeric_limits< real >::epsilon() && m_radius > std::numeric_limits< real >::epsilon() )
		{
			Submesh & submeshBase = *p_mesh.createSubmesh();
			Submesh & submeshSide = *p_mesh.createSubmesh();
			//CALCUL DE LA POSITION DES POINTS
			real angleRotation = real( Angle::PiMult2 / m_nbFaces );
			uint32_t i = 0;
			real rCos, rSin;

			for ( real dAlphaI = 0; i <= m_nbFaces; dAlphaI += angleRotation )
			{
				rCos = cos( dAlphaI );
				rSin = sin( dAlphaI );

				if ( i < m_nbFaces )
				{
					Vertex::setTexCoord( submeshBase.addPoint( m_radius * rCos, 0.0, m_radius * rSin ), ( 1 + rCos ) / 2, ( 1 + rSin ) / 2 );
				}

				Vertex::setTexCoord( submeshSide.addPoint( m_radius * rCos, 0.0, m_radius * rSin ), real( i ) / m_nbFaces, real( 1.0 ) );
				Vertex::setTexCoord( submeshSide.addPoint( real( 0 ), m_height, real( 0 ) ), real( i ) / m_nbFaces, real( 0.0 ) );
				i++;
			}

			BufferElementGroupSPtr ptBottomCenter	= submeshBase.addPoint( 0.0, 0.0, 0.0 );
			Vertex( *ptBottomCenter ).setTexCoord( 0.5, 0.5 );

			//RECONSTITION DES FACES
			if ( m_height < 0 )
			{
				m_height = -m_height;
			}

			auto indexMappingBase = std::make_shared< TriFaceMapping >( submeshBase );
			auto indexMappingSide = std::make_shared< TriFaceMapping >( submeshSide );

			//Composition des extràmitàs
			for ( i = 0; i < m_nbFaces - 1; i++ )
			{
				//Composition du bas
				indexMappingBase->addFace( submeshBase[i]->getIndex(), submeshBase[i + 1]->getIndex(), ptBottomCenter->getIndex() );
			}

			//Composition du bas
			indexMappingBase->addFace( submeshBase[m_nbFaces - 1]->getIndex(), submeshBase[0]->getIndex(), ptBottomCenter->getIndex() );

			//Composition des càtàs
			for ( i = 0; i < 2 * m_nbFaces; i += 2 )
			{
				indexMappingSide->addFace( submeshSide[i + 0]->getIndex(), submeshSide[i + 1]->getIndex(), submeshSide[i + 2]->getIndex() );
			}

			submeshBase.setIndexMapping( indexMappingBase );
			submeshSide.setIndexMapping( indexMappingSide );

			indexMappingBase->computeNormals( true );
			indexMappingSide->computeNormals( true );

			Coords3r ptNormal0Top;
			Coords3r ptNormal0Base;
			Coords3r ptTangent0Top;
			Coords3r ptTangent0Base;
			Coords3r ptNormal1Top;
			Coords3r ptNormal1Base;
			Coords3r ptTangent1Top;
			Coords3r ptTangent1Base;
			Vertex::getNormal( submeshSide[0], ptNormal0Top );
			Vertex::getNormal( submeshSide[1], ptNormal0Base );
			Vertex::getTangent( submeshSide[0], ptTangent0Top );
			Vertex::getTangent( submeshSide[1], ptTangent0Base );
			ptNormal0Top += Vertex::getNormal( submeshSide[submeshSide.getPointsCount() - 2], ptNormal1Top );
			ptNormal0Base += Vertex::getNormal( submeshSide[submeshSide.getPointsCount() - 1], ptNormal1Base );
			ptTangent0Top += Vertex::getTangent( submeshSide[submeshSide.getPointsCount() - 2], ptTangent1Top );
			ptTangent0Base += Vertex::getTangent( submeshSide[submeshSide.getPointsCount() - 1], ptTangent1Base );
			point::normalise( ptNormal0Top );
			point::normalise( ptNormal0Base );
			point::normalise( ptTangent0Top );
			point::normalise( ptTangent0Base );
			Vertex::getNormal( submeshSide[submeshSide.getPointsCount() - 2], ptNormal0Top );
			Vertex::getNormal( submeshSide[submeshSide.getPointsCount() - 1], ptNormal0Base );
			Vertex::getTangent( submeshSide[submeshSide.getPointsCount() - 2], ptTangent0Top );
			Vertex::getTangent( submeshSide[submeshSide.getPointsCount() - 1], ptTangent0Base );
		}
	}
}
