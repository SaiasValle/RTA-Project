Texture2D Texture : register(t0);
//SamplerState Sample : register(s0);
struct INPUT_VERTEX
{
	float4 projectedCoordinate : SV_POSITION;
	float2 uvOut : UV;
};
float4 main(INPUT_VERTEX input) : SV_TARGET
{
	return float4(1, 0, 1, 0);//Texture.Sample(Sample, input.uvOut);
}