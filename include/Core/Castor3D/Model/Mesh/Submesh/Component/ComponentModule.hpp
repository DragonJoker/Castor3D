/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelMeshSubmeshComponentModule_H___
#define ___C3D_ModelMeshSubmeshComponentModule_H___

#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"

#define C3D_PluginMakeSubmeshComponentName( p, x ) C3D_Join3Strings( p, "submesh", x )
#define C3D_MakeSubmeshComponentName( x ) C3D_PluginMakeSubmeshComponentName( "c3d", x )

#define C3D_PluginMakeSubmeshIndexComponentName( p, x ) C3D_Join4Strings( p, "submesh", "index", x )
#define C3D_MakeSubmeshIndexComponentName( x ) C3D_PluginMakeSubmeshIndexComponentName( "c3d", x )

#define C3D_PluginMakeSubmeshRenderComponentName( p, x ) C3D_Join4Strings( p, "submesh", "render", x )
#define C3D_MakeSubmeshRenderComponentName( x ) C3D_PluginMakeSubmeshRenderComponentName( "c3d", x )

namespace castor3d
{
	/**@name Model */
	//@{
	/**@name Mesh */
	//@{
	/**@name Submesh */
	//@{
	/**@name Component */
	//@{
	
	struct SubmeshComponentCombine
	{
		SubmeshComponentCombineID baseId{};
		SubmeshComponentFlagsSet flags{};
		// Computed from \p flags
		bool hasLineIndexFlag{};
		bool hasTriangleIndexFlag{};
		bool hasPositionFlag{};
		bool hasNormalFlag{};
		bool hasTangentFlag{};
		bool hasBitangentFlag{};
		bool hasTexcoord0Flag{};
		bool hasTexcoord1Flag{};
		bool hasTexcoord2Flag{};
		bool hasTexcoord3Flag{};
		bool hasColourFlag{};
		bool hasSkinFlag{};
		bool hasMorphFlag{};
		bool hasPassMaskFlag{};
		bool hasVelocityFlag{};
		bool hasRenderFlag{};
	};

	using SubmeshComponentCombines = std::vector< SubmeshComponentCombine >;

	C3D_API bool operator==( SubmeshComponentCombine const & lhs, SubmeshComponentCombine const & rhs )noexcept;

