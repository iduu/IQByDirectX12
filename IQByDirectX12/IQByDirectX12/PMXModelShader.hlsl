Texture2D<float4> materialNormalTexture : register(t0);   // マテリアル通常テクスチャ
Texture2D<float4> materialSphereTexture : register(t1); // マテリアルスフィアテクスチャ

SamplerState smp : register(s0);  

cbuffer mat : register(b0)
{
    float4x4 wvp;           // wvp行列
    float4x4 world;         // world行列
    float4x4 view;          // view行列
    float4x4 projection;    // projection行列	
}

cbuffer material : register(b1)
{
    float3 diffuseColor;
    float alpha;
    float specularity;
    float3 specularColor;
    float3 ambientColor;
}

cbuffer boneMatrix : register(b2)
{
	float4x4 boneMatrix[256];
}

struct VSInput
{
    float3 position : POSITION0;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4x4 appendUV : APPEND_UV;
	uint deformType : DEFORM_TYPE;
	uint4 boneIndex : BONE_INDEX;
	float4 boneWeight : BONE_WEIGHT;
    float4x4 modelMatrix : INSTANCE_MATRIX;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 origPosition : POSITION1;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
	float4 color : COLOR;
};

typedef VSOutput PSInput;

VSOutput VSMain(VSInput input)
{
    VSOutput output;

	uint boneIndex = input.boneIndex.x;
	float4x4 localBoneMatrix = boneMatrix[boneIndex];
    
    // BDEF1
    if(input.deformType == 0)
    {
        localBoneMatrix = boneMatrix[boneIndex];
    }
    else if(input.deformType == 1)
    {
        float bone1Weight = input.boneWeight.x;
        localBoneMatrix = boneMatrix[input.boneIndex.x] * bone1Weight + boneMatrix[input.boneIndex.y] * (1.0f - bone1Weight);
    }
    else if(input.deformType == 2)
    {

    }

    output.position = mul(wvp, mul(input.modelMatrix, mul(localBoneMatrix, float4(input.position, 1.0f))));
    output.origPosition = mul(world, mul(input.modelMatrix, float4(input.position, 1.0f)));

    input.modelMatrix._14_24_34 = float3(0.0f, 0.0f, 0.0f);
    output.normal = normalize(mul(input.modelMatrix, float4(input.normal, 1.0f)).xyz);

    output.uv = input.uv;
    
    output.color = float4(0.0f, 0.0f, 1.0f, 1.0f);
    if(input.deformType == 0)
    {
        output.color = float4(1.0f, 0.0f, 0.0f, 1.0f);
    } 
    else if(input.deformType == 1)
    {
        output.color = float4(0.0f, 1.0f, 0.0f, 1.0f);
    }

    return output;
}


float4 PSMain(PSInput input) : SV_Target
{
    //return input.color;
	float3 light = normalize(float3(0.0f, 1.0f, -1.0f));
    float3 lightSpecularColor = float3(1.0f, 1.0f, 1.0f);
    float3 lightDiffuseColor = float3(1.0f, 1.0f, 1.0f);
    float3 lightAmbientColor = float3(1.0f, 1.0f, 1.0f);

    float3 eyePosition = float3(0.0f, 10.0f, -15.0f);

    float3 vray = normalize(input.origPosition - eyePosition);
	
    float brightness = dot(input.normal, light);
    
    float attenuation = 1.0f;

	/// アンビエントカラー計算
    float3 modelAmbientColor = ambientColor * attenuation;
    modelAmbientColor.x = modelAmbientColor.x * lightAmbientColor.x;
    modelAmbientColor.y = modelAmbientColor.y * lightAmbientColor.y;
    modelAmbientColor.z = modelAmbientColor.z * lightAmbientColor.z;

	// ディフューズカラー計算
    float3 modelDiffuseColor = diffuseColor * brightness;
    modelDiffuseColor.x = modelDiffuseColor.x * lightDiffuseColor.x;
    modelDiffuseColor.y = modelDiffuseColor.y * lightDiffuseColor.y;
    modelDiffuseColor.z = modelDiffuseColor.z * lightDiffuseColor.z;

	// スペキュラ計算
    float3 modelSpecularColor = specularColor * attenuation;
    float spec = saturate(pow(dot(reflect(-light, input.normal), -vray), specularity));
    modelSpecularColor = modelSpecularColor * spec;

    
    float4 texColor = materialNormalTexture.Sample(smp, input.uv);

    float4 modelColor = (float4(modelAmbientColor, 0.0f) + float4(modelDiffuseColor, alpha));
    modelColor = modelColor * texColor;


    modelColor = modelColor +float4(modelSpecularColor, 1.0f);

    return modelColor;
}