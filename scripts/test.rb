#!/usr/bin/env ruby

require 'pathname'

base_directory = Pathname.new(__FILE__).parent.parent.expand_path

test_directory = base_directory + "test"

data = <<-DATA

#include <stdlib.h>
#include <stdio.h>
#define FILE "%{file}.test"

#define TEST_PREFIX
#include "./utest.h"
#include "./%{file}.test"
#undef TEST_PREFIX

int main() {
#define TEST_BEFORE
#include "./utest.h"
TEST_BEFORE_HOOK1;
#include "./%{file}.test"
TEST_BEFORE_HOOK2;
#undef TEST_BEFORE

#define TEST_DURING
#include "./utest.h"
#include "./%{file}.test"
#undef TEST_DURING

#define TEST_AFTER
#include "./utest.h"
TEST_AFTER_HOOK1;
#include "./%{file}.test"
TEST_AFTER_HOOK2;
#undef TEST_AFTER

  return TEST_RETURN;
}

DATA

test_directory.children.select(&:file?).each do |child|
  next unless child.basename.to_s =~ /\A(.+?)\.test\z/
  new_file = Pathname.new(child.dirname) + "#{$1}.c"

  File.open(new_file, "w") do |f|
    f.write sprintf(data, :file => $1)
  end
end

