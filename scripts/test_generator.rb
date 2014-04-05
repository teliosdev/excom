#!/usr/bin/env ruby

require 'pathname'
require 'strscan'
require 'erb'

base_directory = Pathname.new(__FILE__).parent.parent.realpath
test_directory = base_directory + "test"

test_definitions = test_directory.each_child.select { |e| e.extname == ".test" }

template = <<TEMP
#include <stdlib.h>
#include <stdio.h>
#include "excom.h"
#define __FILE "<%= @file.basename %>"
#include "utest.h"

//prefix
<%= prefix %>

static size_t tests  = 0;
static size_t failed = 0;

<% groups.each do |group| %>
  <% next if group[:name] != "GROUP" %>
  <% gname = e(group[:arguments]) %>
  <% group[:subgroups].each do |test| %>
    <% next if test[:name] != "TEST" %>
    void <%= gname %><%= e(test[:arguments]) %>()
    {
      tests++;
      int test_success = 1;
      output(TEXT_COLOR_MAGENTA "\\t\\tTEST \\"" TEXT_COLOR_BOLD_MAGENTA
        "<%= test[:arguments].gsub(/"/, '\\"') %>" TEXT_COLOR_MAGENTA
        "\\": ");
      //before
      <%= (group[:subgroups] + test[:subgroups]).select { |x|
          x[:name] == "BEFORE" }.map {|x| x[:body]}.map { |x|
          body_of(x) }.join("\\n") %>
      //body
      <%= body_of(test[:body]) %>
      //after
      <%= (group[:subgroups] + test[:subgroups]).select { |x|
          x[:name] == "AFTER" }.map { |x| x[:body] }.map { |x|
          body_of(x) }.join("\\n") %>
  #line 42
      if(test_success) {
  #ifdef VERBOSE
        output("\\n\\t\\t\\t" TEXT_COLOR_GREEN "OK\\n");
  #else
        output(TEXT_COLOR_BOLD_GREEN "OK\\n");
  #endif
      }
    }
  <% end %>
  void <%= gname %>()
  {
    output(TEXT_COLOR_MAGENTA "\\tSET \\"" TEXT_COLOR_BOLD_MAGENTA
      "<%= group[:arguments].gsub(/"/, '\\"') %>" TEXT_COLOR_MAGENTA
      "\\":\\n");
    <% group[:subgroups].each do |test| %>
    <% next if test[:name] != "TEST" %>
    <%= gname %><%= e test[:arguments] %>();
    <% end %>
  }
<% end %>

