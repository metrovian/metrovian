# metrovian core
src_core = files(
  'source/operation/corr.cpp',
  'source/operation/desa.cpp',
  'source/operation/fft.cpp',
  'source/reconstruction/decompression/abstract.cpp',
  'source/reconstruction/decompression/audio.cpp',
  'source/reconstruction/decompression/avcodec.cpp',
  'source/reconstruction/decryption/abstract.cpp',
  'source/reconstruction/decryption/ecdsa.cpp',
  'source/reconstruction/decryption/rsa.cpp',
  'source/reconstruction/optimization/abstract.cpp',
  'source/reconstruction/optimization/polynomial.cpp',
  'source/reconstruction/optimization/rational.cpp',
  'source/reconstruction/optimization/sinusoidal.cpp',
  'source/reconstruction/optimization/transcendental.cpp',
  'source/reconstruction/optimization/user.cpp',
  'source/synthesis/abstract.cpp',
  'source/synthesis/sin.cpp',
  'source/synthesis/square.cpp',
  'source/property.cpp')

lib_core = static_library(
  'metrovian-core',
  src_core,
  include_directories: inc_core,
  dependencies: dep_common)

dep_core = declare_dependency(
  link_with: lib_core,
  include_directories: inc_core,
  dependencies: dep_common)