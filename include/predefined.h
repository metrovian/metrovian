// clang-format off
#define RETURN_CLEANUP(retcode, code)   do { retcode = code; goto cleanup; } while(0)
#define LOG_ENTER()                     spdlog::trace("[enter] [{}:{}] {}", basename(__FILE__), __LINE__, __func__)
#define LOG_EXIT()                      spdlog::trace("[exit] [{}:{}] {}", basename(__FILE__), __LINE__, __func__)
#define LOG_CONDITION(condition)        spdlog::error("[condition] [{}:{}] {}", basename(__FILE__), __LINE__, #condition);
#define LOG_ARGUMENT(argument)          spdlog::critical("[argument] [{}:{}] {}", basename(__FILE__), __LINE__, #argument);
#define RSA_FERMAT_MAX_ITERATION        1000000
#define RSA_POLLARDS_RHO_MAX_ITERATION  100000000
#define RSA_POLLARDS_P1_MAX_ITERATION   100000
#define RSA_WILLIAMS_P1_MAX_ITERATION   100000
// clang-format on