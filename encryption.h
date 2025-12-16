#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <string>
#include <vector>

typedef unsigned long long ull;

typedef struct {
    ull n, e, d;
} rsa_keys;

std::string caesar_encrypt(const std::string &text, int shift);
std::string caesar_decrypt(const std::string &text, int shift);
std::vector<int> sieve_eratosthenes(int n);
ull gcd(ull a, ull b);
ull inverse_mod(ull exp, ull phi);
ull safe_mod_mul(ull a, ull b, ull mod);
ull pow_mod(ull base, ull exp, ull mod);
std::vector<ull> rsa_encrypt(const std::string &text, ull e, ull n);
std::string rsa_decrypt(const std::vector<ull> &text, ull d, ull n);
rsa_keys keys(int min = 300, int max = 1000);

#endif