int main()
{
  output(TEXT_COLOR_MAGENTA "FILE \\""  TEXT_COLOR_BOLD_MAGENTA
         __FILE TEXT_COLOR_MAGENTA "\\":\\n");

  <% groups.each do |group| %>
  <% next if group[:name] != "GROUP" %>
  <%= e(group[:arguments]) %>();
  <% end %>

  output2("\\n\\t" TEXT_COLOR_MAGENTA "RESULT:\\n\\t\\t"
    "PASSED: " TEXT_COLOR_BOLD_MAGENTA "%zu"
    TEXT_COLOR_MAGENTA "\\n\\t\\tFAILED: "
    TEXT_COLOR_BOLD_MAGENTA "%zu" TEXT_COLOR_MAGENTA
    "\\n", tests - failed, failed);

  return TEST_RETURN;
}

TEMP

class Test
  attr_reader :groups

  def initialize(file)
    @file    = file
    @scanner = StringScanner.new(@file.read)
    @line    = 1
  end

  def parse
    @groups = groups = []
    until @scanner.eos? || @scanner.check(/[ \t]*\z/)
      @line += 1 if @scanner.scan(/^\#.*?\n/)
      groups << parse_group
    end
  end

  def parse_group(indent = 0)
    @scanner.scan(/^[ \t]{#{indent}}([A-Z0-9]+)(( [\w\W]+?)*)\n/)
    group = {}
    raise StandardError, [@line, @scanner].inspect unless @scanner.matched?
    @line += 1

    group[:name]      = @scanner[1]
    group[:arguments] = @scanner[2]
    group[:subgroups] = []
    group[:body]      = []
    until @scanner.eos?
      if @scanner.check(/^[ \t]{#{indent+2}}([A-Z0-9]+)(( [\w\W]+?)*)\n/)
        group[:subgroups] << parse_group(indent + 2)
      elsif @scanner.check(/^[ \t]{#{indent+2}}(\.[^\n]*)\n+/)
        group[:body] << [@line, parse_macro(indent + 2)]
      elsif @scanner.scan(/^[ \t]{#{indent+2}}([^\n]*\n+)/)
        group[:body] << [@line, @scanner[1]]
        @line += @scanner[1].gsub(/\A[^\n]*/, "").length
      elsif @scanner.scan(/^[ \t]{#{indent+2}}?\n/) || @scanner.scan(/^\#.*?\n/)
        @line += 1
      else
        break
      end
    end

    group
  end

  def parse_macro(indent = 0)
    @scanner.scan(/^[ \t]{#{indent}}.([A-Za-z0-9-]+)(([\w\W]+?)*?)\n/)
    body = ""
    command = @scanner[1]
    args = @scanner[2]

    until @scanner.eos?
      if @scanner.scan(/^[ \t]{#{indent+2}}(\.[^\n]*)\n+/)
        body << parse_macro(indent + 2)
      elsif @scanner.scan(/^[ \t]{#{indent+2}}([^\n]*\n+)/)
        @line += @scanner[1].gsub(/\A[^\n]*/, "").length
        body << @scanner[1]
      elsif @scanner.scan(/^[ \t]{#{indent+2}}?\n/) || @scanner.scan(/^\#.*?\n/)
        @line += 1
      else
        break
      end
    end

    macro(command, args, body, indent)
  end

  def bind
    binding
  end

  def all_groups(groups = @groups)
    if groups.size > 0
      groups + all_groups(groups.map { |x|
        x[:subgroups] }.inject([], :concat))
    else
      groups
    end
  end

  def macros
    macro_groups = all_groups.select { |x| x[:name] == "MACRO" }
    macro_groups.each do |m|
      match = m[:arguments].match(/\A ([\w-]+)(.*?)\z/)
      m[:mname] = match[1]
      m[:arg] = { :normal => [], :block => nil }

      args = match[2].scan(/(?:,\s*)?(&?[\w]+)\s*/)
      m[:arg][:normal] = args.flatten.reject { |x| x =~ /^&/ }
      m[:arg][:block]  = args.flatten.select { |x| x =~ /^&/ }.last.gsub(/^&/, "")
    end

    macro_groups.inject({}) { |m, d| m[d[:mname]] = d; m }
  end

  def macro(command, args, body, indent = 0)
    macro = macros.fetch(command)

    args = args.scan(/\s*((?:[^,]|(\'|\").*?\2)*),?\s*/).map(&:first)
    match = macro[:arg][:normal].zip(args)
    pass = match.inject({}) { |m, d| m[d[0].intern] = d[1]; m }

    if macro[:arg][:block]
      pass[macro[:arg][:block].intern] = body
    end

    sprintf body_of(macro[:body]), pass
  end

  private

  def prefix
    all_groups.select { |x|
      x[:name] == "PREFIX" }.map { |x|
      x[:body].map(&:last) }.join("\n")
  end

  def e(t)
    t.to_s.downcase.gsub(/[\W]/, "_")
  end

  def body_of(b)
    b.map { |x| "#line #{x[0]} \"#{@file.basename}\"\n#{x[1]}" }.join("\n")
  end
end

temp = ERB.new(template)
test_definitions.each do |file|
  test      = Test.new(file)
  file_name = file.basename('test').to_path + 'c'
  test.parse
  File.open(file.parent + file_name, 'w') do |f|
    f.write temp.result(test.bind)
  end
end
