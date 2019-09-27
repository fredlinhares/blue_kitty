#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_frag_color;
layout(location = 1) in vec2 in_frag_texture_coord;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 1) uniform sampler2D texture_sampler;

void main()
{
  out_color = texture(texture_sampler, in_frag_texture_coord);
}
