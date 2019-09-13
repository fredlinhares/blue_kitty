# SPDX-License-Identifier: MIT
module BlueKitty
  module Entity3D
    attr_reader :model, :position, :rotation

    def model=(new_model)
      unless new_model.is_a?(Model) then
        raise ArgumentError.new("argument must be a Model")
      end

      @model = new_model
    end

    def position=(new_position)
      unless new_position.is_a?(Vector3D) then
        raise ArgumentError.new("argument must be a Vector3D")
      end

      @position = new_position
    end

    def rotation=(new_rotation)
      unless new_rotation.is_a?(Vector3D) then
        raise ArgumentError.new("argument must be a Vector3D")
      end

      @rotation = new_rotation
    end
  end
end
