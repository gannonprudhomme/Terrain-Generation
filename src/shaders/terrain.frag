#version 420 core

//vec3 getNormal();

out vec4 fragColor;

in DATA {
	vec3 position;
	vec3 surfaceNormal;
	vec3 toLightVector;
	vec3 toCameraVector;
	vec2 texCoords;
} fs_in;

uniform sampler2D tex;
uniform sampler2D normalMap;
uniform sampler2D heightMap;
uniform vec3 lightColor;
uniform float shineDamper = 1;
uniform float reflectivity = 0.0;

void main() {
	// normalize into vector space
	/*
	float r = texture(normalMap, fs_in.texCoords / 40).r * 2.0 - 1.0;
	float g = texture(normalMap, fs_in.texCoords / 40).g * 2.0 - 1.0;
	float b = texture(normalMap, fs_in.texCoords / 40).b * 2.0 - 1.0;
	float a = texture(normalMap, fs_in.texCoords / 40).a * 2.0 - 1.0;
	*/

	vec3 normal = fs_in.surfaceNormal;
	
	float r = normal.x;
	float g = normal.y;
	float b = normal.z;
	float a = 1; 

	// Light calculations
	vec3 unitNormal = vec3(r, g, b);
	vec3 unitLightVector = normalize(fs_in.toLightVector);

	float nDotl = dot(unitNormal, unitLightVector);
	float brightness = max(nDotl, 0.2);
	vec3 diffuse = brightness * lightColor;
	
	vec3 unitVectorToCamera = normalize(fs_in.toCameraVector);
	vec3 lightDirection = -unitLightVector;
	vec3 reflectedLightDirection = reflect(lightDirection, unitNormal);

	float specularFactor = dot(reflectedLightDirection, unitVectorToCamera);
	specularFactor = max(specularFactor, 0.0);
	float dampedFactor = pow(specularFactor, shineDamper);
	vec3 finalSpecular = dampedFactor * reflectivity * lightColor;

	//fragColor = vec4(brightness) * vec4(lightColor, 1.0) * texture(tex, fs_in.texCoords);
	fragColor = vec4(diffuse, 1.0) * texture(tex, fs_in.texCoords) + vec4(finalSpecular, 1.0);
	//fragColor = texture(heightMap, fs_in.texCoords);
	//fragColor = vec4(r, g, b, a);
}

/*
vec3 getNormal() {
	float offset = 1.0 / 128.0;
	float hL = texture2D(tex, fs_in.texCoords + vec2(-offset, 0)).x;
	float hR = texture2D(tex, fs_in.texCoords + vec2(offset, 0)).x;
	float hD = texture2D(tex, fs_in.texCoords + vec2(0, -offset)).x;
	float hU = texture2D(tex, fs_in.texCoords + vec2(0, offset)).x;

	vec3 n = vec3(hL - hR, 2.0f, hD - hU);
	
	return normalize(n);
} */

/*
vec3 getNormal() {
	float a = texture2D(tex, fs_in.texCoords).x;
	float b = texture2D(tex, fs_in.texCoords + ivec2(1, 0)).x;
	float c = texture2D(tex, fs_in.texCoords + ivec2(0, 1)).x;

	vec3 n = vec3(b - a, c - a, 0.0);
	
	return normalize(n);
} */	

// Would be better if we calculated normals once and reused, but this will work for now

/*
vec3 getNormal() {
	const vec2 size = vec2(2.0, 0.0); // what does this do?
	const ivec3 offset = ivec3(-1, 0, 1);

	//vec3 t = texture(tex, fs_in.texCoords);
	float s01 = textureOffset(heightMap, fs_in.texCoords, offset.xy).x;
	float s21 = textureOffset(heightMap, fs_in.texCoords, offset.zy).x;
    float s10 = textureOffset(heightMap, fs_in.texCoords, offset.yx).x;
    float s12 = textureOffset(heightMap, fs_in.texCoords, offset.yz).x;
	vec3 va = normalize(vec3(size.xy, s21 - s01));
	vec3 vb = normalize(vec3(size.yx, s12 - s10));

	return cross(va, vb);
} */