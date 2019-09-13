# SPDX-License-Identifier: MIT
module BlueKitty
  class Controller
    def initialize
      @commands = {}
    end

    def add_command(command, &function)
      @commands[command] = function
    end

    def call_command(command)
      @commands[command].call unless @commands[command].nil?
    end
  end
end
