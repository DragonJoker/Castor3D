/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LinesMapping_H___
#define ___C3D_LinesMapping_H___

#include "Castor3D/Model/Mesh/Submesh/Component/IndexMapping.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/Line.hpp"

namespace castor3d
{
	class LinesMapping
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
		C3D_API explicit LinesMapping( Submesh & submesh
			, VkBufferUsageFlags bufferUsageFlags = {} );
		/**
		 *\~english
		 *\brief		Clears the lines array.
		 *\~french
		 *\brief		Vide le tableau de lignes.
		 */
		C3D_API void clearLines();
		/**
		 *\~english
		 *\brief		Creates and adds a line to the submesh.
		 *\param[in]	a	The first vertex index.
		 *\param[in]	b	The second vertex index.
		 *\return		The created face.
		 *\~french
		 *\brief		Crée et ajoute une ligne au sous-maillage.
		 *\param[in]	a	L'index du premier sommet.
		 *\param[in]	b	L'index du second sommet.
		 *\return		La face créée.
		 */
		C3D_API Line addLine( uint32_t a, uint32_t b );
		/**
		 *\~english
		 *\brief		Creates and adds lines to the submesh.
		 *\param[in]	begin	The faces data begin.
		 *\param[in]	end		The faces data end.
		 *\~french
		 *\brief		Crée et ajoute des lignes au sous-maillage.
		 *\param[in]	begin	Le début des données de lignes.
		 *\param[in]	end		La fin des données de lignes.
		 */
		C3D_API void addLineGroup( LineIndices const * const begin
			, LineIndices const * const end );
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
		 *\brief		Creates and adds lines to the submesh.
		 *\param[in]	lines	The lines.
		 *\~french
		 *\brief		Crée et ajoute un group de lignes au sous-maillage.
		 *\param[in]	lines	Les lignes.
		 */
		inline void addLineGroup( std::vector< LineIndices > const & lines )
		{
			addLineGroup( lines.data(), lines.data() + lines.size() );
		}
		/**
		 *\~english
		 *\brief		Creates and adds lines to the submesh.
		 *\param[in]	lines	The lines.
		 *\~french
		 *\brief		Crée et ajoute une face au sous-maillage.
		 *\param[in]	lines	Les lignes.
		 */
		template< size_t Count >
		inline void addLineGroup( std::array< LineIndices, Count > const & lines )
		{
			addLineGroup( lines.data(), lines.data() + Count );
		}
		/**
		 *\~english
		 *\brief		Creates and adds lines to the submesh.
		 *\param[in]	lines	The lines.
		 *\~french
		 *\brief		Crée et ajoute une face au sous-maillage.
		 *\param[in]	lines	Les lignes.
		 */
		template< uint32_t Count >
		void addLineGroup( LineIndices( &lines )[Count] )
		{
			addLineGroup( lines, lines + Count );
		}
		/**
		 *\~english
		 *\brief		Retrieves the line at given index.
		 *\param[in]	index	The index.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère la ligne à l'index donné.
		 *\param[in]	index	L'index.
		 *\return		La valeur.
		 */
		inline Line const & operator[]( uint32_t index )const
		{
			CU_Require( index < m_lines.size() );
			return m_lines[index];
		}
		/**
		 *\~english
		 *\return		The lines array.
		 *\~french
		 *\return		Le tableau de lignes.
		 */
		inline LineArray const & getFaces()const
		{
			return m_lines;
		}
		/**
		 *\~english
		 *\return		The lines array.
		 *\~french
		 *\return		Le tableau de lignes.
		 */
		inline LineArray & getFaces()
		{
			return m_lines;
		}

	private:
		void doCleanup( RenderDevice const & device )override;
		void doUpload()override;

	public:
		C3D_API static castor::String const Name;

	private:
		//!\~english	The lines in the submesh.
		//!\~french		Le tableau de lignes.
		LineArray m_lines;
		//!\~english	The transformed camera position at last sort.
		//!\~french		La position transformée de la caméra au dernier tri.
		castor::Point3f m_cameraPosition;
	};
}

#endif
