#include "decryption/rsa.h"
#include "predefined.h"

int8_t decryption_rsa::setkey(const std::vector<uint8_t> &private_key) {
	LOG_ENTER();
	const uint8_t *ptr = private_key.data();
	EVP_PKEY *pkey = d2i_PrivateKey(EVP_PKEY_RSA, nullptr, &ptr, static_cast<int64_t>(private_key.size()));
	if (pkey == nullptr) {
		LOG_CONDITION(d2i_PrivateKey(EVP_PKEY_RSA) == nullptr);
		LOG_EXIT();
		return -1;
	}

	EVP_PKEY_free(pkey);
	private_key_ = private_key;
	LOG_EXIT();
	return 0;
}

int8_t decryption_rsa::setkey(const std::string &private_key) {
	LOG_ENTER();
	BIO *bio = BIO_new_mem_buf(private_key.data(), static_cast<int32_t>(private_key.size()));
	if (bio == nullptr) {
		LOG_CONDITION(BIO_new_mem_buf == nullptr);
		LOG_EXIT();
		return -1;
	}

	EVP_PKEY *pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
	BIO_free(bio);
	if (pkey == nullptr) {
		LOG_CONDITION(PEM_read_bio_PrivateKey == nullptr);
		LOG_EXIT();
		return -2;
	}

	int32_t len_key = i2d_PrivateKey(pkey, nullptr);
	if (len_key <= 0) {
		EVP_PKEY_free(pkey);
		LOG_CONDITION(i2d_PrivateKey <= 0);
		LOG_EXIT();
		return -3;
	}

	std::vector<uint8_t> der_key(len_key);
	uint8_t *ptr = der_key.data();
	if (i2d_PrivateKey(pkey, &ptr) != len_key) {
		EVP_PKEY_free(pkey);
		LOG_CONDITION(i2d_PrivateKey <= 0);
		LOG_EXIT();
		return -4;
	}

	EVP_PKEY_free(pkey);
	private_key_ = std::move(der_key);
	LOG_EXIT();
	return 0;
}

int8_t decryption_rsa::calckey(const std::vector<uint8_t> &public_key, rsa::attack algorithm) {
	return calckey(base64(public_key), algorithm);
}

