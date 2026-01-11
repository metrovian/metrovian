#pragma once
// clang-format off
#define RETURN_CLEANUP(retcode, code)   do { retcode = code; goto cleanup; } while(0)

#define LOG_PATH                        std::string(__FILE__).substr(3)
#define LOG_EXIT()                      spdlog::trace("[exit] [{}:{}] {}", LOG_PATH, __LINE__, __func__)
#define LOG_VALUE(now)                  spdlog::debug("[value] [{}:{}] {} == {}", LOG_PATH, __LINE__, std::string(#now), now)
#define LOG_PROGRESS(now, max)          spdlog::debug("[render] [{}:{}] {}/{}", LOG_PATH, __LINE__, now, max)
#define LOG_ENTER()                     spdlog::info("[enter] [{}:{}] {}", LOG_PATH, __LINE__, __func__)
#define LOG_WARN(condition)             spdlog::warn("[warn] [{}:{}] {}", LOG_PATH, __LINE__, std::string(#condition))
#define LOG_CONDITION(condition)        spdlog::error("[condition] [{}:{}] {}", LOG_PATH, __LINE__, std::string(#condition))
#define LOG_ARGUMENT(condition)         spdlog::critical("[argument] [{}:{}] {}", LOG_PATH, __LINE__, std::string(#condition))

#define CONFIG_STRING(...)              property_singleton::instance().parse(std::vector<std::string>{__VA_ARGS__})
#define CONFIG_INT32(...)               std::stoi(CONFIG_STRING(__VA_ARGS__))
#define CONFIG_INT64(...)               std::stoll(CONFIG_STRING(__VA_ARGS__))
#define CONFIG_UINT32(...)              std::stoul(CONFIG_STRING(__VA_ARGS__))
#define CONFIG_UINT64(...)              std::stoull(CONFIG_STRING(__VA_ARGS__))
#define CONFIG_FLOAT32(...)             std::stof(CONFIG_STRING(__VA_ARGS__))
#define CONFIG_FLOAT64(...)             std::stod(CONFIG_STRING(__VA_ARGS__))

#define PATH_DIR                        "/.config/metrovian"
#define PATH_PROPERTY                   "/.config/metrovian/property.json"
#define PATH_PRESET                     "/.config/metrovian/preset.json"
// clang-format on