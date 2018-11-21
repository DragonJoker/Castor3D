namespace glsl
{
	template<>
	struct TypeTraits< Boolean >
	{
		static TypeName const TypeEnum = TypeName::eBool;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< Int >
	{
		static TypeName const TypeEnum = TypeName::eInt;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< UInt >
	{
		static TypeName const TypeEnum = TypeName::eUInt;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< Float >
	{
		static TypeName const TypeEnum = TypeName::eFloat;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< BVec2 >
	{
		static TypeName const TypeEnum = TypeName::eVec2B;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< BVec3 >
	{
		static TypeName const TypeEnum = TypeName::eVec3B;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< BVec4 >
	{
		static TypeName const TypeEnum = TypeName::eVec4B;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< IVec2 >
	{
		static TypeName const TypeEnum = TypeName::eVec2I;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< IVec3 >
	{
		static TypeName const TypeEnum = TypeName::eVec3I;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< IVec4 >
	{
		static TypeName const TypeEnum = TypeName::eVec4I;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< UVec2 >
	{
		static TypeName const TypeEnum = TypeName::eVec2UI;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< UVec3 >
	{
		static TypeName const TypeEnum = TypeName::eVec3UI;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< UVec4 >
	{
		static TypeName const TypeEnum = TypeName::eVec4UI;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< Vec2 >
	{
		static TypeName const TypeEnum = TypeName::eVec2F;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< Vec3 >
	{
		static TypeName const TypeEnum = TypeName::eVec3F;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< Vec4 >
	{
		static TypeName const TypeEnum = TypeName::eVec4F;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< BMat2 >
	{
		static TypeName const TypeEnum = TypeName::eMat2x2B;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< BMat3 >
	{
		static TypeName const TypeEnum = TypeName::eMat3x3B;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< BMat4 >
	{
		static TypeName const TypeEnum = TypeName::eMat4x4B;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< IMat2 >
	{
		static TypeName const TypeEnum = TypeName::eMat2x2I;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< IMat3 >
	{
		static TypeName const TypeEnum = TypeName::eMat3x3I;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< IMat4 >
	{
		static TypeName const TypeEnum = TypeName::eMat4x4I;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< Mat2 >
	{
		static TypeName const TypeEnum = TypeName::eMat2x2F;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< Mat3 >
	{
		static TypeName const TypeEnum = TypeName::eMat3x3F;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< Mat4 >
	{
		static TypeName const TypeEnum = TypeName::eMat4x4F;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< SamplerBuffer >
	{
		static TypeName const TypeEnum = TypeName::eSamplerBuffer;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< Sampler1D >
	{
		static TypeName const TypeEnum = TypeName::eSampler1D;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< Sampler2D >
	{
		static TypeName const TypeEnum = TypeName::eSampler2D;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< Sampler3D >
	{
		static TypeName const TypeEnum = TypeName::eSampler3D;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< SamplerCube >
	{
		static TypeName const TypeEnum = TypeName::eSamplerCube;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< Sampler2DRect >
	{
		static TypeName const TypeEnum = TypeName::eSampler2DRect;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< Sampler1DArray >
	{
		static TypeName const TypeEnum = TypeName::eSampler1DArray;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< Sampler2DArray >
	{
		static TypeName const TypeEnum = TypeName::eSampler2DArray;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< SamplerCubeArray >
	{
		static TypeName const TypeEnum = TypeName::eSamplerCubeArray;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< Sampler1DShadow >
	{
		static TypeName const TypeEnum = TypeName::eSampler1DShadow;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< Sampler2DShadow >
	{
		static TypeName const TypeEnum = TypeName::eSampler2DShadow;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< SamplerCubeShadow >
	{
		static TypeName const TypeEnum = TypeName::eSamplerCubeShadow;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< Sampler2DRectShadow >
	{
		static TypeName const TypeEnum = TypeName::eSampler2DRectShadow;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< Sampler1DArrayShadow >
	{
		static TypeName const TypeEnum = TypeName::eSampler1DArrayShadow;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< Sampler2DArrayShadow >
	{
		static TypeName const TypeEnum = TypeName::eSampler2DArrayShadow;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< SamplerCubeArrayShadow >
	{
		static TypeName const TypeEnum = TypeName::eSamplerCubeArrayShadow;
		GlslWriter_API static castor::String const Name;
	};

	template<>
	struct SamplerTypeTraits< SamplerType::eBuffer >
	{
		static TypeName const TypeEnum = TypeName::eSamplerBuffer;
		using Type = SamplerBuffer;
	};

	template<>
	struct SamplerTypeTraits< SamplerType::e1D >
	{
		static TypeName const TypeEnum = TypeName::eSampler1D;
		using Type = Sampler1D;
	};

	template<>
	struct SamplerTypeTraits< SamplerType::e2D >
	{
		static TypeName const TypeEnum = TypeName::eSampler2D;
		using Type = Sampler2D;
	};

	template<>
	struct SamplerTypeTraits< SamplerType::e3D >
	{
		static TypeName const TypeEnum = TypeName::eSampler3D;
		using Type = Sampler3D;
	};

	template<>
	struct SamplerTypeTraits< SamplerType::eCube >
	{
		static TypeName const TypeEnum = TypeName::eSamplerCube;
		using Type = SamplerCube;
	};

	template<>
	struct SamplerTypeTraits< SamplerType::e2DRect >
	{
		static TypeName const TypeEnum = TypeName::eSampler2DRect;
		using Type = Sampler2DRect;
	};

	template<>
	struct SamplerTypeTraits< SamplerType::e1DShadow >
	{
		static TypeName const TypeEnum = TypeName::eSampler1DShadow;
		using Type = Sampler1DShadow;
	};

	template<>
	struct SamplerTypeTraits< SamplerType::e2DShadow >
	{
		static TypeName const TypeEnum = TypeName::eSampler2DShadow;
		using Type = Sampler2DShadow;
	};

	template<>
	struct SamplerTypeTraits< SamplerType::eCubeShadow >
	{
		static TypeName const TypeEnum = TypeName::eSamplerCubeShadow;
		using Type = SamplerCubeShadow;
	};

	template<>
	struct SamplerTypeTraits< SamplerType::e2DRectShadow >
	{
		static TypeName const TypeEnum = TypeName::eSampler2DRectShadow;
		using Type = Sampler2DRectShadow;
	};

	template<>
	struct SamplerTypeTraits< SamplerType::e1DArray >
	{
		static TypeName const TypeEnum = TypeName::eSampler1DArray;
		using Type = Sampler1DArray;
	};

	template<>
	struct SamplerTypeTraits< SamplerType::e2DArray >
	{
		static TypeName const TypeEnum = TypeName::eSampler2DArray;
		using Type = Sampler2DArray;
	};

	template<>
	struct SamplerTypeTraits< SamplerType::eCubeArray >
	{
		static TypeName const TypeEnum = TypeName::eSamplerCubeArray;
		using Type = SamplerCubeArray;
	};

	template<>
	struct SamplerTypeTraits< SamplerType::e1DArrayShadow >
	{
		static TypeName const TypeEnum = TypeName::eSampler1DArrayShadow;
		using Type = Sampler1DArrayShadow;
	};

	template<>
	struct SamplerTypeTraits< SamplerType::e2DArrayShadow >
	{
		static TypeName const TypeEnum = TypeName::eSampler2DArrayShadow;
		using Type = Sampler2DArrayShadow;
	};

	template<>
	struct SamplerTypeTraits< SamplerType::eCubeArrayShadow >
	{
		static TypeName const TypeEnum = TypeName::eSamplerCubeArrayShadow;
		using Type = SamplerCubeArrayShadow;
	};

	template< typename T >
	struct TypeTraits< InParam< T > >
	{
		static TypeName const TypeEnum = TypeTraits< T >::TypeEnum;
	};

	template< typename T >
	struct TypeTraits< Optional< T > >
	{
		static TypeName const TypeEnum = TypeTraits< T >::TypeEnum;
	};

	template< typename T >
	struct TypeTraits< OutParam< T > >
	{
		static TypeName const TypeEnum = TypeTraits< T >::TypeEnum;
	};

	template< typename T >
	struct TypeTraits< InOutParam< T > >
	{
		static TypeName const TypeEnum = TypeTraits< T >::TypeEnum;
	};

	template< typename T >
	struct TypeOf< Optional< T > >
	{
		using Type = T;
	};

	template< typename T >
	struct TypeOf< InParam< T > >
	{
		using Type = T;
	};

	template< typename T >
	struct TypeOf< OutParam< T > >
	{
		using Type = T;
	};

	template< typename T >
	struct TypeOf< InOutParam< T > >
	{
		using Type = T;
	};
}