int8_t decryption_rsa::calckey(const std::string &public_key, rsa::attack algorithm) {
	LOG_ENTER();
	int8_t retcode = 0;
	BIO *bio = nullptr;
	EVP_PKEY_CTX *ctx_public = nullptr;
	EVP_PKEY_CTX *ctx_private = nullptr;
	EVP_PKEY *pkey_public = nullptr;
	EVP_PKEY *pkey_private = nullptr;
	BN_CTX *ctx_rsa = nullptr;
	BIGNUM *phi_rsa = nullptr;
	BIGNUM *p1_rsa = nullptr;
	BIGNUM *q1_rsa = nullptr;
	BIGNUM *p_rsa = nullptr;
	BIGNUM *q_rsa = nullptr;
	BIGNUM *d_rsa = nullptr;
	BIGNUM *n_rsa = nullptr;
	BIGNUM *e_rsa = nullptr;
	BIGNUM *coefficient_rsa = nullptr;
	BIGNUM *exponent1_rsa = nullptr;
	BIGNUM *exponent2_rsa = nullptr;
	OSSL_PARAM_BLD *param_bld = nullptr;
	OSSL_PARAM *param = nullptr;
	BIO *mem = nullptr;
	BUF_MEM *buf_mem = nullptr;
	char *p_hexstr = nullptr;
	char *q_hexstr = nullptr;
	char *n_hexstr = nullptr;
	bio = BIO_new_mem_buf(public_key.data(), static_cast<int32_t>(public_key.size()));
	if (bio == nullptr) {
		LOG_CONDITION(BIO_new_mem_buf == nullptr);
		RETURN_CLEANUP(retcode, -1);
	}

	pkey_public = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
	if (pkey_public == nullptr) {
		LOG_CONDITION(PEM_read_bio_PUBKEY == nullptr);
		RETURN_CLEANUP(retcode, -2);
	}

	ctx_public = EVP_PKEY_CTX_new(pkey_public, nullptr);
	if (ctx_public == nullptr) {
		LOG_CONDITION(EVP_PKEY_CTX_new == nullptr);
		RETURN_CLEANUP(retcode, -3);
	} else if (EVP_PKEY_get_base_id(pkey_public) != EVP_PKEY_RSA) {
		LOG_CONDITION(EVP_PKEY_get_base_id != EVP_PKEY_RSA);
		RETURN_CLEANUP(retcode, -4);
	}

	EVP_PKEY_get_bn_param(pkey_public, OSSL_PKEY_PARAM_RSA_N, &n_rsa);
	EVP_PKEY_get_bn_param(pkey_public, OSSL_PKEY_PARAM_RSA_E, &e_rsa);
	if (n_rsa == nullptr) {
		LOG_CONDITION(EVP_PKEY_get_bn_param(OSSL_PKEY_PARAM_RSA_N) == nullptr);
		RETURN_CLEANUP(retcode, -5);
	} else if (e_rsa == nullptr) {
		LOG_CONDITION(EVP_PKEY_get_bn_param(OSSL_PKEY_PARAM_RSA_E) == nullptr);
		RETURN_CLEANUP(retcode, -6);
	}

	ctx_rsa = BN_CTX_new();
	p_rsa = BN_new();
	q_rsa = BN_new();
	n_hexstr = BN_bn2hex(n_rsa);
	switch (algorithm) {
	case rsa::attack::trial: {
		BIGNUM *trial = BN_new();
		BIGNUM *remainder = BN_new();
		BN_set_word(trial, 2);
		while (BN_cmp(trial, n_rsa) < 0) {
			BN_mod(remainder, n_rsa, trial, ctx_rsa);
			if (BN_is_zero(remainder)) {
				BN_copy(p_rsa, trial);
				BN_div(q_rsa, nullptr, n_rsa, p_rsa, ctx_rsa);
				break;
			}

			BN_add_word(trial, 1);
		}

		BN_free(trial);
		BN_free(remainder);
		break;
	}

	case rsa::attack::fermat: {
		mpz_t n_fermat;
		mpz_t p_fermat;
		mpz_t q_fermat;
		mpz_t a_fermat;
		mpz_t b_fermat;
		mpz_t b2_fermat;
		mpz_t tmp_fermat;
		mpz_inits(n_fermat, p_fermat, q_fermat, a_fermat, b_fermat, b2_fermat, tmp_fermat, nullptr);
		mpz_set_str(n_fermat, n_hexstr, 16);
		mpz_sqrt(a_fermat, n_fermat);
		mpz_mul(tmp_fermat, a_fermat, a_fermat);
		if (mpz_cmp(tmp_fermat, n_fermat) < 0) {
			mpz_add_ui(a_fermat, a_fermat, 1);
		}

		for (uint64_t i = 0; i < RSA_FERMAT_MAX_ITERATION; ++i) {
			mpz_mul(b2_fermat, a_fermat, a_fermat);
			mpz_sub(b2_fermat, b2_fermat, n_fermat);
			if (mpz_perfect_square_p(b2_fermat)) {
				mpz_sqrt(b_fermat, b2_fermat);
				mpz_sub(p_fermat, a_fermat, b_fermat);
				mpz_add(q_fermat, a_fermat, b_fermat);
				break;
			}

			mpz_add_ui(a_fermat, a_fermat, 1);
		}

		p_hexstr = mpz_get_str(nullptr, 16, p_fermat);
		q_hexstr = mpz_get_str(nullptr, 16, q_fermat);
		BN_hex2bn(&p_rsa, p_hexstr);
		BN_hex2bn(&q_rsa, q_hexstr);
		mpz_clears(n_fermat, p_fermat, q_fermat, a_fermat, b_fermat, b2_fermat, tmp_fermat, nullptr);
		break;
	}

	case rsa::attack::pollards_rho: {
		mpz_t n_rho;
		mpz_t a_rho;
		mpz_t b_rho;
		mpz_t d_rho;
		mpz_t one_rho;
		mpz_t absub_rho;
		mpz_t tmp_rho;
		mpz_inits(n_rho, a_rho, b_rho, d_rho, one_rho, absub_rho, tmp_rho, nullptr);
		mpz_set_str(n_rho, n_hexstr, 16);
		mpz_set_ui(a_rho, 2);
		mpz_set_ui(b_rho, 2);
		mpz_set_ui(d_rho, 1);
		mpz_set_ui(one_rho, 1);
		auto iteration_rho = [&](mpz_t result, const mpz_t value) {
			mpz_mul(tmp_rho, value, value);
			mpz_add_ui(tmp_rho, tmp_rho, 1);
			mpz_mod(result, tmp_rho, n_rho);
		};

		for (uint64_t i = 0; i < RSA_POLLARDS_RHO_MAX_ITERATION; ++i) {
			iteration_rho(a_rho, a_rho);
			iteration_rho(tmp_rho, b_rho);
			iteration_rho(b_rho, tmp_rho);
			mpz_cmp(a_rho, b_rho) > 0 ? mpz_sub(absub_rho, a_rho, b_rho) : mpz_sub(absub_rho, b_rho, a_rho);
			mpz_gcd(d_rho, absub_rho, n_rho);
			if (mpz_cmp_ui(d_rho, 1) != 0) {
				break;
			}
		}

		if (mpz_cmp(d_rho, one_rho) == 0 || mpz_cmp(d_rho, n_rho) == 0) {
			mpz_clears(n_rho, a_rho, b_rho, d_rho, tmp_rho, one_rho, absub_rho, nullptr);
			break;
		}

		mpz_divexact(tmp_rho, n_rho, d_rho);
		p_hexstr = mpz_get_str(nullptr, 16, d_rho);
		q_hexstr = mpz_get_str(nullptr, 16, tmp_rho);
		BN_hex2bn(&p_rsa, p_hexstr);
		BN_hex2bn(&q_rsa, q_hexstr);
		mpz_clears(n_rho, a_rho, b_rho, d_rho, one_rho, absub_rho, tmp_rho, nullptr);
		break;
	}

	case rsa::attack::pollards_p1: {
		mpz_t n_p1;
		mpz_t a_p1;
		mpz_t d_p1;
		mpz_t m_p1;
		mpz_t one_p1;
		mpz_t exp_p1;
		mpz_t tmp_p1;
		mpz_inits(n_p1, a_p1, d_p1, m_p1, one_p1, exp_p1, tmp_p1, nullptr);
		mpz_set_str(n_p1, n_hexstr, 16);
		mpz_set_ui(a_p1, 2);
		mpz_set_ui(m_p1, 1);
		mpz_set_ui(one_p1, 1);
		auto primecheck_p1 = [](uint64_t value) {
			for (uint64_t i = 2; i * i <= value; ++i) {
				if (value % i == 0) {
					return false;
				}
			}

			return true;
		};

		for (uint64_t i = 2; i < RSA_POLLARDS_P1_MAX_ITERATION; ++i) {
			if (primecheck_p1(i)) {
				uint64_t pow_p1 = i;
				while (pow_p1 * i < RSA_POLLARDS_P1_MAX_ITERATION) {
					pow_p1 *= i;
				}

				mpz_mul_ui(tmp_p1, m_p1, pow_p1);
				mpz_set(m_p1, tmp_p1);
			}
		}

		mpz_powm(a_p1, a_p1, m_p1, n_p1);
		mpz_sub(tmp_p1, a_p1, one_p1);
		mpz_gcd(d_p1, tmp_p1, n_p1);
		if (mpz_cmp_ui(d_p1, 1) == 0 || mpz_cmp(d_p1, n_p1) == 0) {
			mpz_clears(n_p1, a_p1, d_p1, one_p1, tmp_p1, exp_p1, m_p1, nullptr);
			break;
		}

		mpz_divexact(tmp_p1, n_p1, d_p1);
		p_hexstr = mpz_get_str(nullptr, 16, d_p1);
		q_hexstr = mpz_get_str(nullptr, 16, tmp_p1);
		BN_hex2bn(&p_rsa, p_hexstr);
		BN_hex2bn(&q_rsa, q_hexstr);
		mpz_clears(n_p1, a_p1, d_p1, m_p1, one_p1, exp_p1, tmp_p1, nullptr);
		break;
	}

	case rsa::attack::williams_p1: {
		mpz_t n_p1;
		mpz_t d_p1;
		mpz_t m_p1;
		mpz_t one_p1;
		mpz_t exp_p1;
		mpz_t ures_p1;
		mpz_t vres_p1;
		mpz_t ubase_p1;
		mpz_t vbase_p1;
		mpz_t tmp_p1;
		mpz_inits(n_p1, d_p1, m_p1, one_p1, exp_p1, ures_p1, vres_p1, ubase_p1, vbase_p1, tmp_p1, nullptr);
		mpz_set_str(n_p1, n_hexstr, 16);
		mpz_set_ui(m_p1, 1);
		mpz_set_ui(one_p1, 1);
		mpz_set_ui(ures_p1, 0);
		mpz_set_ui(vres_p1, 2);
		mpz_set_ui(ubase_p1, 1);
		mpz_set_ui(vbase_p1, 3);
		auto primecheck_p1 = [](uint64_t value) {
			for (uint64_t i = 2; i * i <= value; ++i) {
				if (value % i == 0) {
					return false;
				}
			}

			return true;
		};

		auto lucas_square = [](mpz_t u, mpz_t v, mpz_t n) {
			mpz_t u2, v2;
			mpz_inits(u2, v2, nullptr);
			mpz_mul(u2, u, v);
			mpz_mod(u2, u2, n);
			mpz_mul(v2, v, v);
			mpz_sub_ui(v2, v2, 2);
			mpz_mod(v2, v2, n);
			mpz_set(u, u2);
			mpz_set(v, v2);
			mpz_clears(u2, v2, nullptr);
		};

		auto lucas_cross = [](mpz_t u1, mpz_t v1, mpz_t u2, mpz_t v2, mpz_t n) {
			mpz_t t1, t2, t3, t4;
			mpz_inits(t1, t2, t3, t4, nullptr);
			mpz_mul(t1, u1, v2);
			mpz_mul(t2, u2, v1);
			mpz_add(t1, t1, t2);
			if (mpz_odd_p(t1)) {
				mpz_add(t1, t1, n);
			}

			mpz_divexact_ui(t1, t1, 2);
			mpz_mod(t1, t1, n);
			mpz_mul(t3, v1, v2);
			mpz_mul(t4, u1, u2);
			mpz_mul_ui(t4, t4, 5);
			mpz_add(t3, t3, t4);
			if (mpz_odd_p(t3)) {
				mpz_add(t3, t3, n);
			}

			mpz_divexact_ui(t3, t3, 2);
			mpz_mod(t3, t3, n);
			mpz_set(u1, t1);
			mpz_set(v1, t3);
			mpz_clears(t1, t2, t3, t4, nullptr);
		};

		for (uint64_t i = 2; i < RSA_WILLIAMS_P1_MAX_ITERATION; ++i) {
			if (primecheck_p1(i)) {
				uint64_t pow_p1 = i;
				while (pow_p1 * i < RSA_WILLIAMS_P1_MAX_ITERATION) {
					pow_p1 *= i;
				}

				mpz_mul_ui(tmp_p1, m_p1, pow_p1);
				mpz_set(m_p1, tmp_p1);
			}
		}

		mpz_set(exp_p1, m_p1);
		for (uint64_t i = 0; i < mpz_sizeinbase(exp_p1, 2); ++i) {
			lucas_square(ures_p1, vres_p1, n_p1);
			if (mpz_tstbit(exp_p1, mpz_sizeinbase(exp_p1, 2) - i - 1)) {
				lucas_cross(ures_p1, vres_p1, ubase_p1, vbase_p1, n_p1);
			}
		}

		mpz_sub_ui(tmp_p1, vres_p1, 2);
		mpz_gcd(d_p1, tmp_p1, n_p1);
		if (mpz_cmp_ui(d_p1, 1) == 0 || mpz_cmp(d_p1, n_p1) == 0) {
			mpz_clears(n_p1, d_p1, m_p1, one_p1, exp_p1, ures_p1, vres_p1, ubase_p1, vbase_p1, tmp_p1, nullptr);
			break;
		}

		mpz_divexact(tmp_p1, n_p1, d_p1);
		p_hexstr = mpz_get_str(nullptr, 16, d_p1);
		q_hexstr = mpz_get_str(nullptr, 16, tmp_p1);
		BN_hex2bn(&p_rsa, p_hexstr);
		BN_hex2bn(&q_rsa, q_hexstr);
		mpz_clears(n_p1, d_p1, m_p1, one_p1, exp_p1, ures_p1, vres_p1, ubase_p1, vbase_p1, tmp_p1, nullptr);
		break;
	}

	default:
		LOG_ARGUMENT(algorithm);
		RETURN_CLEANUP(retcode, -7);
	}

	if (BN_is_zero(p_rsa)) {
		LOG_CONDITION(BN_is_zero == 1);
		RETURN_CLEANUP(retcode, -8);
	}

	phi_rsa = BN_new();
	p1_rsa = BN_new();
	q1_rsa = BN_new();
	d_rsa = BN_new();
	BN_sub(p1_rsa, p_rsa, BN_value_one());
	BN_sub(q1_rsa, q_rsa, BN_value_one());
	BN_mul(phi_rsa, p1_rsa, q1_rsa, ctx_rsa);
	if (BN_mod_inverse(d_rsa, e_rsa, phi_rsa, ctx_rsa) == nullptr) {
		LOG_CONDITION(BN_mod_inverse == nullptr);
		RETURN_CLEANUP(retcode, -9);
	}

	param_bld = OSSL_PARAM_BLD_new();
	ctx_private = EVP_PKEY_CTX_new_from_name(nullptr, "RSA", nullptr);
	coefficient_rsa = BN_new();
	exponent1_rsa = BN_new();
	exponent2_rsa = BN_new();
	BN_mod_inverse(coefficient_rsa, q_rsa, p_rsa, ctx_rsa);
	BN_mod(exponent1_rsa, d_rsa, p1_rsa, ctx_rsa);
	BN_mod(exponent2_rsa, d_rsa, q1_rsa, ctx_rsa);
	OSSL_PARAM_BLD_push_BN(param_bld, OSSL_PKEY_PARAM_RSA_N, n_rsa);
	OSSL_PARAM_BLD_push_BN(param_bld, OSSL_PKEY_PARAM_RSA_E, e_rsa);
	OSSL_PARAM_BLD_push_BN(param_bld, OSSL_PKEY_PARAM_RSA_D, d_rsa);
	OSSL_PARAM_BLD_push_BN(param_bld, "rsa-coefficient1", coefficient_rsa);
	OSSL_PARAM_BLD_push_BN(param_bld, "rsa-factor1", p_rsa);
	OSSL_PARAM_BLD_push_BN(param_bld, "rsa-factor2", q_rsa);
	OSSL_PARAM_BLD_push_BN(param_bld, "rsa-exponent1", exponent1_rsa);
	OSSL_PARAM_BLD_push_BN(param_bld, "rsa-exponent2", exponent2_rsa);
	mem = BIO_new(BIO_s_mem());
	if (mem == nullptr) {
		LOG_CONDITION(BIO_new == nullptr);
		RETURN_CLEANUP(retcode, -10);
	}

	param = OSSL_PARAM_BLD_to_param(param_bld);
	EVP_PKEY_fromdata_init(ctx_private);
	EVP_PKEY_fromdata(ctx_private, &pkey_private, EVP_PKEY_KEYPAIR, param);
	if (i2d_PrivateKey_bio(mem, pkey_private) != 1) {
		LOG_CONDITION(i2d_PrivateKey_bio(EVP_PKEY_KEYPAIR) != 1);
		RETURN_CLEANUP(retcode, -11);
	}

	BIO_get_mem_ptr(mem, &buf_mem);
	if (buf_mem == nullptr) {
		LOG_CONDITION(BIO_get_mem_ptr == nullptr);
		RETURN_CLEANUP(retcode, -12);
	} else if (buf_mem->data == nullptr) {
		LOG_CONDITION(BIO_get_mem_ptr == nullptr);
		RETURN_CLEANUP(retcode, -13);
	}

	private_key_.clear();
	private_key_.assign(buf_mem->data, buf_mem->data + buf_mem->length);

cleanup:
	// clang-format off
	if (bio) BIO_free(bio);
	if (ctx_public) EVP_PKEY_CTX_free(ctx_public);
	if (ctx_private) EVP_PKEY_CTX_free(ctx_private);
	if (pkey_public) EVP_PKEY_free(pkey_public);
	if (pkey_private) EVP_PKEY_free(pkey_private);
	if (ctx_rsa) BN_CTX_free(ctx_rsa);
	if (phi_rsa) BN_clear_free(phi_rsa);
	if (p1_rsa) BN_clear_free(p1_rsa);
	if (q1_rsa) BN_clear_free(q1_rsa);
	if (p_rsa) BN_clear_free(p_rsa);
	if (q_rsa) BN_clear_free(q_rsa);
	if (d_rsa) BN_clear_free(d_rsa);
	if (n_rsa) BN_free(n_rsa);
	if (e_rsa) BN_free(e_rsa);
	if (coefficient_rsa) BN_free(coefficient_rsa);
	if (exponent1_rsa) BN_free(exponent1_rsa);
	if (exponent2_rsa) BN_free(exponent2_rsa);
	if (param_bld) OSSL_PARAM_BLD_free(param_bld);
	if (param) OSSL_PARAM_free(param);
	if (mem) BIO_free(mem);
	if (p_hexstr) free(p_hexstr);
	if (q_hexstr) free(q_hexstr);
	if (n_hexstr) OPENSSL_free(n_hexstr);
	// clang-format on
	LOG_EXIT();
	return retcode;
}

