namespace glsl
{
	template<>
	struct name_of< Boolean >
	{
		static TypeName const value = TypeName::eBool;
	};

	template<>
	struct name_of< Int >
	{
		static TypeName const value = TypeName::eInt;
	};

	template<>
	struct name_of< UInt >
	{
		static TypeName const value = TypeName::eUInt;
	};

	template<>
	struct name_of< Float >
	{
		static TypeName const value = TypeName::eFloat;
	};

	template<>
	struct name_of< BVec2 >
	{
		static TypeName const value = TypeName::eVec2B;
	};

	template<>
	struct name_of< BVec3 >
	{
		static TypeName const value = TypeName::eVec3B;
	};

	template<>
	struct name_of< BVec4 >
	{
		static TypeName const value = TypeName::eVec4B;
	};

	template<>
	struct name_of< IVec2 >
	{
		static TypeName const value = TypeName::eVec2I;
	};

	template<>
	struct name_of< IVec3 >
	{
		static TypeName const value = TypeName::eVec3I;
	};

	template<>
	struct name_of< IVec4 >
	{
		static TypeName const value = TypeName::eVec4I;
	};

	template<>
	struct name_of< Vec2 >
	{
		static TypeName const value = TypeName::eVec2F;
	};

	template<>
	struct name_of< Vec3 >
	{
		static TypeName const value = TypeName::eVec3F;
	};

	template<>
	struct name_of< Vec4 >
	{
		static TypeName const value = TypeName::eVec4F;
	};

	template<>
	struct name_of< BMat2 >
	{
		static TypeName const value = TypeName::eMat2x2B;
	};

	template<>
	struct name_of< BMat3 >
	{
		static TypeName const value = TypeName::eMat3x3B;
	};

	template<>
	struct name_of< BMat4 >
	{
		static TypeName const value = TypeName::eMat4x4B;
	};

	template<>
	struct name_of< IMat2 >
	{
		static TypeName const value = TypeName::eMat2x2I;
	};

	template<>
	struct name_of< IMat3 >
	{
		static TypeName const value = TypeName::eMat3x3I;
	};

	template<>
	struct name_of< IMat4 >
	{
		static TypeName const value = TypeName::eMat4x4I;
	};

	template<>
	struct name_of< Mat2 >
	{
		static TypeName const value = TypeName::eMat2x2F;
	};

	template<>
	struct name_of< Mat3 >
	{
		static TypeName const value = TypeName::eMat3x3F;
	};

	template<>
	struct name_of< Mat4 >
	{
		static TypeName const value = TypeName::eMat4x4F;
	};

	template<>
	struct name_of< SamplerBuffer >
	{
		static TypeName const value = TypeName::eSamplerBuffer;
	};

	template<>
	struct name_of< Sampler1D >
	{
		static TypeName const value = TypeName::eSampler1D;
	};

	template<>
	struct name_of< Sampler2D >
	{
		static TypeName const value = TypeName::eSampler2D;
	};

	template<>
	struct name_of< Sampler3D >
	{
		static TypeName const value = TypeName::eSampler3D;
	};

	template<>
	struct name_of< SamplerCube >
	{
		static TypeName const value = TypeName::eSamplerCube;
	};

	template<>
	struct name_of< Sampler1DArray >
	{
		static TypeName const value = TypeName::eSampler1DArray;
	};

	template<>
	struct name_of< Sampler2DArray >
	{
		static TypeName const value = TypeName::eSampler2DArray;
	};

	template<>
	struct name_of< SamplerCubeArray >
	{
		static TypeName const value = TypeName::eSamplerCubeArray;
	};

	template<>
	struct name_of< Sampler1DShadow >
	{
		static TypeName const value = TypeName::eSampler1DShadow;
	};

	template<>
	struct name_of< Sampler2DShadow >
	{
		static TypeName const value = TypeName::eSampler2DShadow;
	};

	template<>
	struct name_of< SamplerCubeShadow >
	{
		static TypeName const value = TypeName::eSamplerCubeShadow;
	};

	template<>
	struct name_of< Sampler1DArrayShadow >
	{
		static TypeName const value = TypeName::eSampler1DArrayShadow;
	};

	template<>
	struct name_of< Sampler2DArrayShadow >
	{
		static TypeName const value = TypeName::eSampler2DArrayShadow;
	};

	template<>
	struct name_of< SamplerCubeArrayShadow >
	{
		static TypeName const value = TypeName::eSamplerCubeArrayShadow;
	};

	template<>
	struct name_of< Light >
	{
		static TypeName const value = TypeName::eLight;
	};

	template<>
	struct name_of< DirectionalLight >
	{
		static TypeName const value = TypeName::eDirectionalLight;
	};

	template<>
	struct name_of< PointLight >
	{
		static TypeName const value = TypeName::ePointLight;
	};

	template<>
	struct name_of< SpotLight >
	{
		static TypeName const value = TypeName::eSpotLight;
	};

	template< typename T >
	struct name_of< InParam< T > >
	{
		static TypeName const value = name_of< T >::value;
	};

	template< typename T >
	struct name_of< OutParam< T > >
	{
		static TypeName const value = name_of< T >::value;
	};

	template< typename T >
	struct name_of< InOutParam< T > >
	{
		static TypeName const value = name_of< T >::value;
	};

	template< typename T >
	struct type_of< InParam< T > >
	{
		using type = T;
	};

	template< typename T >
	struct type_of< OutParam< T > >
	{
		using type = T;
	};

	template< typename T >
	struct type_of< InOutParam< T > >
	{
		using type = T;
	};
}
