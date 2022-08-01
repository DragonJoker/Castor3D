/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TriFaceMapping_H___
#define ___C3D_TriFaceMapping_H___

#include "Castor3D/Model/Mesh/Submesh/Component/IndexMapping.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/FaceIndices.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/FaceInfos.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/Face.hpp"

namespace castor3d
{
	class TriFaceMapping
		: public IndexMapping
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	submesh				The parent submesh.
		 *\param[in]	bufferUsageFlags	The buffer usage flags.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	submesh				Le sous-maillage parent.
		 *\param[in]	bufferUsageFlags	Les flags d'utilisation du buffer.
		 */
		C3D_API explicit TriFaceMapping( Submesh & submesh
			, VkBufferUsageFlags bufferUsageFlags = {} );
		/**
		 *\~english
		 *\brief		Clears this submesh's face array.
		 *\~french
		 *\brief		Vide le tableau de faces.
		 */
		C3D_API void clearFaces();
		/**
		 *\~english
		 *\brief		Creates and adds a face to the submesh.
		 *\param[in]	a	The first face's vertex index.
		 *\param[in]	b	The second face's vertex index.
		 *\param[in]	c	The third face's vertex index.
		 *\return		The created face.
		 *\~french
		 *\brief		Crée et ajoute une face au sous-maillage.
		 *\param[in]	a	L'index du premier vertex.
		 *\param[in]	b	L'index du second vertex.
		 *\param[in]	c	L'index du troisième vertex.
		 *\return		La face créée.
		 */
		C3D_API Face addFace( uint32_t a, uint32_t b, uint32_t c );
		/**
		 *\~english
		 *\brief		Creates and adds faces to the submesh.
		 *\param[in]	begin	The faces data begin.
		 *\param[in]	end		The faces data end.
		 *\~french
		 *\brief		Crée et ajoute des faces au sous-maillage.
		 *\param[in]	begin	Le début des données de faces.
		 *\param[in]	end		La fin des données de faces.
		 */
		C3D_API void addFaceGroup( FaceIndices const * const begin
			, FaceIndices const * const end );
		/**
		 *\~english
		 *\brief		Creates and adds a quad face to the submesh.
		 *\param[in]	a		The first face's vertex index.
		 *\param[in]	b		The second face's vertex index.
		 *\param[in]	c		The third face's vertex index.
		 *\param[in]	d		The fourth face's vertex index.
		 *\param[in]	minUV	The UV of the bottom left corner.
		 *\param[in]	maxUV	The UV of the top right corner.
		 *\~french
		 *\brief		Crée et ajoute une face à 4 côtés au sous-maillage.
		 *\param[in]	a		L'index du premier vertex.
		 *\param[in]	b		L'index du second vertex.
		 *\param[in]	c		L'index du troisième vertex.
		 *\param[in]	d		L'index du quatrième vertex.
		 *\param[in]	minUV	L'UV du coin bas gauche.
		 *\param[in]	maxUV	L'UV du coin haut droit.
		 */
		C3D_API void addQuadFace( uint32_t a, uint32_t b, uint32_t c, uint32_t d
			, castor::Point3f const & minUV = castor::Point3f()
			, castor::Point3f const & maxUV = castor::Point3f( 1, 1, 1 ) );
		/**
		 *\~english
		 *\brief		Creates faces from the points.
		 *\remarks		This function assumes the points are sorted like triangles fan.
		 *\~french
		 *\brief		Crée les faces à partir des points.
		 *\remarks		Cette fonction suppose que les points sont tirés à la manière triangles fan.
		 */
		C3D_API void computeFacesFromPolygonVertex();
		/**
		 *\~english
		 *\brief		Computes normal and tangent for each vertex of the given face.
		 *\param[in]	positions	The vertices positions.
		 *\param[in]	texcoords	The vertices texture coordinates.
		 *\param[out]	normals		The vertices normals.
		 *\param[out]	tangents	The vertices tangents.
		 *\param[in]	reverted	\p true to invert normals.
		 *\~french
		 *\brief		Calcule la normale et la tangente pour chaque vertex de la face donnée.
		 *\param[in]	positions	Les positions des sommets.
		 *\param[in]	texcoords	Les coordonnées de texture des sommets.
		 *\param[out]	normals		Les normales des sommets.
		 *\param[out]	tangents	Les tangentes des sommets.
		 *\param[in]	reverted	\p true pour inverser les normales.
		 */
		C3D_API void computeNormals( castor::Point3fArray const & positions
			, castor::Point3fArray const & texcoords
			, castor::Point3fArray & normals
			, castor::Point3fArray & tangents
			, bool reverted = false )const;
		/**
		 *\~english
		 *\brief		Computes normal and tangent for each vertex of the given face.
		 *\param[in]	face	The face.
		 *\~french
		 *\brief		Calcule la normale et la tangente pour chaque vertex de la face donnée.
		 *\param[in]	face	La face.
		 */
		C3D_API void computeNormals( Face const & face );
		/**
		 *\~english
		 *\brief		Computes normal and tangent for each vertex of the given face.
		 *\param[in]	positions	The vertices positions.
		 *\param[in]	texcoords	The vertices texture coordinates.
		 *\param[out]	normals		The vertices normals.
		 *\param[out]	tangents	The vertices tangents.
		 *\param[in]	face		The face.
		 *\~french
		 *\brief		Calcule la normale et la tangente pour chaque vertex de la face donnée.
		 *\param[in]	positions	Les positions des sommets.
		 *\param[in]	texcoords	Les coordonnées de texture des sommets.
		 *\param[out]	normals		Les normales des sommets.
		 *\param[out]	tangents	Les tangentes des sommets.
		 *\param[in]	face		La face.
		 */
		C3D_API void computeNormals( castor::Point3fArray const & positions
			, castor::Point3fArray const & texcoords
			, castor::Point3fArray & normals
			, castor::Point3fArray & tangents
			, Face const & face )const;
		/**
		 *\~english
		 *\brief		Computes tangent for each vertex of the given face.
		 *\param[in]	face	The face.
		 *\~french
		 *\brief		Calcule la tangente pour chaque vertex de la face donnée.
		 *\param[in]	face	La face.
		 */
		C3D_API void computeTangents( Face const & face );
		/**
		 *\~english
		 *\brief		Computes tangent for each vertex of the given face.
		 *\param[in]	positions	The vertices positions.
		 *\param[in]	texcoords	The vertices texture coordinates.
		 *\param[out]	tangents	The vertices tangents.
		 *\param[in]	face		The face.
		 *\~french
		 *\brief		Calcule la tangente pour chaque vertex de la face donnée.
		 *\param[in]	positions	Les positions des sommets.
		 *\param[in]	texcoords	Les coordonnées de texture des sommets.
		 *\param[out]	tangents	Les tangentes des sommets.
		 *\param[in]	face		La face.
		 */
		C3D_API void computeTangents( castor::Point3fArray const & positions
			, castor::Point3fArray const & texcoords
			, castor::Point3fArray & tangents
			, Face const & face )const;
		/**
		 *\~english
		 *\brief		Computes tangent for each vertex of the submesh.
		 *\remarks		This function supposes the normals are defined.
		 *\param[in]	positions	The vertices positions.
		 *\param[in]	texcoords	The vertices texture coordinates.
		 *\param[in]	normals		The vertices normals.
		 *\param[out]	tangents	The vertices tangents.
		 *\~french
		 *\brief		Calcule la tangente pour chaque vertex du sous-maillage.
		 *\remarks		Cette fonction suppose que les normales sont définies.
		 *\param[in]	positions	Les positions des sommets.
		 *\param[in]	texcoords	Les coordonnées de texture des sommets.
		 *\param[in]	normals		Les normales des sommets.
		 *\param[out]	tangents	Les tangentes des sommets.
		 */
		C3D_API void computeTangentsFromNormals( castor::Point3fArray const & positions
			, castor::Point3fArray const & texcoords
			, castor::Point3fArray const & normals
			, castor::Point3fArray & tangents )const;
		/**
		 *\copydoc		castor3d::IndexMapping::getCount
		 */
		C3D_API uint32_t getCount()const override;
		/**
		 *\copydoc		castor3d::IndexMapping::getComponentsCount
		 */
		C3D_API uint32_t getComponentsCount()const override;
		/**
		 *\copydoc		castor3d::IndexMapping::computeNormals
		 */
		C3D_API void computeNormals( bool reverted = false )override;
		/**
		 *\copydoc		castor3d::IndexMapping::computeTangents
		 */
		C3D_API void computeTangents()override;
		/**
		 *\copydoc		castor3d::SubmeshComponent::clone
		 */
		C3D_API SubmeshComponentSPtr clone( Submesh & submesh )const override;
		/**
		 *\~english
		 *\brief		Creates and adds faces to the submesh.
		 *\param[in]	faces	The faces.
		 *\~french
		 *\brief		Crée et ajoute une face au sous-maillage.
		 *\param[in]	faces	Les faces.
		 */
		void addFaceGroup( std::vector< FaceIndices > const & faces )
		{
			addFaceGroup( faces.data(), faces.data() + faces.size() );
		}
		/**
		 *\~english
		 *\brief		Creates and adds faces to the submesh.
		 *\param[in]	faces	The faces.
		 *\~french
		 *\brief		Crée et ajoute une face au sous-maillage.
		 *\param[in]	faces	Les faces.
		 */
		template< size_t Count >
		void addFaceGroup( std::array< FaceIndices, Count > const & faces )
		{
			addFaceGroup( faces.data(), faces.data() + Count );
		}
		/**
		 *\~english
		 *\brief		Creates and adds faces to the submesh.
		 *\param[in]	faces	The faces.
		 *\~french
		 *\brief		Crée et ajoute une face au sous-maillage.
		 *\param[in]	faces	Les faces.
		 */
		template< uint32_t Count >
		void addFaceGroup( FaceIndices( &faces )[Count] )
		{
			addFaceGroup( faces, faces + Count );
		}

		Face const & operator[]( uint32_t index )const
		{
			CU_Require( index < m_faces.size() );
			return m_faces[index];
		}

		FaceArray const & getFaces()const
		{
			return m_faces;
		}

		FaceArray & getFaces()
		{
			return m_faces;
		}

		void setData( FaceArray faces )
		{
			m_faces = std::move( faces );
		}

	private:
		void doCleanup( RenderDevice const & device )override;
		void doUpload()override;

	public:
		C3D_API static castor::String const Name;

	private:
		//!\~english	The faces in the submesh.
		//!\~french		Le tableau de faces.
		FaceArray m_faces;
		//!\~english	Tells if normals exist or need to be computed.
		//!\~french		Dit si les normales existent ou doivent être calculées.
		bool m_hasNormals{ false };
		//!\~english	The transformed camera position at last sort.
		//!\~french		La position transformée de la caméra au dernier tri.
		castor::Point3f m_cameraPosition;
	};
}

#endif
