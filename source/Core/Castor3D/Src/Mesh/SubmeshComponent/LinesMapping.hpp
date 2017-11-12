/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LinesMapping_H___
#define ___C3D_LinesMapping_H___

#include "IndexMapping.hpp"
#include "Line.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\date		11/11/2017
	\~english
	\brief		The submesh component for lines.
	\~french
	\brief		Composant de sous-maillage pour les lignes.
	*/
	class LinesMapping
		: public IndexMapping
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_mesh	The parent mesh.
		 *\param[in]	p_scene	The scene.
		 *\param[in]	p_id	The submesh ID.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_mesh	Le mesh parent.
		 *\param[in]	p_scene	La scène.
		 *\param[in]	p_id	L'ID du sous-maillage.
		 */
		C3D_API LinesMapping( Submesh & submesh );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~LinesMapping();
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
		 *\~english
		 *\brief		Generates normals and tangents.
		 *\~french
		 *\brief		Génère les normales et les tangentes.
		 */
		C3D_API void computeNormals( bool reverted = false )override;
		/**
		 *\~english
		 *\brief		Sorts the lines from farthest to nearest from the camera.
		 *\param[in]	p_cameraPosition	The camera position, relative to submesh.
		 *\~french
		 *\brief		Trie les lignes des plus éloignées aux plus proches de la caméra.
		 *\param[in]	p_cameraPosition	La position de la caméra, relative au sous-maillage.
		 */
		C3D_API void sortByDistance( castor::Point3r const & p_cameraPosition )override;
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
		 *\brief		Retrieves the line at given index
		 *\param[in]	index	The index
		 *\return		The value
		 *\~french
		 *\brief		Récupère la ligne à l'index donné
		 *\param[in]	index	L'index
		 *\return		La valeur
		 */
		inline Line const & operator[]( uint32_t index )const
		{
			REQUIRE( index < m_lines.size() );
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
		void doCleanup()override;
		void doFill()override;

	public:
		C3D_API static castor::String const Name;

	private:
		//!\~english	The lines in the submesh.
		//!\~french		Le tableau de lignes.
		LineArray m_lines;
		//!\~english	The lines count.
		//!\~french		Le nombre de lignes.
		uint32_t m_lineCount{ 0u };
		//!\~english	The transformed camera position at last sort.
		//!\~french		La position transformée de la caméra au dernier tri.
		castor::Point3r m_cameraPosition;
	};
}

#endif
