# metrovian command
src_command = files(
  'decompression/audio.cpp',
  'operation/correlation.cpp',
  'operation/fourier.cpp',
  'optimization/polynomial.cpp',
  'optimization/rational.cpp',
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