/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GEOMETRY_BUFFERS_H___
#define ___C3D_GEOMETRY_BUFFERS_H___

#include "Castor3DPrerequisites.hpp"

#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date		18/11/2012
	\~english
	\brief		Geometry buffers holder
	\remark		Allows implementations to use API specific optimisations (like OpenGL Vertex array objects)
				<br />Acquires it's buffers responsibility when constructed
	\~french
	\brief		Conteneur de buffers de géométries
	\remark		Permet aux implémentations d'utiliser les optimisations spécifiques aux API (comme les Vertex arrays objects OpenGL)
				<br />Acquiert la responsabilité de ses buffers à la création
	*/
	class GeometryBuffers
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_topology	The buffers topology.
		 *\param[in]	p_program	The shader program.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_topology	La topologie des tampons.
		 *\param[in]	p_program	Le programme shader.
		 */
		C3D_API GeometryBuffers( Topology p_topology, ShaderProgram const & p_program );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~GeometryBuffers();
		/**
		 *\~english
		 *\brief		Creates the VAO.
		 *\param[in]	p_buffers	The vertex, animation, ... buffers.
		 *\param[in]	p_index		The index buffer.
		 *\return		\p true on ok.
		 *\~french
		 *\brief		Crée le VAO.
		 *\param[in]	p_buffers	Les tampons de sommets, animations, ....
		 *\param[in]	p_index		Le tampon d'indices.
		 *\return		\p true si ça s'est bien passé.
		 */
		C3D_API bool initialise( VertexBufferArray const & p_buffers, IndexBufferRPtr p_index );
		/**
		 *\~english
		 *\brief		Destroys the VAO.
		 *\~french
		 *\brief		Détruit le VAO.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Draws the geometry held into the buffers
		 *\param[in]	p_size	Specifies the number of elements to be rendered
		 *\param[in]	p_index	Specifies the starting index in the enabled arrays
		 *\return		\p true if OK
		 *\~french
		 *\brief		Dessine la géométrie contenue dans les buffers
		 *\param[in]	p_size	Spécifie le nombre de vertices à rendre
		 *\param[in]	p_index	Spécifie l'indice du premier vertice
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool draw( uint32_t p_size, uint32_t p_index )const = 0;
		/**
		 *\~english
		 *\brief		Draws the geometry held into the buffers
		 *\param[in]	p_size	Specifies the number of elements to be rendered
		 *\param[in]	p_index	Specifies the starting index in the enabled arrays
		 *\param[in]	p_count	The instances count
		 *\return		\p true if OK
		 *\~french
		 *\brief		Dessine la géométrie contenue dans les buffers
		 *\param[in]	p_size	Spécifie le nombre de vertices à rendre
		 *\param[in]	p_index	Spécifie l'indice du premier vertice
		 *\param[in]	p_count	Le nombre d'instances à dessiner
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool drawInstanced( uint32_t p_size, uint32_t p_index, uint32_t p_count )const = 0;
		/**
		 *\~english
		 *\return		The program.
		 *\~french
		 *\return		Le programme.
		 */
		inline ShaderProgram const & getProgram()const
		{
			return m_program;
		}
		/**
		 *\~english
		 *\return		The buffers topology.
		 *\~french
		 *\return		La topologie des tampons.
		 */
		inline Topology getTopology()const
		{
			return m_topology;
		}
		/**
		 *\~english
		 *\brief		sets the buffers topology.
		 *\param[in]	p_value	The new value
		 *\~french
		 *\brief		Définit la topologie des tampons.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void setTopology( Topology p_value )
		{
			m_topology = p_value;
		}

	private:
		/**
		 *\~english
		 *\brief		Creates the VAO.
		 *\return		\p true on ok.
		 *\~french
		 *\brief		Crée le VAO.
		 *\return		\p true si ça s'est bien passé.
		 */
		C3D_API virtual bool doInitialise() = 0;
		/**
		 *\~english
		 *\brief		Destroys the VAO.
		 *\~french
		 *\brief		Détruit le VAO.
		 */
		C3D_API virtual void doCleanup() = 0;

	protected:
		//!\~english	The vertex, animation, ... buffers.
		//!\\~french	Les tampons de sommets, animations, ....
		VertexBufferArray m_buffers;
		//!\~english	The index buffer.
		//!\\~french	Le tampon d'indices.
		IndexBufferRPtr m_indexBuffer{ nullptr };
		//!\~english	The shader program.
		//!\\~french	Le programme de rendu.
		ShaderProgram const & m_program;
		//!\~english	The buffers topology.
		//!\\~french	La topologie des tampons.
		Topology m_topology;
	};
}

#endif
