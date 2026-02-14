# metrovian daemon
src_daemon = files(
  'server-micro/parser/read/mid.cpp',
  'server-micro/parser/read/mids.cpp',
  'server-micro/parser/read/notes.cpp',
  'server-micro/parser/read/progress.cpp',
  'server-micro/parser/read/state.cpp',
  'server-micro/parser/read/waveform.cpp',
  'server-micro/parser/read/waveforms.cpp',
  'server-micro/parser/write/play.cpp',
  'server-micro/parser/write/reboot.cpp',
  'server-micro/parser/write/volume.cpp',
  'server-micro/parser/write/waveform.cpp',
  'server-micro/parser/abstract.cpp',
  'server-micro/parser/router.cpp',
  'server-micro/api.cpp',
  'server-micro/automata.cpp',
  'server-micro/context.cpp',
  'server-micro/response.cpp',
  'state/performance.cpp',
  'state/setup.cpp',
  'state/synthesis.cpp',
  'main.cpp')

executable(
  'metroviand',
  src_daemon,
  include_directories: inc_core,
  dependencies: [dep_core],
  install : true, 
  install_dir : '/usr/local/bin/')