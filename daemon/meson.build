# metrovian daemon
src_daemon = files(
  'main.cpp')

executable(
  'metroviand',
  src_daemon,
  include_directories: inc_core,
  dependencies: [dep_core],
  install : true, 
  install_dir : '/usr/local/bin/')