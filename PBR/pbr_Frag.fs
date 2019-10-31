#version 330

out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

//Tex
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

uniform vec3 l_Positions[4];
uniform vec3 l_Colors[4];

uniform vec3 camPos;

const float PI = 3.141592653589;

vec3 GetNormals(){
	vec3 tangentNorm = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;
	vec3 q1 = dFdx(WorldPos);
	vec3 q2 = dFdy(WorldPos);
	vec2 st1 = dFdx(TexCoords);
	vec2 st2 = dFdy(TexCoords);
	vec3 N = normalize(Normal);
	vec3 T = normalize(q1 * st2.t - q2*st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);
	return normalize(TBN * tangentNorm);
}

float DistributionGGX(vec3 n, vec3 h, float r){
	float a = r * r;
	float a2 = a * a;
	float NdotH = max(dot(n, h), 0.0);
	float NdotH2 = NdotH * NdotH;
	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	return nom / denom;
}

float GeoSchlickGGX(float ndv, float r){
	float rough = (r + 1.0);
	float k = (rough * rough) / 8.0;
	float nom = ndv;
	float denom = ndv * (1.0 - k) + k;
	return nom / denom;
}

float GeoSmith(vec3 n, vec3 v, vec3 l, float r){
	float ndotv = max(dot(n, v), 0.0);
	float ndotl = max(dot(n, l), 0.0);
	float ggx2 = GeoSchlickGGX(ndotv, r);
	float ggx1 = GeoSchlickGGX(ndotl, r);
	return ggx1 * ggx2;
}

vec3 FresnelSchlick(float ct, vec3 f0){
	return f0 + (1.0 - f0) * pow(1.0-ct, 5.0);
}

void main(){
	vec3 albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
	float metallic = texture(metallicMap, TexCoords).r;
	float roughness = texture(roughnessMap, TexCoords).r;
	float ao = texture(aoMap, TexCoords).r;

	vec3 n = GetNormals();
	vec3 v = normalize(camPos - WorldPos);

	vec3 f0 = vec3(0.04);
	f0 = mix(f0, albedo, metallic);

	vec3 lo = vec3(0.0);
	for(int i = 0; i < 4;i++){
		vec3 l = normalize(l_Positions[i] - WorldPos);
		vec3 h = normalize(v + l);
		float dist = length(l_Positions[i] - WorldPos);
		float atten = 1.0 / (dist * dist);
		vec3 rad = l_Colors[i] * atten;

		//Cook-Torrance BRDF
		float ndf = DistributionGGX(n, h, roughness);
		float g = GeoSmith(n, v, l, roughness);
		vec3 f = FresnelSchlick(max(dot(h, v), 0.0), f0);

		vec3 nominator = ndf * g * f;
		float denominator = 4.0 * max(dot(n, v), 0.0) * max(dot(n, l), 0.0) + 0.001; //late addition of decimal to prevent /0
		vec3 spec = nominator / denominator;

		//ks is equal to Fresnel
		vec3 ks = f;
		// for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kd = vec3(1.0) - ks;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kd *= 1.0 - metallic;	
		float ndotl = max(dot(n, l), 0.0);
		lo += (kd * albedo / PI + spec) * rad * ndotl;
	}
	vec3 ambient = vec3(0.03) * albedo * ao;
	vec3 color = ambient * lo;
	color = color / (color+vec3(1.0));
	color = pow(color, vec3(1.0/2.2));
	FragColor = vec4(color, 1.0);
}