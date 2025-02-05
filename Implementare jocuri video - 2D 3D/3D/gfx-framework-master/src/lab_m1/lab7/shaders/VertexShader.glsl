#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Uniforms for light properties
uniform vec3 light_position;
uniform vec3 eye_position;
uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform vec3 object_color;

// Output value to fragment shader
out vec3 color;

void main()
{
    // Compute world space vectors
    vec3 world_position = vec3(Model * vec4(v_position, 1.0));
    vec3 N = normalize(mat3(transpose(inverse(Model))) * v_normal);

    vec3 L = normalize(light_position - world_position);
    vec3 V  = normalize(eye_position - world_position);
    vec3 H = normalize(L + V);

    // Define ambient light component
    float ambient_light = 0.25;
    vec3 ambient_color = ambient_light * object_color;

    // Compute diffuse light component
    float diffuse_light = max(dot(N, L), 0.0);
    vec3 diffuse_color = diffuse_light * object_color * material_kd;

    // Compute specular light component
    vec3 R = reflect(-L, N);
    float specular_light = 0;

    // It's important to distinguish between "reflection model" and
    // "shading method". In this shader, we are experimenting with the Phong
    // (1975) and Blinn-Phong (1977) reflection models, and we are using the
    // Gouraud (1971) shading method. There is also the Phong (1975) shading
    // method, which we'll use in the future. Don't mix them up!
    if (diffuse_light > 0)
    {
        specular_light = pow(max(dot(V , R), 0.0), material_shininess);
    }
    vec3 specular_color = material_ks * specular_light * object_color;

    // Compute light
    float dist = length(light_position - world_position);
    vec3 color_f = ambient_color + (diffuse_color + specular_color) * (1/(1 + dist * dist)) ;

    // Send color light output to fragment shader
    color = color_f;

    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}