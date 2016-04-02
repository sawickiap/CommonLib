float4 g_Color;
float4x4 g_WorldViewProj;
float3 g_DirToLight; // Local Space

struct VS_INPUT
{
	float4 Pos : POSITION;
	float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float4 Color : COLOR0;
};


void MainVS(VS_INPUT In, out VS_OUTPUT Out)
{
	Out.Pos = mul(In.Pos, g_WorldViewProj);
	Out.Color = g_Color;
	
	float PseudoDiffuse = abs(dot(g_DirToLight, In.Normal));
	float PseudoSpecular = saturate(PseudoDiffuse * 5 - 4);
	Out.Color.rgb *= PseudoDiffuse * 0.5 + 0.5;
	Out.Color.rgb += float3(PseudoSpecular, PseudoSpecular, PseudoSpecular);
}

void MainPS(VS_OUTPUT In, out float4 Out : COLOR0)
{
	Out = In.Color;
}


technique {
	pass {
		VertexShader = compile vs_3_0 MainVS();
		PixelShader = compile ps_3_0 MainPS();
		CullMode = NONE;
	}
}
