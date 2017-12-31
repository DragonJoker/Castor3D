/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BonesComponent_H___
#define ___C3D_BonesComponent_H___

#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"
#include "Mesh/Skeleton/VertexBoneData.hpp"
#include "Mesh/SubmeshComponent/SubmeshComponent.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\date		09/11/2017
	\~english
	\brief		The submesh component used for skinning.
	\~french
	\brief		Le composant de sous-maillage pour le skinning.
	*/
	class BonesComponent
		: public SubmeshComponent
	{
	private:
		DECLARE_LIST( castor::ByteArray, BytePtr );

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	submesh	The parent submesh.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	submesh	Le sous-maillage parent.
		 */
		C3D_API explicit BonesComponent( Submesh & submesh );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~BonesComponent();
		/**
		 *\~english
		 *\brief		adds bone datas.
		 *\param[in]	p_begin	The bones data begin.
		 *\param[in]	p_end	The bones data end.
		 *\~french
		 *\brief		Ajoute des données de bones.
		 *\param[in]	p_begin	Le début des données de bones.
		 *\param[in]	p_end	La fin des données de bones.
		 */
		C3D_API void addBoneDatas( VertexBoneData const * const p_begin, VertexBoneData const * const p_end );
		/**
		 *\~english
		 *\return		The skeleton.
		 *\~french
		 *\return		Le squelette.
		 */
		C3D_API SkeletonSPtr getSkeleton()const;
		/**
		 *\copydoc		castor3d::SubmeshComponent::gather
		 */
		C3D_API void gather( VertexBufferArray & buffers )override;
		/**
		 *\~english
		 *\brief		adds bone datas.
		 *\param[in]	p_boneData	The bone datas.
		 *\~french
		 *\brief		Ajoute des données de bones.
		 *\param[in]	p_boneData	Les données de bones.
		 */
		inline void addBoneDatas( std::vector< VertexBoneData > const & p_boneData )
		{
			addBoneDatas( p_boneData.data(), p_boneData.data() + p_boneData.size() );
		}
		/**
		 *\~english
		 *\brief		adds bone datas.
		 *\param[in]	p_boneData	The bone datas.
		 *\~french
		 *\brief		Ajoute des données de bones.
		 *\param[in]	p_boneData	Les données de bones.
		 */
		template< size_t Count >
		inline void addBoneDatas( std::array< VertexBoneData, Count > const & p_boneData )
		{
			addBoneDatas( p_boneData.data(), p_boneData.data() + p_boneData.size() );
		}
		/**
		 *\~english
		 *\return		\p true if the submesh has bone data.
		 *\~french
		 *\return		\p true si le sous-maillage a des données d'os.
		 */
		inline bool hasBoneData()const
		{
			return !m_bonesData.empty();
		}
		/**
		 *\~english
		 *\return		The bone data.
		 *\~french
		 *\return		Les données d'os.
		 */
		inline VertexPtrArray const & getBonesData()const
		{
			return m_bones;
		}
		/**
		 *\~english
		 *\return		The bones VertexBuffer.
		 *\~french
		 *\return		Le VertexBuffer des bones.
		 */
		inline VertexBuffer const & getBonesBuffer()const
		{
			return m_bonesBuffer;
		}
		/**
		 *\~english
		 *\return		The bones VertexBuffer.
		 *\~french
		 *\return		Le VertexBuffer des bones.
		 */
		inline VertexBuffer & getBonesBuffer()
		{
			return m_bonesBuffer;
		}
		/**
		 *\copydoc		castor3d::SubmeshComponent::getProgramFlags
		 */
		inline ProgramFlags getProgramFlags()const override
		{
			return hasBoneData()
				? ProgramFlag::eSkinning
				: ProgramFlag( 0 );
		}

	private:
		bool doInitialise()override;
		void doCleanup()override;
		void doFill()override;
		void doUpload()override;

	public:
		C3D_API static castor::String const Name;

	private:
		//!\~english	The bone data buffer (animation).
		//!\~french		Le tampon de données de bones (animation).
		VertexBuffer m_bonesBuffer;
		//!\~english	The bones data array.
		//!\~french		Le tableau de données des bones.
		BytePtrList m_bonesData;
		//!\~english	The bones pointer array.
		//!\~french		Le tableau de bones.
		VertexPtrArray m_bones;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date		09/11/2017
	\~english
	\brief		Helper structure to find ChunkType from a type.
	\remarks	Specialisation for BonesComponent.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour BonesComponent.
	*/
	template<>
	struct ChunkTyper< BonesComponent >
	{
		static ChunkType const Value = ChunkType::eBonesComponent;
	};
	/*!
	\author		Sylvain DOREMUS
	\date		09/11/2017
	\~english
	\brief		BonesComponent loader.
	\~english
	\brief		Loader de BonesComponent.
	*/
	template<>
	class BinaryWriter< BonesComponent >
		: public BinaryWriterBase< BonesComponent >
	{
	private:
		/**
		 *\~english
		 *\brief		Function used to fill the chunk from specific data.
		 *\param[in]	obj	The object to write.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques.
		 *\param[in]	obj	L'objet à écrire.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool doWrite( BonesComponent const & obj )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\date		09/11/2017
	\~english
	\brief		BonesComponent loader.
	\~english
	\brief		Loader de BonesComponent.
	*/
	template<>
	class BinaryParser< BonesComponent >
		: public BinaryParserBase< BonesComponent >
	{
	private:
		/**
		 *\~english
		 *\brief		Function used to retrieve specific data from the chunk.
		 *\param[out]	obj	The object to read.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk.
		 *\param[out]	obj	L'objet à lire.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool doParse( BonesComponent & obj )override;
	};
}

#endif
