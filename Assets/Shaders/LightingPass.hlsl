Texture2D ObjTexture : register(t0);
Texture2D ObjNormalTexture : register(t1);
Texture2D ObjSpecularMap : register(t2);
Texture2D ObjDepthTexture : register(t3);
Texture2D ObjUITexture : register(t4);
Texture2D ObjPositionTexture : register(t5);
Texture2D ObjShadowTexture : register(t6);
SamplerState ObjSamplerState;

struct PixelShaderInput
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

struct VertexShaderInput
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct Light
{
	float4 pos;
	float4 dir;
	float4 cameraPos;
	float4 ambient;
	float4 diffuse;
};

cbuffer LightCommand : register(b0)
{
	Light light;
	matrix LightSpaceMatrix;
}

PixelShaderInput main_vs(VertexShaderInput input)
{
	PixelShaderInput vout;
    vout.Position = float4(input.Position, 1.0f);
    vout.TexCoord = input.TexCoord;
	return vout;
}

float4 main_ps(PixelShaderInput input) : SV_TARGET
{
	float4 ui = ObjUITexture.Sample(ObjSamplerState, input.TexCoord);
	float4 color = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
	float4 normal = ObjNormalTexture.Sample(ObjSamplerState, input.TexCoord);
	float4 position = ObjPositionTexture.Sample(ObjSamplerState, input.TexCoord);
	float4 spec = ObjSpecularMap.Sample(ObjSamplerState, input.TexCoord);


	//if (any(spec.xyz))
	//{
	//	float4 lightColor = light.ambient;
	//	float4 lightDir = -light.dir;
	//	float lightIntensity = saturate(dot(normal, lightDir));

	//	lightColor += color * lightIntensity;
	//	color = saturate(lightColor) * color;
	//}

	float3 finalColor = ui.xyz + (color.xyz * (1.0f - ui.a));
	return float4(finalColor, 1.0);
}
