#include "crypto/RSA.h"

RSA::RSA()
    : m_alg(nullptr), m_key(nullptr)
{
    NTSTATUS status = BCryptOpenAlgorithmProvider(
        &m_alg,
        BCRYPT_RSA_ALGORITHM,
        nullptr,
        0
    );

    if (!BCRYPT_SUCCESS(status))
    {
        m_alg = nullptr;
    }
}

RSA::~RSA()
{
    if (m_key)
    {
        BCryptDestroyKey(m_key);
        m_key = nullptr;
    }

    if (m_alg)
    {
        BCryptCloseAlgorithmProvider(m_alg, 0);
        m_alg = nullptr;
    }
}

bool RSA::GenerateKeyPair(int keySize)
{
    if (!m_alg) return false;

    if (m_key)
    {
        BCryptDestroyKey(m_key);
        m_key = nullptr;
    }

    NTSTATUS status = BCryptGenerateKeyPair(
        m_alg,
        &m_key,
        keySize,
        0
    );

    if (!BCRYPT_SUCCESS(status))
        return false;

    status = BCryptFinalizeKeyPair(m_key, 0);

    return BCRYPT_SUCCESS(status);
}

std::vector<uint8_t> RSA::Encrypt(const std::string& data)
{
    std::vector<uint8_t> result;

    if (!m_key) return result;

    const size_t blockSize = 245; // 2048-bit RSA PKCS1 max plaintext

    for (size_t i = 0; i < data.size(); i += blockSize)
    {
        size_t len = min(blockSize, data.size() - i);

        auto block = EncryptBlock(
            reinterpret_cast<const uint8_t*>(data.data()) + i,
            len
        );

        if (block.empty())
            return {};

        result.insert(result.end(), block.begin(), block.end());
    }

    return result;
}

std::string RSA::Decrypt(const std::vector<uint8_t>& data)
{
    std::string result;

    if (!m_key) return result;

    const size_t blockSize = 256; // 2048-bit RSA ciphertext size

    for (size_t i = 0; i < data.size(); i += blockSize)
    {
        size_t len = min(blockSize, data.size() - i);

        auto block = DecryptBlock(data.data() + i, len);

        if (block.empty())
            return "";

        result.append(block.begin(), block.end());
    }

    return result;
}

std::vector<uint8_t> RSA::EncryptBlock(
    const uint8_t* data,
    size_t size)
{
    ULONG outSize = 0;

    NTSTATUS status = BCryptEncrypt(
        m_key,
        const_cast<PUCHAR>(data),
        static_cast<ULONG>(size),
        nullptr,
        nullptr,
        0,
        nullptr,
        0,
        &outSize,
        PADDING_FLAG
    );

    if (!BCRYPT_SUCCESS(status))
        return {};

    std::vector<uint8_t> out(outSize);

    status = BCryptEncrypt(
        m_key,
        const_cast<PUCHAR>(data),
        static_cast<ULONG>(size),
        nullptr,
        nullptr,
        0,
        out.data(),
        outSize,
        &outSize,
        PADDING_FLAG
    );

    if (!BCRYPT_SUCCESS(status))
        return {};

    return out;
}

std::vector<uint8_t> RSA::DecryptBlock(
    const uint8_t* data,
    size_t size)
{
    ULONG outSize = 0;

    NTSTATUS status = BCryptDecrypt(
        m_key,
        const_cast<PUCHAR>(data),
        static_cast<ULONG>(size),
        nullptr,
        nullptr,
        0,
        nullptr,
        0,
        &outSize,
        PADDING_FLAG
    );

    if (!BCRYPT_SUCCESS(status))
        return {};

    std::vector<uint8_t> out(outSize);

    status = BCryptDecrypt(
        m_key,
        const_cast<PUCHAR>(data),
        static_cast<ULONG>(size),
        nullptr,
        nullptr,
        0,
        out.data(),
        outSize,
        &outSize,
        PADDING_FLAG
    );

    if (!BCRYPT_SUCCESS(status))
        return {};

    return out;
}

std::vector<uint8_t> RSA::ExportPublicKey()
{
    ULONG size = 0;

    NTSTATUS status = BCryptExportKey(
        m_key,
        nullptr,
        BCRYPT_PUBLIC_KEY_BLOB,
        nullptr,
        0,
        &size,
        0
    );

    if (!BCRYPT_SUCCESS(status))
        return {};

    std::vector<uint8_t> blob(size);

    status = BCryptExportKey(
        m_key,
        nullptr,
        BCRYPT_PUBLIC_KEY_BLOB,
        blob.data(),
        size,
        &size,
        0
    );

    if (!BCRYPT_SUCCESS(status))
        return {};

    return blob;
}

std::vector<uint8_t> RSA::ExportPrivateKey()
{
    ULONG size = 0;

    NTSTATUS status = BCryptExportKey(
        m_key,
        nullptr,
        BCRYPT_RSAFULLPRIVATE_BLOB,
        nullptr,
        0,
        &size,
        0
    );

    if (!BCRYPT_SUCCESS(status))
        return {};

    std::vector<uint8_t> blob(size);

    status = BCryptExportKey(
        m_key,
        nullptr,
        BCRYPT_RSAFULLPRIVATE_BLOB,
        blob.data(),
        size,
        &size,
        0
    );

    if (!BCRYPT_SUCCESS(status))
        return {};

    return blob;
}

bool RSA::ImportPublicKey(const std::vector<uint8_t>& blob)
{
    if (!m_alg) return false;

    if (m_key)
    {
        BCryptDestroyKey(m_key);
        m_key = nullptr;
    }

    NTSTATUS status = BCryptImportKeyPair(
        m_alg,
        nullptr,
        BCRYPT_PUBLIC_KEY_BLOB,
        &m_key,
        const_cast<PUCHAR>(blob.data()),
        static_cast<ULONG>(blob.size()),
        0
    );

    return BCRYPT_SUCCESS(status);
}

bool RSA::ImportPrivateKey(const std::vector<uint8_t>& blob)
{
    if (!m_alg) return false;

    if (m_key)
    {
        BCryptDestroyKey(m_key);
        m_key = nullptr;
    }

    NTSTATUS status = BCryptImportKeyPair(
        m_alg,
        nullptr,
        BCRYPT_RSAFULLPRIVATE_BLOB,
        &m_key,
        const_cast<PUCHAR>(blob.data()),
        static_cast<ULONG>(blob.size()),
        0
    );

    return BCRYPT_SUCCESS(status);
}