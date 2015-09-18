#include "RenderEngine.hpp"

#pragma warning( push )
#pragma warning( disable:4996 )

using namespace Castor3D;
using namespace Castor;
using namespace CastorShape;

RenderEngine::RenderEngine( String const & p_strFileName, SceneSPtr p_pScene )
	:	m_strFileName( p_strFileName )
	,	m_pScene( p_pScene )
{
}

RenderEngine::~RenderEngine()
{
}

bool RenderEngine::Draw()
{
	bool l_return = false;
	std::ofstream l_imageFile( str_utils::to_str( m_strFileName ), std::ios_base::binary );

	if ( l_imageFile )
	{
		int l_iSizeX = 256;
		int l_iSizeY = 256;
		// Ajout du header TGA
		l_imageFile.put( 0 ).put( 0 );
		l_imageFile.put( 2 );       /* RGB non compresse */
		l_imageFile.put( 0 ).put( 0 );
		l_imageFile.put( 0 ).put( 0 );
		l_imageFile.put( 0 );
		l_imageFile.put( 0 ).put( 0 ); /* origine X */
		l_imageFile.put( 0 ).put( 0 ); /* origine Y */
		l_imageFile.put( char( l_iSizeX & 0x00FF ) ).put( char( ( l_iSizeX & 0xFF00 ) >> 8 ) );
		l_imageFile.put( char( l_iSizeY & 0x00FF ) ).put( char( ( l_iSizeY & 0xFF00 ) >> 8 ) );
		l_imageFile.put( 24	);       /* 24 bit bitmap */
		l_imageFile.put( 0	);
		// fin du header TGA
		uint8_t l_r, l_g, l_b;

		// balayage
		for ( int y = 0; y < l_iSizeY; ++y )
		{
			Logger::LogDebug( StringStream() << cuT( "y: " ) << y );

			for ( int x = 0; x < l_iSizeX; ++x )
			{
				Colour l_clrColour;
				real coef = 1.0f;
				int level = 0;
				// lancer de rayon
				Ray l_viewRay( Point3r( real( x - 128 ), real( y - 128 ), real( -10000.0 ) ), Point3r( 0, 0, 1 ) );
				SceneNodePtrStrMap::const_iterator l_itNodesEnd = m_pScene->NodesEnd();
				LightPtrIntMap::const_iterator l_itLightsEnd = m_pScene->LightsEnd();
				GeometrySPtr l_pNearestGeometry = nullptr;
				SubmeshSPtr l_pNearestSubmesh;

				do
				{
					// recherche de l'intersection la plus proche
					real l_fDistance = 20000.0f;
					FaceSPtr l_pFace;

					for ( SceneNodePtrStrMap::iterator l_it = m_pScene->NodesBegin(); l_it != l_itNodesEnd; ++l_it )
					{
						l_pNearestGeometry = l_it->second->GetNearestGeometry( & l_viewRay, l_fDistance, & l_pFace, & l_pNearestSubmesh );
					}

					if ( l_pNearestGeometry )
					{
						//Logger::LogDebug( cuT( "Geometry : ") + l_pNearestGeometry->GetName());
						Point3r l_vNewStart( l_viewRay.m_ptOrigin + ( l_viewRay.m_ptDirection * l_fDistance ) );
						// la normale au point d'intersection
						Point3r l_vNormal( l_vNewStart - l_pNearestSubmesh->GetSphere().GetCenter() );
						//Point3r l_vNormal( l_pFace->m_ptFaceNormal);
						real l_fTemp = point::dot( l_vNormal, l_vNormal );

						if ( l_fTemp == 0.0f )
						{
							break;
						}

						l_fTemp = real( 1.0 ) / std::sqrt( l_fTemp );
						l_vNormal *= l_fTemp;
						MaterialSPtr l_pCurrentMat = l_pNearestGeometry->GetMaterial( l_pNearestSubmesh );

						// calcul de la valeur d'éclairement au point
						for ( LightPtrIntMap::iterator l_it = m_pScene->LightsBegin(); l_it != l_itLightsEnd; ++l_it )
						{
							LightSPtr l_pCurrent = l_it->second;
							Point3r l_vDist( l_pCurrent->GetParent()->GetPosition() - l_vNewStart );

							if ( point::dot( l_vNormal, l_vDist ) > 0 )
							{
								real l_fT = real( point::distance( l_vDist ) );

								if ( l_fT > 0.0f )
								{
									Ray l_lightRay( l_vNewStart, l_vDist * ( 1 / l_fT ) );
									// calcul des ombres
									bool l_bInShadow = false;

									for ( SceneNodePtrStrMap::iterator l_it = m_pScene->NodesBegin(); ! l_bInShadow && l_it != l_itNodesEnd; ++l_it )
									{
										l_pNearestGeometry = l_it->second->GetNearestGeometry( & l_lightRay, l_fT, nullptr, nullptr );

										if ( l_pNearestGeometry )
										{
											l_bInShadow = true;
										}
									}

									if ( ! l_bInShadow )
									{
										// lambert
										float l_fLambert = float( point::dot( l_lightRay.m_ptDirection, l_vNormal ) * coef );
										l_clrColour.red()	+= l_pCurrent->GetDiffuse()[0] * l_pCurrentMat->GetPass( 0 )->GetDiffuse().red() * l_fLambert;
										l_clrColour.green()	+= l_pCurrent->GetDiffuse()[1] * l_pCurrentMat->GetPass( 0 )->GetDiffuse().green() * l_fLambert;
										l_clrColour.blue()	+= l_pCurrent->GetDiffuse()[2] * l_pCurrentMat->GetPass( 0 )->GetDiffuse().blue() * l_fLambert;
									}
								}
							}
						}

						// on itére sur la prochaine réflexion
						coef *= l_pCurrentMat->GetPass( 0 )->GetShininess();
						real l_fReflet = 2.0f * point::dot( l_viewRay.m_ptDirection, l_vNormal );
						l_viewRay.m_ptOrigin = l_vNewStart;
						l_viewRay.m_ptDirection = l_viewRay.m_ptDirection - ( l_vNormal * l_fReflet );
						level++;
					}
				}
				while ( ( coef > 0.0f ) && ( level < 10 ) && l_pNearestGeometry );

				l_imageFile.put( l_clrColour.blue().convert_to( l_b ) ).put( l_clrColour.green().convert_to( l_g ) ).put( l_clrColour.red().convert_to( l_r ) );
			}
		}

		l_return = true;
	}

	return l_return;
}

#pragma warning( pop )
