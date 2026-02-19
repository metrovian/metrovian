# metrovian core
src_core = files(
  'operation/correlation.cpp',
  'operation/fourier.cpp',
  'decompression/abstract.cpp',
  'decompression/audio.cpp',
  'optimization/abstract.cpp',
  'optimization/polynomial.cpp',
  'optimization/rational.cpp',
  'optimization/user.cpp',
  'sound/abstract.cpp',
  'sound/factory.cpp',
  'sound/player.cpp',
  'sound/sequencer.cpp',
  'sound/writer.cpp',
  'synthesis/abstract.cpp',
  'synthesis/add.cpp',
  'synthesis/oscillator.cpp',
  'predefined.cpp',
  'property.cpp')

lib_core = static_library(
  'metrovian-core',
  src_core,
  include_directories: inc_core,
  dependencies: dep_common)

dep_core = declare_dependency(
  link_with: lib_core,
  include_directories: inc_core,
  dependencies: dep_common)