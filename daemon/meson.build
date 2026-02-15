# metrovian daemon
src_daemon = files(
  'server-micro/read/mid.cpp',
  'server-micro/read/mids.cpp',
  'server-micro/read/notes.cpp',
  'server-micro/read/progress.cpp',
  'server-micro/read/state.cpp',
  'server-micro/read/waveform.cpp',
  'server-micro/read/waveforms.cpp',
  'server-micro/write/play.cpp',
  'server-micro/write/reboot.cpp',
  'server-micro/write/volume.cpp',
  'server-micro/write/waveform.cpp',
  'server-micro/router.cpp',
  'server-micro/response.cpp',
  'state/performance.cpp',
  'state/setup.cpp',
  'state/synthesis.cpp',
  'api.cpp',
  'automata.cpp',
  'context.cpp',
  'main.cpp')

executable(
  'metroviand',
  src_daemon,
  include_directories: inc_core,
  dependencies: [dep_core],
  install : true, 
  install_dir : '/usr/local/bin/')