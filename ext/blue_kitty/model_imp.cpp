// SPDX-License-Identifier: MIT
#include "model.h"
#include "model_imp.hpp"

#include <array>
#include <fstream>

#include <glm/glm.hpp>

#include "engine.h"
#include "engine_imp.hpp"
#include "vk_vertex.hpp"

namespace
{
uint32_t read_uint32_from_file(std::ifstream &input_file)
{
  uint32_t data{};
  input_file.read((char*)&data, sizeof(uint32_t));
  return data;
}

glm::vec3 read_vec3_from_file(std::ifstream &input_file)
{
  glm::vec3 data{};
  input_file.read((char*)&data.x, sizeof(glm::vec3::value_type));
  input_file.read((char*)&data.y, sizeof(glm::vec3::value_type));
  input_file.read((char*)&data.z, sizeof(glm::vec3::value_type));
  return data;
}
}

VALUE bk_cModel;

/*
  Basic functions all Ruby classes need.
 */

void
bk_free_model(void* obj);

size_t
bk_memsize_model(const void* obj);

static const rb_data_type_t
bk_model_type = {
  "blue_kitty_model",
  {0, bk_free_model, bk_memsize_model,},
  0, 0,
  RUBY_TYPED_FREE_IMMEDIATELY,
};

VALUE
bk_alloc_model(VALUE klass)
{
  VALUE obj;
  struct bk_model_data *ptr;

  ptr = new bk_model_data{};
  ptr->loader = new Loader::Stack<bk_model_data>{ptr};
  obj = TypedData_Wrap_Struct(klass, &bk_model_type, ptr);

  return obj;
}

void
bk_free_model(void* obj)
{
  struct bk_model_data *ptr;
  ptr = static_cast<bk_model_data*>(obj);
  ptr->loader->unload();

  delete ptr->loader;
  delete ptr;
}

size_t
bk_memsize_model(const void* obj)
{
  // TODO
  return 0;
}

/*
  External interface.
*/

VALUE
bk_cModel_initialize(VALUE self, VALUE file_path)
{
  if(!engine_inilialized)
    rb_raise(rb_eRuntimeError, "%s",
             "Can not create a BlueKitty::Model instance before "
             "BlueKitty::Engine is started");

  SafeStringValue(file_path);

  struct bk_model_data *ptr;
  TypedData_Get_Struct(self, struct bk_model_data, &bk_model_type, ptr);

  ptr->model_path = StringValueCStr(file_path);

  ptr->loader->add(&bk_model_data::load_mesh, &bk_model_data::unload_mesh);
  ptr->loader->add(&bk_model_data::load_uniform_buffers,
                   &bk_model_data::unload_uniform_buffers);
  ptr->loader->add(&bk_model_data::load_descriptor_sets,
                   &bk_model_data::unload_descriptor_sets);

  try
  {
    ptr->loader->load();
  }
  catch(Loader::Error le)
  {
    std::string base_error{"Failed to load model → "};
    base_error += le.message;
    rb_raise(rb_eRuntimeError, "%s\n", base_error.c_str());
  }

  return self;
}

