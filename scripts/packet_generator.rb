#!/usr/bin/env ruby

require 'set'
require 'forwardable'
require 'pp'

class PacketGenerator

  def initialize(file, dest)
    @file    = file
    @packets = SortedSet.new
    @locked  = true
    @dest    = dest
  end

  def method_missing(method, *arguments, &block)
    if @locked
      return super
    end

    if arguments.size != 1
      raise ArgumentError, "Wrong number of arguments " \
        "(#{arguments.size} for 1)"
    end

    packet = Packet.new
    packet.name = method
    packet.id   = arguments.first
    packet.instance_exec &block
    packet.lock!

    @packets << packet
  end

  def generate!
    data = nil
    @locked = false
    File.open(@file, "r") { |f| data = f.read }
    instance_eval data, @file, 0
    @locked = true
    @packets
  end

  def output!
    outbuf = []
    @packets.each do |packet|
      typedata = []
      packet.fields.each do |field|
        typedata << "    #{field[:type]}(#{field[:arguments].join(', ')})"
      end
      typedata = typedata.join("\n").chomp
      outbuf <<
 <<-PACKDEF
PACKET(
  #{packet.name},
  #{"0x%02x" % packet.id},
  {
#{typedata}
  }
);

PACKDEF

    end

    File.open(@dest, "w") { |f|  f.write outbuf.join("") }
  end

end

class Packet
  attr_accessor :name, :id, :fields

  include Enumerable
  extend Forwardable

  def_delegator :@id, :"<=>"

  def initialize
    @locked = false
    @fields = []
  end

  def method_missing(method, *arguments, &block)
    if @locked
      return super
    end

    @fields << {type: method, arguments: arguments}
  end

  def lock!
    @locked = true
  end
end

if File.basename($0, '.rb') == "packet_generator"
   packet_generator = PacketGenerator.new(ARGV[0], ARGV[1])
   packet_generator.generate!
   packet_generator.output!
end
