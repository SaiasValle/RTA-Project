struct INPUT_VERTEX
{
	float4 coordinate : POSITION;
	float4 color : COLOR;
	float2 uv : UV;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
};
struct OUTPUT_VERTEX
{
	float4 coord : SV_POSITION;
	float3 x_axis : X_AXIS;
	float3 y_axis : Y_AXIS;
	float3 z_axis : Z_AXIS;
};

cbuffer ROTATE : register(b0)
{
	float4x4 worldMatrix;
};

cbuffer VIEW : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projMatrix;
};

OUTPUT_VERTEX main(INPUT_VERTEX fromVertexBuffer)
{
	OUTPUT_VERTEX sendToGeometry = (OUTPUT_VERTEX)0;
	sendToGeometry.coord = float4(fromVertexBuffer.coordinate.xyz, 1);
	sendToGeometry.x_axis = float4(fromVertexBuffer.normal.xyz, 0);
	sendToGeometry.y_axis = float4(fromVertexBuffer.tangent.xyz, 0);
	sendToGeometry.z_axis = cross(sendToGeometry.x_axis, sendToGeometry.y_axis);
	return sendToGeometry;
}