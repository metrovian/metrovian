// clang-format off
#define RETURN_CLEANUP(retcode, code) do { retcode = code; goto cleanup; } while(0)
// clang-format on