	C3D_API bool hasAny( SubmeshComponentCombine const & lhs
		, SubmeshComponentFlag rhs )noexcept;
	C3D_API bool hasAny( SubmeshComponentCombine const & lhs
		, std::vector< SubmeshComponentFlag > const & rhs )noexcept;
	C3D_API void remFlags( SubmeshComponentCombine & lhs
		, SubmeshComponentFlag rhs )noexcept;
	C3D_API void remFlags( SubmeshComponentCombine & lhs
		, SubmeshComponentFlagsSet const & rhs )noexcept;
	C3D_API void addFlags( SubmeshComponentCombine & lhs
		, SubmeshComponentFlag rhs )noexcept;
	C3D_API void addFlags( SubmeshComponentCombine & lhs
		, SubmeshComponentFlagsSet const & rhs )noexcept;
	C3D_API bool contains( SubmeshComponentCombine const & cont
		, SubmeshComponentFlag test )noexcept;
	C3D_API bool contains( SubmeshComponentCombine const & cont
		, SubmeshComponentCombine const & test )noexcept;
	/**
	\~english
	\brief		Submesh component holding base submesh data.
	\~french
	\brief		Composant de sous-maillage détenant des données basiques d'un sous-maillage.
	*/
	template< SubmeshData SubmeshDataT, typename DataT = castor::Point3f >
	class BaseDataComponentT;
	/**
	\~english
	\brief		Submesh component holding the default render shader.
	\~french
	\brief		Composant de sous-maillage contenant le shader de rendu par défaut.
	*/
	class DefaultRenderComponent;
	/**
	\~english
	\brief		Submesh component holding subpasses masks.
	\~french
	\brief		Composant de sous-maillage contenant les masques des subpasses.
	*/
	class PassMasksComponent;
	/**
	\~english
	\brief		The submesh component used for skinning.
	\~french
	\brief		Le composant de sous-maillage pour le skinning.
	*/
	class SkinComponent;
	/**
	\~english
	\brief		Face implementation
	\remark		A face is constituted from 3 vertices
	\~french
	\brief		Implémentation d'un face
	\remark		Une face est constituée de 3 vertices
	*/
	class Face;
	/**
	\~english
	\brief		Holds the 3 vertex indices
	\~french
	\brief		Contient simplement les index des 3 vertex d'une face
	*/
	struct FaceIndices;
	/**
	\~english
	\brief		Holds face informations.
	\~french
	\brief		Contient des informations sur une face.
	*/
	struct FaceInfos;
	/**
	\~english
	\brief		A submesh component for faces.
	\~french
	\brief		Un composant de sous-maillage pour les faces.
	*/
	class IndexMapping;
	/**
	\~english
	\brief		The transformation matrix and material index for one instance.
	\~french
	\brief		La matrice de transformation et l'index de matériau pour une instance.
	*/
	struct InstantiationData;
	/**
	\~english
	\brief		The submesh component used for hardware instantiation.
	\~french
	\brief		Le composant de sous-maillage pour l'instantiation matérielle.
	*/
	class InstantiationComponent;
	/**
	\~english
	\brief		Line implementation.
	\~french
	\brief		Implémentation d'un ligne.
	*/
	class Line;
	/**
	\~english
	\brief		Holds the 2 vertex indices of a ligne.
	\~french
	\brief		Contient les index des 2 sommets d'une ligne.
	*/
	struct LineIndices;
	/**
	\~english
	\brief		The submesh component for lines.
	\~french
	\brief		Composant de sous-maillage pour les lignes.
	*/
	class LinesMapping;
	/**
	\~english
	\brief		The submesh component used for meshlets.
	\~french
	\brief		Le composant de sous-maillage pour les meshlets.
	*/
	class MeshletComponent;
	/**
	\~english
	\brief		The submesh component used for morphing.
	\~french
	\brief		Le composant de sous-maillage pour le morphing.
	*/
	class MorphComponent;
	/**
	\~english
	\brief		Component for a submesh.
	\~french
	\brief		Composant pour un sous-maillage.
	*/
	class SubmeshComponent;
	/**
	\~english
	\brief		Submesh component data holder.
	\~french
	\brief		Détient les données d'un composant de sous-maillage.
	*/
	struct SubmeshComponentData;
	/**
	\~english
	\brief		Plugin for a submesh component.
	\~french
	\brief		Plugin pour un composant pour un sous-maillage.
	*/
	class SubmeshComponentPlugin;
	/**
	\~english
	\brief		Submesh components registrar.
	\~french
	\brief		Registre des composants de sous-maillage.
	*/
	class SubmeshComponentRegister;
	/**
	\~english
	\brief		Submesh render component data.
	\~french
	\brief		Données de composant de rendu de sous-maillage.
	*/
	struct SubmeshRenderData;
	/**
	\~english
	\brief		Submesh render component.
	\~french
	\brief		Composant de rendu de sous-maillage.
	*/
	struct SubmeshRenderShader;
	/**
	\~english
	\brief		The submesh component for triangular faces.
	\~french
	\brief		Composant de sous-maillage pour les faces triangulaires.
	*/
	class TriFaceMapping;
	/**
	\~english
	\brief		The submesh component holding bitangents.
	\~french
	\brief		Le composant de sous-maillage contenant les bitangentes.
	*/
	using BitangentsComponent = BaseDataComponentT< SubmeshData::eBitangents >;
	/**
	\~english
	\brief		The submesh component holding normals.
	\~french
	\brief		Le composant de sous-maillage contenant les normales.
	*/
	using NormalsComponent = BaseDataComponentT< SubmeshData::eNormals >;
	/**
	\~english
	\brief		The submesh component holding positions.
	\~french
	\brief		Le composant de sous-maillage contenant les positions.
	*/
	using PositionsComponent = BaseDataComponentT< SubmeshData::ePositions >;
	/**
	\~english
	\brief		The submesh component holding tangents.
	\~french
	\brief		Le composant de sous-maillage contenant les tangentes.
	*/
	using TangentsComponent = BaseDataComponentT< SubmeshData::eTangents, castor::Point4f >;
	/**
	\~english
	\brief		The submesh component holding texture coordinates.
	\~french
	\brief		Le composant de sous-maillage contenant les coordonnées de texture.
	*/
	using Texcoords0Component = BaseDataComponentT< SubmeshData::eTexcoords0 >;
	/**
	\~english
	\brief		The submesh component holding texture coordinates.
	\~french
	\brief		Le composant de sous-maillage contenant les coordonnées de texture.
	*/
	using Texcoords1Component = BaseDataComponentT< SubmeshData::eTexcoords1 >;
	/**
	\~english
	\brief		The submesh component holding texture coordinates.
	\~french
	\brief		Le composant de sous-maillage contenant les coordonnées de texture.
	*/
	using Texcoords2Component = BaseDataComponentT< SubmeshData::eTexcoords2 >;
	/**
	\~english
	\brief		The submesh component holding texture coordinates.
	\~french
	\brief		Le composant de sous-maillage contenant les coordonnées de texture.
	*/
	using Texcoords3Component = BaseDataComponentT< SubmeshData::eTexcoords3 >;
	/**
	\~english
	\brief		The submesh component holding colours.
	\~french
	\brief		Le composant de sous-maillage contenant les couleurs.
	*/
	using ColoursComponent = BaseDataComponentT< SubmeshData::eColours >;
	/**
	\~english
	\brief		The submesh component holding colours.
	\~french
	\brief		Le composant de sous-maillage contenant les couleurs.
	*/
	using VelocityComponent = BaseDataComponentT< SubmeshData::eVelocity >;

