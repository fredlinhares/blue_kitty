require "bundler/gem_tasks"
require "rake/extensiontask"

task :build => :compile

task :glsl do
  system('glslangValidator -V data/blue_kitty/GLSL/shader.vert -o '\
         'data/blue_kitty/GLSL/vert.spv') and
    system('glslangValidator -V data/blue_kitty/GLSL/shader.frag -o '\
           'data/blue_kitty/GLSL/frag.spv')
end

Rake::ExtensionTask.new("blue_kitty") do |ext|
  ext.lib_dir = "lib/blue_kitty"
end

task :default => [:glsl, :clobber, :compile]
