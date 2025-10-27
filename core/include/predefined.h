#pragma once
// clang-format off
#define RETURN_CLEANUP(retcode, code)   do { retcode = code; goto cleanup; } while(0)
#define LOG_ENTER()                     spdlog::trace("[enter] [{}:{}] {}", basename(__FILE__), __LINE__, __func__)
#define LOG_EXIT()                      spdlog::trace("[exit] [{}:{}] {}", basename(__FILE__), __LINE__, __func__)
#define LOG_WARN(condition)             spdlog::warn("[warn] [{}:{}] {}", basename(__FILE__), __LINE__, #condition)
#define LOG_CONDITION(condition)        spdlog::error("[condition] [{}:{}] {}", basename(__FILE__), __LINE__, #condition)
#define LOG_ARGUMENT(argument)          spdlog::critical("[argument] [{}:{}] {}", basename(__FILE__), __LINE__, #argument)

#define PATH_PROPERTY                   "/.config/metrovian.json"
// clang-format on