	CU_DeclareSmartPtr( castor3d, DefaultRenderComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, IndexMapping, C3D_API );
	CU_DeclareSmartPtr( castor3d, InstantiationComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, LinesMapping, C3D_API );
	CU_DeclareSmartPtr( castor3d, MeshletComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, MorphComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, PassMasksComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, SkinComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, SubmeshComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, SubmeshComponentPlugin, C3D_API );
	CU_DeclareSmartPtr( castor3d, SubmeshComponentRegister, C3D_API );
	CU_DeclareSmartPtr( castor3d, TriFaceMapping, C3D_API );

	using SubmeshComponentDataUPtr = std::unique_ptr< SubmeshComponentData >;
	using SubmeshComponentDataRPtr = SubmeshComponentData *;
	using SubmeshRenderDataPtr = std::unique_ptr< SubmeshRenderData >;
	using SubmeshRenderShaderPtr = std::unique_ptr< SubmeshRenderShader >;

	//! Face array
	CU_DeclareVector( Face, Face );

	CU_DeclareMap( SubmeshComponentID, SubmeshComponentUPtr, SubmeshComponentID );

	C3D_API castor::String const & getSubmeshComponentType( SubmeshComponent const & component );

	using CreateSubmeshComponentPlugin = std::function< SubmeshComponentPluginUPtr( SubmeshComponentRegister const & ) >;

	namespace shader
	{
		struct SubmeshVertexSurfaceShader;
		struct SubmeshRasterSurfaceShader;

		using SubmeshVertexSurfaceShaderPtr = std::unique_ptr< SubmeshVertexSurfaceShader >;
		using SubmeshRasterSurfaceShaderPtr = std::unique_ptr< SubmeshRasterSurfaceShader >;
	}

	//@}
	//@}
	//@}
	//@}
}

namespace castor
{
	template< castor3d::SubmeshData SubmeshDataT, typename DataT >
	struct Deleter< castor3d::BaseDataComponentT< SubmeshDataT, DataT > >
	{
		void operator()( castor3d::BaseDataComponentT< SubmeshDataT, DataT > * pointer )noexcept
		{
			delete pointer;
		}
	};
}
namespace castor3d
{
	template< castor3d::SubmeshData SubmeshDataT, typename DataT >
	using BaseDataComponentUPtrT = castor::UniquePtr< BaseDataComponentT< SubmeshDataT, DataT > >;
	template< castor3d::SubmeshData SubmeshDataT, typename DataT >
	using BaseDataComponentRPtrT = BaseDataComponentT< SubmeshDataT, DataT > *;
}

#endif
