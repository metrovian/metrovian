# metrovian command
src_command = files(
  'decompression/audio.cpp',
  'decryption/ecdsa.cpp',
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
  dependencies: [dep_core],
  install : true, 
  install_dir : '/usr/local/bin/')