int8_t decryption_rsa::decryption(const std::vector<uint8_t> &cipher, std::vector<uint8_t> &plain) {
	LOG_ENTER();
	const uint8_t *ptr = private_key_.data();
	EVP_PKEY *pkey = d2i_PrivateKey(EVP_PKEY_RSA, nullptr, &ptr, static_cast<int32_t>(private_key_.size()));
	if (pkey == nullptr) {
		LOG_CONDITION(d2i_PrivateKey == nullptr);
		LOG_EXIT();
		return -1;
	}

	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, nullptr);
	if (ctx == nullptr) {
		EVP_PKEY_free(pkey);
		LOG_CONDITION(EVP_PKEY_CTX_new == nullptr);
		LOG_EXIT();
		return -2;
	}

	int32_t pkey_bits = EVP_PKEY_get_bits(pkey);
	if (EVP_PKEY_decrypt_init(ctx) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		LOG_CONDITION(EVP_PKEY_decrypt_init <= 0);
		LOG_EXIT();
		return -3;
	}

	if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		LOG_CONDITION(EVP_PKEY_CTX_set_rsa_padding(RSA_PKCS1_PADDING) <= 0);
		LOG_EXIT();
		return -4;
	}

	size_t len_update = 0;
	if (EVP_PKEY_decrypt(ctx, nullptr, &len_update, cipher.data(), cipher.size()) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		LOG_CONDITION(EVP_PKEY_decrypt <= 0);
		LOG_EXIT();
		return -5;
	}

	plain.clear();
	plain.resize(len_update);
	if (EVP_PKEY_decrypt(ctx, plain.data(), &len_update, cipher.data(), cipher.size()) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		LOG_CONDITION(EVP_PKEY_decrypt <= 0);
		LOG_EXIT();
		return -6;
	}

	plain.resize(len_update);
	EVP_PKEY_CTX_free(ctx);
	EVP_PKEY_free(pkey);
	spdlog::debug("rsa-{} cipher: \"{}\"", pkey_bits, base64(cipher));
	spdlog::debug("rsa-{} plain:  \"{}\"", pkey_bits, base64(plain));
	LOG_EXIT();
	return 0;
}