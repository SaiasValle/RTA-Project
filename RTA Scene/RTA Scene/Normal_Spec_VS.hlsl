#pragma pack_matrix(row_major)
struct INPUT_VERTEX
{
	float4 coordinate : POSITION;
	float4 color : COLOR;
	float2 uv : UV;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 camera : COORD;
};

struct OUTPUT_VERTEX
{
	float4 projectedCoordinate : SV_POSITION;
	float2 uvOut : UV;
	float3 normOut : NORMAL;
	float3 tanOut : TANGENT;
	float3 biTanOut: BITAN;
	float4 camPos : COORD;
	float4 worldPos : WORLDPOS;
};

cbuffer ROTATE : register(b0)
{
	float4x4 worldMatrix;
};

cbuffer VIEW : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};

OUTPUT_VERTEX main( INPUT_VERTEX fromVertexBuffer )
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;
	float4 position = mul(fromVertexBuffer.coordinate, worldMatrix);
	sendToRasterizer.worldPos = position;
	position = mul(position, viewMatrix);
	position = mul(position, projectionMatrix);
	
	sendToRasterizer.projectedCoordinate.xyzw = position;	
	sendToRasterizer.colorOut = fromVertexBuffer.color;
	sendToRasterizer.uvOut = fromVertexBuffer.uv;
	float3 localN = mul(fromVertexBuffer.normal, (float3x3)worldMatrix);
	sendToRasterizer.normOut = localN;
	sendToRasterizer.camPos = fromVertexBuffer.camera;

	float4 ltan = fromVertexBuffer.tangent;
		ltan = mul(float4(ltan.xyz*ltan.w, 0.0f), worldMatrix);
	sendToRasterizer.tanOut = ltan;
	sendToRasterizer.biTanOut = float4(cross(localN, ltan), 0.0f);
	return sendToRasterizer;
}