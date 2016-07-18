#pragma pack_matrix(row_major)
struct GS_Input
{
	float4 coord : SV_POSITION;
	float3 x_axis : X_AXIS;
	float3 y_axis : Y_AXIS;
	float3 z_axis : Z_AXIS;
};
struct GS_Output
{
	float4 pos : SV_POSITION;
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

//3 points per triangle, 2 triangles per face, 3*2*6 = 36
[maxvertexcount(36)]
void main(point GS_Input input[1], inout TriangleStream< GS_Output > output)
{
	//four actual points per face, 4*6
	//positions only
	GS_Output act[8];
	//top of box;
	act[0].pos = float4(-0.1f, 0.1f, 0.1f, input[0].coord.w); //front left
	act[1].pos = float4(0.1f, 0.1f, 0.1f, input[0].coord.w); //front right
	act[2].pos = float4(-0.1f, 0.1f, -0.1f, input[0].coord.w); //back left
	act[3].pos = float4(0.1f, 0.1f, -0.1f, input[0].coord.w); //back right
	//bottom of box
	act[4].pos = float4(-0.1f, -0.1f, 0.1f, input[0].coord.w); //front left
	act[5].pos = float4(0.1f, -0.1f, 0.1f, input[0].coord.w); //front right
	act[6].pos = float4(-0.1f, -0.1f, -0.1f, input[0].coord.w); //back left
	act[7].pos = float4(0.1f, -0.1f, -0.1f, input[0].coord.w); //back right

	//Joint positions and rotation information
	//used to form a local space matric for the box
	float4x4 BoxSpace;
	BoxSpace[0] = float4(input[0].x_axis, 0);
	BoxSpace[1] = float4(input[0].y_axis, 0);
	BoxSpace[2] = float4(input[0].z_axis, 0);
	BoxSpace[3] = float4(input[0].coord.xyz, 1);
	for (int i = 0; i < 8; ++i)
	{
		act[i].pos = mul(float4(act[i].pos), BoxSpace);
		act[i].pos = mul(float4(act[i].pos), worldMatrix);
		act[i].pos = mul(float4(act[i].pos), viewMatrix);
		act[i].pos = mul(float4(act[i].pos), projMatrix);
	}
	//Front Face - checked
	output.Append(act[0]);
	output.Append(act[5]);
	output.Append(act[4]);
	output.RestartStrip();
	output.Append(act[0]);
	output.Append(act[1]);
	output.Append(act[5]);
	output.RestartStrip();
	//right face - checked
	output.Append(act[1]);
	output.Append(act[7]);
	output.Append(act[5]);
	output.RestartStrip();
	output.Append(act[1]);
	output.Append(act[3]);
	output.Append(act[7]);
	output.RestartStrip();
	//back face - checked
	output.Append(act[3]);
	output.Append(act[6]);
	output.Append(act[7]);
	output.RestartStrip();
	output.Append(act[3]);
	output.Append(act[2]);
	output.Append(act[6]);
	output.RestartStrip();
	//left face - checked
	output.Append(act[2]);
	output.Append(act[4]);
	output.Append(act[6]);
	output.RestartStrip();
	output.Append(act[2]);
	output.Append(act[0]);
	output.Append(act[4]);
	output.RestartStrip();
	//top face - checked
	output.Append(act[2]);
	output.Append(act[1]);
	output.Append(act[0]);
	output.RestartStrip();
	output.Append(act[2]);
	output.Append(act[3]);
	output.Append(act[1]);
	output.RestartStrip();
	//bottom face - checked
	output.Append(act[4]);
	output.Append(act[7]);
	output.Append(act[6]);
	output.RestartStrip();
	output.Append(act[4]);
	output.Append(act[5]);
	output.Append(act[7]);
	output.RestartStrip();
}