#include <string>
#include <vector>
#include <time.h>
#include <iostream>
#include <limits.h>
#include <cstring>

#include "encryption.h"

typedef unsigned long long ull;

std::string caesar_encrypt(const std::string &text, int shift) {
    std::string res = "";
    shift %= 26;
    if (shift < 0) shift = 26 + shift;
    for (char c : text) {
        if (isalpha(c)) {
            char base = isupper(c) ? 'A' : 'a';
            res += char((c - base + shift) % 26 + base);
        } else {
            res += c;
        }
    }
    return res;
}

std::string caesar_decrypt(const std::string &text, int shift) {
    return caesar_encrypt(text, -shift);
}

std::vector<int> sieve_eratosthenes(int n) {
    std::vector<bool> primes(n + 1, true);
    primes[0] = primes[1] = false;
    for (int i = 2; i * i <= n; i++) {
        if (primes[i])
            for (int j = i * i; j <= n; j += i) primes[j] = false;
    }
    std::vector<int> res;
    for (int i = 2; i <= n; i++) if(primes[i]) res.push_back(i);
    return res;
}

ull gcd(ull a, ull b) {
    if (a < b) {
        std::swap(a, b);
    }
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

ull inverse_mod(ull exp, ull phi) {
    long long m0 = (long long) phi, a0 = (long long) exp, y = 0, x = 1;
    if (phi == 1) return 0;
    while (a0 > 1) {
        long long q = a0 / phi, tmp = phi;
        phi = a0 % phi;
        a0 = tmp;
        tmp = y;
        y = x - q * y;
        x = tmp;
    }
    if (x < 0) x += m0;
    return (ull)x;
}

ull safe_mod_mul(ull a, ull b, ull mod) {
    if (mod == 0) return 0;
    a %= mod;
    b %= mod;
    if (!a || !b) return 0;
    if (a <= ULLONG_MAX / b) {
        return (a * b) % mod;
    }
    ull q = (ull)((long double)a * b / mod);
    ull result = a * b - q * mod;
    if (result >= mod) result -= mod;
    if (result < 0) result += mod;
    return result;
}

ull pow_mod(ull base, ull exp, ull mod) {
    if (mod == 1) return 0;
    ull res = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) res = safe_mod_mul(res, base, mod);
        base = safe_mod_mul(base, base, mod);
        exp >>= 1;
    }
    return res;
} 

std::vector<ull> rsa_encrypt(const std::string &text, ull e, ull n) {
    std::vector<ull> res;
    for (char c : text) {
        res.push_back(pow_mod(c, e, n));
    }
    return res;
}

std::string rsa_decrypt(const std::vector<ull> &text, ull d, ull n) {
    std::string res = "";
    for (ull c : text) {
        res += static_cast<char>(pow_mod(c, d, n));
    }
    return res;
}

rsa_keys keys(int min, int max) {
    rsa_keys keys = {0, 0, 0};
    std::vector<int> primes = sieve_eratosthenes(max);
    int indent = 0;
    for (int num : primes) {
        if (num < min) indent++;
        else break;
    }
    ull p = primes[indent + rand() % ((ull) primes.size() - indent)], q;
    do {
        q = primes[indent + rand() % ((ull) primes.size() - indent)];
    } while (p == q);
    keys.n = p * q;
    ull phi = (p - 1) * (q - 1);
    keys.e = 3;
    while (keys.e < phi) {
        if (gcd(keys.e, phi) == 1) break;
        keys.e += 2;
    }
    keys.d = inverse_mod(keys.e, phi);
    return keys;
}