/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FaceMapping_H___
#define ___C3D_FaceMapping_H___

#include "SubmeshComponent.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\date		11/11/2017
	\~english
	\brief		A submesh component for faces.
	\~french
	\brief		Un composant de sous-maillage pour les faces.
	*/
	class IndexMapping
		: public SubmeshComponent
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	submesh	The parent submesh.
		 *\param[in]	type	The component type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	submesh	Le sous-maillage parent.
		 *\param[in]	type	Le type de composant.
		 */
		C3D_API IndexMapping( Submesh & submesh
			, castor::String const & type );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~IndexMapping();
		/**
		 *\~english
		 *\return		The elements count.
		 *\~french
		 *\return		Le nombre d'éléments.
		 */
		C3D_API virtual uint32_t getCount()const = 0;
		/**
		 *\~english
		 *\return		The components count in an element.
		 *\~french
		 *\return		Le nombre de composantes d'un élément.
		 */
		C3D_API virtual uint32_t getComponentsCount()const = 0;
		/**
		 *\~english
		 *\brief		Sorts the face from farthest to nearest from the camera
		 *\param[in]	cameraPosition	The camera position, relative to submesh
		 *\~french
		 *\brief		Trie les faces des plus éloignées aux plus proches de la caméra
		 *\param[in]	cameraPosition	La position de la caméra, relative au sous-maillage
		 */
		C3D_API virtual void sortByDistance( castor::Point3r const & cameraPosition ) = 0;
		/**
		 *\~english
		 *\brief		Generates normals and tangents.
		 *\param[in]	reverted	Tells if normals are inverted.
		 *\~french
		 *\brief		Génère les normales et les tangentes.
		 *\param[in]	reverted	Dit si les normales sont inversées.
		 */
		C3D_API virtual void computeNormals( bool reverted = false ) = 0;
		/**
		 *\copydoc		castor3d::SubmeshComponent::gather
		 */
		inline void gather( renderer::VertexBufferCRefArray & buffers )override
		{
		}
		/**
		 *\copydoc		castor3d::SubmeshComponent::getProgramFlags
		 */
		inline ProgramFlags getProgramFlags()const override
		{
			return ProgramFlags{};
		}

	private:
		inline bool doInitialise()override
		{
			return true;
		}

		inline void doCleanup()override
		{
		}

		inline void doUpload()override
		{
		}
	};
}

#endif
