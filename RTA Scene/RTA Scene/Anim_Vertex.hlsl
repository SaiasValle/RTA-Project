#pragma pack_matrix(row_major)
struct INPUT_VERTEX
{
	float4 coordinate : POSITION;
	float2 uv : UV;
	int4 BoneIDs : BONEID;
	float4 BoneWeights : BONEWEIGHT;
};

struct OUTPUT_VERTEX
{
	float4 projectedCoordinate : SV_POSITION;
	float2 uvOut : UV;
};

cbuffer ROTATE : register(b0)
{
	float4x4 worldMatrix[52];
};

cbuffer VIEW : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};
cbuffer INVERSE : register(b2)
{
	float4x4 bindInverse[52];
}
OUTPUT_VERTEX main(INPUT_VERTEX input)
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;
	float4 position = input.coordinate;
		float4x4 boneSpace = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		float4x4 boneInvert = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		//	boneSpace = boneInvert = Zero;
		int index;
	for (int i = 0; i < 4; ++i)
	{
		index = input.BoneIDs[i];
		if (index == -1)
			index = 0;
		boneSpace += (input.BoneWeights[i] * worldMatrix[index]);
		boneInvert += (input.BoneWeights[i] * bindInverse[index]);
	}
	position = mul((float4)position, boneInvert);
	position = mul((float4)position, boneSpace);
	position = mul((float4)position, viewMatrix);
	position = mul((float4)position, projectionMatrix);
	sendToRasterizer.projectedCoordinate = position;
	sendToRasterizer.uvOut = input.uv;
	//sendToRasterizer.BoneIDs = input.BoneIDs;

	return sendToRasterizer;
}