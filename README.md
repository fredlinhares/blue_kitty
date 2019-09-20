# BlueKitty

BlueKitty is a minimal 3D game engine for the Ruby language. It intends to be a middleware between your code and system running it.


This project is a work in progress; currently, it is not suitable for any serious project.

## Installation

This gem links against SDL2, Vulkan and GLM, so you need the development libraries installed on your system before installing this gem. Also, you need the GLSL compiler.

For Debian/Ubuntu systems use:

    $ sudo apt install libvulkan-dev libsdl2-dev libglm-dev glslang-tools

Add this line to your application's Gemfile:

```ruby
gem 'blue_kitty'
```

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install blue_kitty

## Usage

For an example of how to use BlueKitty and its features, see this: https://github.com/fredlinhares/bkge-demo.

For importing meshes to BlueKitty, you can use this: https://github.com/fredlinhares/bkge-import_mesh.

## Contributing

Bug reports and pull requests are welcome on GitHub at https://github.com/fredlinhares/blue_kitty.

## License

The gem is available as open source under the terms of the [MIT License](https://opensource.org/licenses/MIT).
