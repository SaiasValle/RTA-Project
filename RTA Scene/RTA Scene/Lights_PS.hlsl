Texture2D Texture : register(t0);
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
	float4 color : COLOR;
	float2 uv : UV;
	float3 normal : NORMAL;
	float4 worldPos : WORLDPOS;
	float4 tangent : TANGENT;
};

float4 main(INPUT_PIXEL input) : SV_TARGET
{
	float4 surfColor = Texture.Sample(Sample, input.uv);
	// Directional Light
	float4 DlightDirNorm = -normalize(direction);
	float4 lightColor = surfColor * Dcolor;
	float3 norm = normalize(input.normal);
	float4 dirLight = saturate(dot(DlightDirNorm, norm) * lightColor);

	float4 specularDir = normalize(cameraPos - input.coordinate);
	float4 halfvec = normalize(-DlightDirNorm * specularDir);
	float4 intensity = saturate(dot(input.normal, normalize(halfvec)));
	dirLight = dirLight * intensity;

	// Point Light
	float4 lightDir = normalize(position - input.worldPos);
	float lightRatio = saturate(dot(lightDir, norm));
	float4 finalPointLight = lightRatio * Pcolor * surfColor;

	// Spot Light
	float4 SlightDir = normalize(Sposition - input.worldPos);
	float surfRatio = saturate(dot(-SlightDir, normalize(Sdirection)));
	float spotFactor = (surfRatio > spotRatio.y) ? 1.0f : 0.0f;
	float LightRatio = saturate(dot(SlightDir, norm));
	float4 finalSpotlight = spotFactor * LightRatio * Scolor * surfColor;

	float4 finalColor = saturate(dirLight + finalPointLight + finalSpotlight);

	return saturate(finalColor);
}