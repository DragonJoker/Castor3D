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
		 *\param[in]	submesh	The parent submesh.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	submesh	Le sous-maillage parent.
		 */
		C3D_API explicit TriFaceMapping( Submesh & submesh
			, VkMemoryPropertyFlags bufferMemoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
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
		 *\return		The created face.
		 *\~french
		 *\brief		Crée et ajoute une face à 4 côtés au sous-maillage.
		 *\param[in]	a		L'index du premier vertex.
		 *\param[in]	b		L'index du second vertex.
		 *\param[in]	c		L'index du troisième vertex.
		 *\param[in]	d		L'index du quatrième vertex.
		 *\param[in]	minUV	L'UV du coin bas gauche.
		 *\param[in]	maxUV	L'UV du coin haut droit.
		 *\return		La face créée.
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
		 *\copydoc		castor3d::IndexMapping::computeNormals
		 */
		C3D_API void computeNormals( InterleavedVertexArray & points
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
		 *\param[in]	face	The face.
		 *\~french
		 *\brief		Calcule la normale et la tangente pour chaque vertex de la face donnée.
		 *\param[in]	face	La face.
		 */
		C3D_API void computeNormals( InterleavedVertexArray & points
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
		 *\param[in]	face	The face.
		 *\~french
		 *\brief		Calcule la tangente pour chaque vertex de la face donnée.
		 *\param[in]	face	La face.
		 */
		C3D_API void computeTangents( InterleavedVertexArray & points
			, Face const & face )const;
		/**
		 *\~english
		 *\brief		Computes tangent for each vertex of the submesh.
		 *\remarks		This function supposes the normals are defined.
		 *\~french
		 *\brief		Calcule la tangente pour chaque vertex du sous-maillage.
		 *\remarks		Cette fonction suppose que les normales sont définies.
		 */
		C3D_API void computeTangentsFromNormals();
		/**
		 *\~english
		 *\brief		Computes tangent for each vertex of the submesh.
		 *\remarks		This function supposes the normals are defined.
		 *\~french
		 *\brief		Calcule la tangente pour chaque vertex du sous-maillage.
		 *\remarks		Cette fonction suppose que les normales sont définies.
		 */
		C3D_API void computeTangentsFromNormals( InterleavedVertexArray & points )const;
		/**
		 *\copydoc		castor3d::IndexMapping::getCount
		 */
		C3D_API uint32_t getCount()const override;
		/**
		 *\copydoc		castor3d::IndexMapping::getComponentsCount
		 */
		C3D_API uint32_t getComponentsCount()const override;
		/**
		 *\copydoc		castor3d::IndexMapping::sortByDistance
		 */
		C3D_API void sortByDistance( castor::Point3f const & cameraPosition )override;
		/**
		 *\copydoc		castor3d::IndexMapping::computeNormals
		 */
		C3D_API void computeNormals( bool reverted = false )override;
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
		/**
		 *\~english
		 *\brief		Retrieves the face at given index.
		 *\param[in]	index	The index.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère la face à l'index donné.
		 *\param[in]	index	L'index.
		 *\return		La valeur.
		 */
		Face const & operator[]( uint32_t index )const
		{
			CU_Require( index < m_faces.size() );
			return m_faces[index];
		}
		/**
		 *\~english
		 *\return		The faces array.
		 *\~french
		 *\return		Le tableau de faces.
		 */
		FaceArray const & getFaces()const
		{
			return m_faces;
		}
		/**
		 *\~english
		 *\return		The faces array.
		 *\~french
		 *\return		Le tableau de faces.
		 */
		FaceArray & getFaces()
		{
			return m_faces;
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
