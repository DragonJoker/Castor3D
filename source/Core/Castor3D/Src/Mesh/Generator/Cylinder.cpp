#include "Cylinder.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Miscellaneous/Parameter.hpp"

using namespace castor;

namespace castor3d
{
	Cylinder::Cylinder()
		: MeshGenerator( cuT( "cylinder" ) )
		, m_height( 0 )
		, m_radius( 0 )
		, m_nbFaces( 0 )
	{
	}

	Cylinder::~Cylinder()
	{
	}

	MeshGeneratorSPtr Cylinder::create()
	{
		return std::make_shared< Cylinder >();
	}

	void Cylinder::doGenerate( Mesh & p_mesh, Parameters const & p_parameters )
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

		if ( m_nbFaces >= 2 )
		{
			Submesh & submeshBase	= *p_mesh.createSubmesh();
			Submesh & submeshTop	= *p_mesh.createSubmesh();
			Submesh & submeshSide	= *p_mesh.createSubmesh();
			//CALCUL DE LA POSITION DES POINTS
			real angleRotation = real( Angle::PiMult2 / m_nbFaces );
			uint32_t i = 0;
			real rCosRot = cos( angleRotation );
			real rSinRot = sin( angleRotation );
			BufferElementGroupSPtr vertex;
			real rCos = real( 1 );
			real rSin = real( 0 );
			real rSinT = real( 0 );

			for ( uint32_t i = 0; i <= m_nbFaces; ++i )
			{
				if ( i < m_nbFaces )
				{
					vertex = submeshBase.addPoint(	m_radius * rCos,	-m_height / 2,	m_radius * rSin );
					Vertex::setNormal( vertex, 0.0, -1.0, 0.0 );
					Vertex::setTexCoord( vertex,	( 1 + rCos ) / 2,	( 1 + rSin ) / 2 );
					vertex = submeshTop.addPoint(	m_radius * rCos,	 m_height / 2,	m_radius * rSinT );
					Vertex::setNormal( vertex, 0.0, 1.0, 0.0 );
					Vertex::setTexCoord( vertex,	( 1 + rCos ) / 2,	( 1 + rSinT ) / 2 );
				}

				vertex = submeshSide.addPoint(	m_radius * rCos,	-m_height / 2,	m_radius * rSin );
				Vertex::setTexCoord( vertex,	real( 1.0 ) - real( i ) / m_nbFaces,	real( 0.0 ) );
				Vertex::setNormal( vertex, rCos, rSin, 0.0 );
				vertex = submeshSide.addPoint(	m_radius * rCos,	 m_height / 2,	m_radius * rSin );
				Vertex::setNormal( vertex, rCos, rSin, 0.0 );
				Vertex::setTexCoord( vertex,	real( 1.0 ) - real( i ) / m_nbFaces,	real( 1.0 ) );

				const real newCos = rCosRot * rCos - rSinRot * rSin;
				const real newSin = rSinRot * rCos + rCosRot * rSin;
				rCos = newCos;
				rSin = newSin;
				rSinT = -newSin;
			}

			FaceSPtr pFace;
			BufferElementGroupSPtr ptTopCenter = submeshTop.addPoint( 0.0, m_height / 2, 0.0 );
			BufferElementGroupSPtr ptBottomCenter = submeshBase.addPoint( 0.0, -m_height / 2, 0.0 );
			Vertex::setTexCoord( ptTopCenter, 0.5, 0.5 );
			Vertex::setTexCoord( ptBottomCenter, 0.5, 0.5 );

			//RECONSTITION DES FACES
			if ( m_height < 0 )
			{
				m_height = -m_height;
			}

			//Composition des extràmitàs
			for ( i = 0; i < m_nbFaces - 1; i++ )
			{
				//Composition du bas
				submeshBase.addFace( submeshBase[i]->getIndex(), submeshBase[i + 1]->getIndex(), ptBottomCenter->getIndex() );
				//Composition du dessus
				submeshTop.addFace( ptTopCenter->getIndex(), submeshTop[i]->getIndex(), submeshTop[i + 1]->getIndex() );
			}

			//Composition du bas
			submeshBase.addFace( submeshBase[m_nbFaces - 1]->getIndex(), submeshBase[0]->getIndex(), ptBottomCenter->getIndex() );
			//Composition du dessus
			submeshTop.addFace( ptTopCenter->getIndex(), submeshTop[m_nbFaces - 1]->getIndex(), submeshTop[0]->getIndex() );

			//Composition des càtàs
			for ( i = 0; i < 2 * m_nbFaces; i += 2 )
			{
				submeshSide.addFace( submeshSide[i + 0]->getIndex(), submeshSide[i + 1]->getIndex(), submeshSide[i + 2]->getIndex() );
				submeshSide.addFace( submeshSide[i + 1]->getIndex(), submeshSide[i + 3]->getIndex(), submeshSide[i + 2]->getIndex() );
			}

			submeshBase.computeTangentsFromNormals();
			submeshTop.computeTangentsFromNormals();
			submeshSide.computeTangentsFromNormals();
			//computeNormals( p_mesh );
			//Coords3r ptNormal0Top;
			//Coords3r ptNormal0Base;
			//Coords3r ptTangent0Top;
			//Coords3r ptTangent0Base;
			//Coords3r ptNormal1Top;
			//Coords3r ptNormal1Base;
			//Coords3r ptTangent1Top;
			//Coords3r ptTangent1Base;
			//Vertex::getNormal( submeshSide[0], ptNormal0Top );
			//Vertex::getNormal( submeshSide[1], ptNormal0Base );
			//Vertex::getTangent( submeshSide[0], ptTangent0Top );
			//Vertex::getTangent( submeshSide[1], ptTangent0Base );
			//l_ptNormal0Top += Vertex::getNormal( submeshSide[submeshSide.getPointsCount() - 2], ptNormal1Top );
			//l_ptNormal0Base += Vertex::getNormal( submeshSide[submeshSide.getPointsCount() - 1], ptNormal1Base );
			//l_ptTangent0Top += Vertex::getTangent( submeshSide[submeshSide.getPointsCount() - 2], ptTangent1Top );
			//l_ptTangent0Base += Vertex::getTangent( submeshSide[submeshSide.getPointsCount() - 1], ptTangent1Base );
			//point::normalise( ptNormal0Top );
			//point::normalise( ptNormal0Base );
			//point::normalise( ptTangent0Top );
			//point::normalise( ptTangent0Base );
			//Vertex::getNormal( submeshSide[submeshSide.getPointsCount() - 2], ptNormal0Top );
			//Vertex::getNormal( submeshSide[submeshSide.getPointsCount() - 1], ptNormal0Base );
			//Vertex::getTangent( submeshSide[submeshSide.getPointsCount() - 2], ptTangent0Top );
			//Vertex::getTangent( submeshSide[submeshSide.getPointsCount() - 1], ptTangent0Base );
		}

		p_mesh.computeContainers();
	}
}
