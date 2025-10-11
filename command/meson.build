# metrovian command
src_command = files(
  'decompression/avcodec.cpp',
  'decryption/aes.cpp',
  'decryption/ecdh.cpp',
  'decryption/rsa.cpp',
  'optimization/polynomial.cpp',
  'optimization/rational.cpp',
  'optimization/sinusoidal.cpp',
  'optimization/transcendental.cpp',
  'optimization/user.cpp',
  'abstract.cpp',
  'main.cpp')

executable(
  'metrovian',
  src_command,
  include_directories: inc_core,
  dependencies: [dep_core])