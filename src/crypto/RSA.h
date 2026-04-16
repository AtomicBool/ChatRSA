#pragma once
#include <windows.h>
#include <bcrypt.h>
#include <vector>
#include <string>
#include <algorithm>

#pragma comment(lib, "bcrypt.lib")

class RSA
{
public:
    RSA();
    ~RSA();

    bool GenerateKeyPair(int keySize = 2048);

    std::vector<uint8_t> Encrypt(const std::string& data);
    std::string Decrypt(const std::vector<uint8_t>& data);

    std::vector<uint8_t> ExportPublicKey();
    std::vector<uint8_t> ExportPrivateKey();

    bool ImportPublicKey(const std::vector<uint8_t>& blob);
    bool ImportPrivateKey(const std::vector<uint8_t>& blob);

private:
    ULONG PADDING_FLAG = BCRYPT_PAD_PKCS1;

    std::vector<uint8_t> EncryptBlock(
        const uint8_t* data,
        size_t size
    );

    std::vector<uint8_t> DecryptBlock(
        const uint8_t* data,
        size_t size
    );

private:
    BCRYPT_ALG_HANDLE m_alg;
    BCRYPT_KEY_HANDLE m_key;
};