void
bk_model_data::load_mesh()
{
  std::ifstream input_file{this->model_path};
  if(!input_file.is_open()) throw Loader::Error{"Failed to open file."};

  std::vector<bk_sMesh> meshes{};
  size_t vertexes_size;

  std::shared_ptr<BKVK::QueueFamily> queue_family{
    BKGE::engine->get_queues_families_with_graphics()[0]};

  // Load meshes.
  {
    uint32_t meshes_count{read_uint32_from_file(input_file)};
    meshes.resize(meshes_count);

    for(uint32_t i{0}; i < meshes_count; i++)
    {
      meshes[i].color = read_vec3_from_file(input_file);

      meshes[i].vertex_base = read_uint32_from_file(input_file);
      meshes[i].vertex_count = read_uint32_from_file(input_file);
      meshes[i].index_base = read_uint32_from_file(input_file);
      meshes[i].index_count = read_uint32_from_file(input_file);
    }
  }

  // Load vertexes.
  {
    uint32_t vertex_count{read_uint32_from_file(input_file)};
    std::vector<BKVK::Vertex> vertexes{vertex_count};

    for(auto mesh: meshes)
    {
      for(uint32_t i{mesh.vertex_base}; i < mesh.vertex_count; i++)
      {
        vertexes[i].position = read_vec3_from_file(input_file);
        vertexes[i].normal = read_vec3_from_file(input_file);
        vertexes[i].color = mesh.color;
      }
    }

    void *vertexes_data{vertexes.data()};
    vertexes_size = sizeof(vertexes[0]) * vertexes.size();
    std::shared_ptr<BKVK::SourceBuffer> source_vertex_buffer{
      new BKVK::SourceBuffer{
        queue_family->get_device(), vertexes_data, vertexes_size}};
    this->vertex_buffer = std::make_shared<BKVK::DestinationBuffer>(
        queue_family, source_vertex_buffer, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
  }

  // Load indexes.
  {
    this->index_count = read_uint32_from_file(input_file);
    std::vector<uint32_t> indexes(this->index_count);

    for(uint32_t i{0}; i < this->index_count; i++)
    {
      indexes[i] = read_uint32_from_file(input_file);
    }

    void *indexes_data{indexes.data()};
    size_t indexes_size{sizeof(indexes[0]) * indexes.size()};
    std::shared_ptr<BKVK::SourceBuffer> source_index_buffer{
      new BKVK::SourceBuffer{
        queue_family->get_device(), indexes_data, vertexes_size}};
    this->index_buffer = std::make_shared<BKVK::DestinationBuffer>(
        queue_family, source_index_buffer, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
  }
}

void
bk_model_data::unload_mesh()
{
  this->index_buffer = nullptr;
  this->vertex_buffer = nullptr;
}

void
bk_model_data::load_uniform_buffers()
{
  for(std::vector<std::shared_ptr<BKVK::UniformBuffer>>::size_type i{0};
      i < BKGE::engine->get_swapchain()->get_vk_image_views().size(); i++)
  for(int i{0}; i < 3; i++)
    this->ub_model_instance.push_back(std::make_shared<BKVK::UniformBuffer>(
        BKGE::engine->get_devices()[0], sizeof(BKVK::UBOModelInstance)));
}

void
bk_model_data::unload_uniform_buffers()
{
  this->ub_model_instance.clear();
}

void
bk_model_data::load_descriptor_sets()
{
  this->ds_model_instance = std::make_shared<BKVK::DS::ModelInstance>(
      BKGE::engine->get_graphic_pipeline_layout()->get_dsl_model_instance(),
      this->ub_model_instance);
}

void
bk_model_data::unload_descriptor_sets()
{
  this->ds_model_instance = nullptr;
}

void
bk_model_data::draw(VkCommandBuffer vk_command_buffer,
                    uint32_t image_index,
                    uint32_t instance_count,
                    VkPipeline vk_graphic_pipeline,
                    VkDescriptorSet vk_ds_view_projection,
                    VkPipelineLayout vk_pipeline_layout)
{
  std::array<VkDescriptorSet, 2> vk_descriptor_sets{
    this->ds_model_instance->get_vk_descriptor_sets()[image_index],
    vk_ds_view_projection};
  VkBuffer vertex_buffers[]{this->vertex_buffer->get_vk_buffer()};
  VkDeviceSize offsets[]{0};

  vkCmdBindDescriptorSets(
      vk_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_pipeline_layout,
      0, vk_descriptor_sets.size(), vk_descriptor_sets.data(), 0, nullptr);
  vkCmdBindPipeline(
      vk_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_graphic_pipeline);
  vkCmdBindVertexBuffers(vk_command_buffer, 0, 1, vertex_buffers, offsets);
  vkCmdBindIndexBuffer(
      vk_command_buffer, this->index_buffer->get_vk_buffer(), 0,
      VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(
      vk_command_buffer, this->index_count, instance_count, 0, 0, 0);
}

struct bk_model_data*
bk_cModel_get_data(VALUE self)
{
  struct bk_model_data *ptr;

  TypedData_Get_Struct(self, struct bk_model_data, &bk_model_type, ptr);

  return ptr;
}
