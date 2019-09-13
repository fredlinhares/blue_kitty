#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_color;

layout(location = 0) out vec3 frag_color;

layout(set = 0, binding = 0) uniform UBOModelInstance
{
  mat4 model[128];
} ubo_model_instance;

layout(set = 1, binding = 0) uniform UBOViewProjection
{
  mat4 view;
  mat4 proj;
} ubo_view_projection;

void main()
{
  gl_Position =
      ubo_view_projection.proj * ubo_view_projection.view *
      ubo_model_instance.model[gl_InstanceIndex] * vec4(in_position, 1.0);
  frag_color = in_color;
}
