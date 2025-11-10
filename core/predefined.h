#pragma once
// clang-format off
#define RETURN_CLEANUP(retcode, code)   do { retcode = code; goto cleanup; } while(0)
#define LOG_ENTER()                     spdlog::trace("[enter] [{}:{}] {}", basename(__FILE__), __LINE__, __func__)
#define LOG_EXIT()                      spdlog::trace("[exit] [{}:{}] {}", basename(__FILE__), __LINE__, __func__)
#define LOG_WARN(condition)             spdlog::warn("[warn] [{}:{}] {}", basename(__FILE__), __LINE__, #condition)
#define LOG_PROGRESS(now, max)          spdlog::info("[render] [{}:{}] {}/{}", basename(__FILE__), __LINE__, now, max)
#define LOG_CONDITION(condition)        spdlog::error("[condition] [{}:{}] {}", basename(__FILE__), __LINE__, #condition)
#define LOG_ARGUMENT(argument)          spdlog::critical("[argument] [{}:{}] {}", basename(__FILE__), __LINE__, #argument)

#define CONFIG_STRING(...)              property_singleton::instance().parse(std::vector<std::string>{__VA_ARGS__})
#define CONFIG_INT32(...)               std::stoi(CONFIG_STRING(__VA_ARGS__))
#define CONFIG_INT64(...)               std::stoll(CONFIG_STRING(__VA_ARGS__))
#define CONFIG_UINT32(...)              std::stoul(CONFIG_STRING(__VA_ARGS__))
#define CONFIG_UINT64(...)              std::stoull(CONFIG_STRING(__VA_ARGS__))
#define CONFIG_FLOAT32(...)             std::stof(CONFIG_STRING(__VA_ARGS__))
#define CONFIG_FLOAT64(...)             std::stod(CONFIG_STRING(__VA_ARGS__))

#define PATH_PROPERTY                   "/.config/metrovian.json"
// clang-format on