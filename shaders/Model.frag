#version 460 core

in vec3 frag_normal;
in vec3 frag_position;
in vec2 frag_texcoord;

out vec4 FragColor;

uniform sampler2D tex_albedo;
uniform sampler2D tex_emission;
uniform sampler2D tex_metal_rough;
uniform sampler2D tex_normal;
uniform sampler2D tex_occlusion;

uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

uniform vec3 camPos;

const float PI = 3.14159265359;

// Простой трюк для получения касательного вектора и нормалей в мировом пространстве для упрощения кода PBR
vec3 getNormalFromMap()
{
	vec3 tangentNormal = texture(tex_normal, frag_texcoord).xyz * 2.0 - 1.0;

	vec3 Q1  = dFdx(frag_position);
	vec3 Q2  = dFdy(frag_position);
	vec2 st1 = dFdx(frag_texcoord);
	vec2 st2 = dFdy(frag_texcoord);

	vec3 N   = normalize(frag_normal);
	vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B  = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;

	float nom   = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float nom   = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() {
	/*vec4 emission = texture(tex_emission, frag_texcoord);
	vec3 albedo = pow(texture(tex_albedo, frag_texcoord).rgb, vec3(2.2));
	// according to GLTF specification
	float metallic = texture(tex_metal_rough, frag_texcoord).b;
	float roughness = texture(tex_metal_rough, frag_texcoord).g;
	float ao = texture(tex_occlusion, frag_texcoord).r;

	vec3 N = getNormalFromMap();
	vec3 V = normalize(camPos - frag_position);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0);
	for(int i = 0; i < 4; ++i) {
		vec3 L = normalize(lightPositions[i] - frag_position);
		vec3 H = normalize(V + L);
		float distance = length(lightPositions[i] - frag_position);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lightColors[i] * attenuation;

		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 nominator = NDF * G * F;
		float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
		vec3 specular = nominator / denominator;

		vec3 kS = F;

		vec3 kD = vec3(1.0) - kS;

		kD *= 1.0 - metallic;
		float NdotL = max(dot(N, L), 0.0);
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}

	vec3 ambient = vec3(0.03) * albedo * ao;
	vec3 color = ambient + Lo + emission.rgb;

	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2));
	FragColor = vec4(color, 1.0);
	*/

	FragColor = texture(tex_albedo, frag_texcoord);
}