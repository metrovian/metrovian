// clang-format off
#define RETURN_CLEANUP(retcode, code)   do { retcode = code; goto cleanup; } while(0)
#define RSA_FERMAT_MAX_ITERATION        1000000
// clang-format on