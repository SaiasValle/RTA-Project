Texture2D Texture : register(t0);
Texture2D normalMap : register(t1);
Texture2D specularMap : register(t2);
SamplerState Sample : register(s0);

cbuffer pointLight : register(b1)
{
	float4 Pcolor;
	float4 position;
};
cbuffer directionLight : register(b2)
{
	float4 direction;
	float4 Dcolor;
};
cbuffer spotLight : register(b3)
{
	float4 Sposition;
	float4 Sdirection;
	float4 Scolor;
	float4 spotRatio;
	float4 cameraPos;
};
struct INPUT_PIXEL
{
	float4 coordinate : SV_POSITION;
	float2 uv : UV;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float3 biTangent: BITAN;
	float4 worldPos : WORLDPOS;
};

float4 main(INPUT_PIXEL input) : SV_TARGET
{
	float4 surfColor = Texture.Sample(Sample, input.uv);

	///////NORMAL MAPPING CODE///////
	float3 MapVal = normalMap.Sample(Sample, input.uv).xyz;
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);
	input.biTangent = normalize(input.biTangent);
	MapVal = (MapVal * 2.0f) - 1.0f;
	float4 ambient = float4(0.15f * MapVal.z *  surfColor.xyz, surfColor.a);
	float3x3 TBN;
	/*##spacing stuff##*/
	TBN[0] = input.tangent.xyz;
	TBN[1] = input.biTangent.xyz;
	TBN[2] = input.normal.xyz;
	MapVal = mul(MapVal, TBN);
	//return float4(1, input.normal.xyz);
	///////NORMAL MAPPING CODE///////

	// Directional Light
	float4 DlightDirNorm = -normalize(direction);
		float4 lightColor = surfColor * Dcolor;
		float3 norm = input.normal;
		float4 dirLight = saturate(dot(DlightDirNorm, MapVal) * lightColor);

		float4 specularDir = normalize(cameraPos - input.coordinate);
		float4 halfvec = normalize(-DlightDirNorm * specularDir);
		float specMap = specularMap.Sample(Sample, input.uv).x;
		float4 intensity = specMap*saturate(dot(input.normal, normalize(halfvec)));
		dirLight = dirLight * intensity;

	// Point Light
	float4 lightDir = normalize(position - input.worldPos);
		float lightRatio = saturate(dot(lightDir, MapVal));
	float4 finalPointLight = lightRatio * Pcolor * surfColor;

		// Spot Light
		float4 SlightDir = normalize(Sposition - input.worldPos);
		float surfRatio = saturate(dot(-SlightDir, normalize(Sdirection)));
	float spotFactor = (surfRatio > spotRatio.y) ? 1.0f : 0.0f;
	float LightRatio = saturate(dot(SlightDir, MapVal));
	float4 finalSpotlight = spotFactor * LightRatio * Scolor * surfColor;

		float4 finalColor = saturate(dirLight + finalPointLight + finalSpotlight + ambient);

		return finalColor;
}