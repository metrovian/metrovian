# metrovian core
src_core = files(
  'operation/corr.cpp',
  'operation/desa.cpp',
  'operation/fft.cpp',
  'reconstruction/decompression/audio.cpp',
  'reconstruction/decryption/abstract.cpp',
  'reconstruction/decryption/ecdsa.cpp',
  'reconstruction/decryption/rsa.cpp',
  'reconstruction/optimization/abstract.cpp',
  'reconstruction/optimization/polynomial.cpp',
  'reconstruction/optimization/rational.cpp',
  'reconstruction/optimization/sinusoidal.cpp',
  'reconstruction/optimization/transcendental.cpp',
  'reconstruction/optimization/user.cpp',
  'sound/factory.cpp',
  'sound/player.cpp',
  'sound/sequencer.cpp',
  'sound/writer.cpp',
  'synthesis/abstract.cpp',
  'synthesis/effector.cpp',
  'synthesis/hammond.cpp',
  'synthesis/oscillator.cpp',
  'synthesis/saw.cpp',
  'synthesis/sin.cpp',
  'synthesis/square.cpp',
  'synthesis/unison.cpp',
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