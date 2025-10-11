# metrovian project
project(
  'metrovian-tools',
  'cpp', 
  'c',
  version: '1.0.0',
  default_options: [
    'cpp_std=c++20',
    'c_std=c99',
    'warning_level=2',
    'buildtype=release'])

# include
inc_core = include_directories('core/include')

# dependency
dep_alsa = dependency('alsa', required: true)
dep_eigen = dependency('eigen3', required: true)
dep_gmp = dependency('gmp', required: true)
dep_gsl = dependency('gsl', required: true)
dep_json = dependency('nlohmann_json', required: true)
dep_muparser = dependency('muparser', required: true)
dep_openssl = dependency('openssl', required: true)
dep_spdlog = dependency('spdlog', required: true)
dep_ffmpeg_codec = dependency('libavcodec', required: true)
dep_ffmpeg_format = dependency('libavformat', required: true)
dep_ffmpeg_util = dependency('libavutil', required: true)
dep_ffmpeg_resample = dependency('libswresample', required: true)
dep_common = [
  dep_alsa,
  dep_eigen,
  dep_gmp,
  dep_gsl,
  dep_json,
  dep_muparser,
  dep_openssl,
  dep_spdlog,
  dep_ffmpeg_codec,
  dep_ffmpeg_format,
  dep_ffmpeg_util,
  dep_ffmpeg_resample]

# subdir
subdir('core')
subdir('command')