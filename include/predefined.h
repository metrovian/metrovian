// clang-format off
#define RETURN_CLEANUP(retcode, code)   do { retcode = code; goto cleanup; } while(0)
#define RSA_FERMAT_MAX_ITERATION        1000000
#define RSA_POLLARDS_RHO_MAX_ITERATION  100000000
#define RSA_POLLARDS_P1_MAX_ITERATION   100000
#define RSA_WILLIAMS_P1_MAX_ITERATION   100000
// clang-format on