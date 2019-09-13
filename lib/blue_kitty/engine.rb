# SPDX-License-Identifier: MIT
require 'yaml'

module BlueKitty
  module Engine
    # Load values from a yaml file and save it into @@configurations. This
    # variable is used by {Engine#load}. Raise an error if there is something
    # wrong or missing in the yaml file. The yaml file must contain the
    # following information:
    #
    # - debug: a boolean value, if true the engine will start in debug mode.
    # - max_fps: must be an Integer bigger than zero,
    # - game_name: must be a String. This value is used by Vulkan. Also used
    #   into name the game window.
    # - screen_resolution: must be an Array containing two Integers bigger than
    #   zero.
    # - version: must be a Hash. This hash must contain 3 key symbols with an
    #   Integer value each: +:major+, +:minor+ and +:patch+. These values are
    #   used by Vulkan.
    #
    # @param file_path [String] path to yaml file
    # @author Frederico Linhares
    def self.load_configuration(config_path)
      config = YAML.load_file(config_path)

      base_error_message =
        "Failed to parse configuration file: it must have a key "

      resolution_error_message =
        "#{base_error_message} 'screen_resolution' with an Array value, the "\
        "array must contain two Integers bigger than 0"
      if(not config.has_key?(:debug)) then
       raise BlueKitty::Error,
             "#{base_error_message} 'debug' with a boolean value."
      else
        # Force value to be boolean.
        config[:debug] = !! config[:debug]
      end

      if(not config.has_key?(:game_name)) or
        (not config[:game_name].is_a?(String)) then
        raise BlueKitty::Error,
              "#{base_error_message} 'game_name' with a String value"
      end

      if(not config.has_key?(:screen_resolution)) or
        (not config[:screen_resolution].is_a?(Array)) or
        (config[:screen_resolution].size != 2) then
        raise BlueKitty::Error, resolution_error_message
      else
        config[:screen_resolution].each do |d|
          if(not d.is_a?(Integer)) or (d < 1) then
            raise BlueKitty::Error, resolution_error_message
          end
        end
      end

      if(not config.has_key?(:max_fps)) or
        (not config[:max_fps].is_a?(Integer)) or
        (config[:max_fps] < 1) then
        raise BlueKitty::Error,
              "#{base_error_message} 'max_fps' with a Integer value bigger "\
              "that zero"
      end

      @@configurations = config
    end

    # Exit the current stage and end the {Engine#run} loop.
    def self.quit_game
      @@quit_game = true
      @@quit_stage = true
    end

    # Exit the current stage. The code block sent to {Engine#run} will be
    # called again to get the next stage.
    def self.quit_stage
      @@quit_stage = true
    end

    # Initialize the game.
    #
    # @param global_data [Object] can be any object containing information that
    #   you want to persist after every stage end. The code block you sent to
    #   this function will receive that object as argument.
    # @param file_path [String] file path that will be used by
    #   {Engine#load_configuration}
    # @return [BlueKitty::Engine]
    # @see Engine#load_configuration
    # @author Frederico Linhares
    def self.run(global_data, config_path, &stage_manager)
      @@quit_game = false

      self.load_configuration(config_path)
      self.load_core

      while not @@quit_game do
        @@quit_stage = false

        # Ensure that 'current_stage' goes out of scope.
        -> () do
          current_stage = stage_manager.call(global_data)
          loop_stage(current_stage)
        end.call()

        # In between stages is the best place to clear memory. Ruby know that
        # all memory used by the old stage can now be reclaimed.
        GC.start
      end

    ensure
      # Ensure that objects using Vulkan and SDL are destroyed before those
      # systems are unloaded.
      GC.start

      self.unload_core
    end
  end
end
