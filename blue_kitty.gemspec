lib = File.expand_path("../lib", __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require "blue_kitty/version"

Gem::Specification.new do |spec|
  spec.name          = "blue_kitty"
  spec.version       = BlueKitty::VERSION
  spec.authors       = ["Frederico Linhares"]
  spec.email         = ["fred@linhares.blue"]

  spec.summary       = %q{A 3D game engine for Ruby.}
  spec.homepage      = "http://projects.linhares.blue/kitty"
  spec.license       = "MIT"

  # Specify which files should be added to the gem when it is released.
  # The `git ls-files -z` loads the files in the RubyGem that have been added
  # into git.
  spec.files         = Dir.chdir(File.expand_path('..', __FILE__)) do
    `git ls-files -z`.split("\x0").reject do |f|
      f.match(%r{^(test|spec|features)/})
    end
  end
  spec.bindir        = "exe"
  spec.executables   = spec.files.grep(%r{^exe/}) { |f| File.basename(f) }
  spec.require_paths = ["lib"]
  spec.extensions    = ["ext/blue_kitty/extconf.rb"]
  spec.files         = [
    "Rakefile",
    "blue_kitty.gemspec",
    "Gemfile",
    "LICENSE.txt",
    "README.md",
    "bin/console",
    "bin/setup",
    "lib/blue_kitty.rb",
    "lib/blue_kitty/blue_kitty.so",
    "lib/blue_kitty/camera.rb",
    "lib/blue_kitty/controller.rb",
    "lib/blue_kitty/engine.rb",
    "lib/blue_kitty/entity3d.rb",
    "lib/blue_kitty/version.rb",
    "data/blue_kitty/GLSL/vert.spv",
    "data/blue_kitty/GLSL/frag.spv"
  ]

  spec.add_development_dependency "bundler", "~> 2.0"
  spec.add_development_dependency "rake", "~> 13.0"
  spec.add_development_dependency "rake-compiler"
end
