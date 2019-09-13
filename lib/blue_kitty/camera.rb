# SPDX-License-Identifier: MIT
module BlueKitty
  class Camera
    attr_reader :position, :rotation

    def initialize(position, rotation)
      unless position.is_a?(Vector3D) and rotation.is_a?(Vector3D) then
        raise ArgumentError.new("arguments must be Vector3D")
      end

      @position = position
      @rotation = rotation
    end

    def position=(new_position)
      unless new_position.is_a?(Vector3D) then
        raise ArgumentError.new("argument must be Vector3D")
      end

      @position = new_position
    end

    def rotation=(new_rotation)
      unless new_rotation.is_a?(Vector3D) then
        raise ArgumentError.new("argument must be Vector3D")
      end

      @rotation = new_rotation
    end
  end
end
