#include <node/validator/validator.hpp>

#ifdef _WIN32
#include <bcrypt.h>
#include <windows.h>
#pragma comment(lib, "bcrypt.lib")
#ifndef STATUS_UNSUCCESSFUL
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001L)
#endif
#else
#include <openssl/sha.h>
#endif

#include <fstream>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <iostream>

RequestType Validator::validate_request(const http::request<http::string_body>& request) {
    // Check if the request contains node identification headers
    auto node_id = request.find("pacPrism_node_id");
    auto node_signature = request.find("pacPrism_node_signature");

    // Case 1: Neither header present - this is a plain client request
    if (node_id == request.end() && node_signature == request.end()) {
        return RequestType::PlainClient;
    }

    // Case 2: Both headers present - validate as a node request
    if (node_id != request.end() && node_signature != request.end()) {
        // Verify the signature
        if (verify_node_identity(node_id->value(), node_signature->value(), request.body())) {
            return RequestType::Node;
        } else {
            // Signature verification failed
            return RequestType::Invalid;
        }
    }

    // Case 3: Only one header present - this is invalid
    // (either node_id without signature, or signature without node_id)
    return RequestType::Invalid;
}

bool Validator::verify_node_identity(const std::string& node_id,
                                     const std::string& node_signature,
                                     const std::string& body) {
    // Demo: always return true for now
    // TODO: Implement actual signature verification using Ed25519 or similar
    return true;
}

std::string Validator::calculate_sha256(const std::string& file_path) const {
#ifdef _WIN32
    // Windows implementation using bcrypt.h
    BCRYPT_ALG_HANDLE hAlg = NULL;
    BCRYPT_HASH_HANDLE hHash = NULL;
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    DWORD cbData = 0, cbHash = 0, cbHashObject = 0;
    PBYTE pbHashObject = NULL;
    PBYTE pbHash = NULL;

    // Open algorithm provider
    status = BCryptOpenAlgorithmProvider(
        &hAlg,
        BCRYPT_SHA256_ALGORITHM,
        NULL,
        0
    );
    if (!BCRYPT_SUCCESS(status)) {
        std::cerr << "BCryptOpenAlgorithmProvider failed: " << status << std::endl;
        return "";
    }

    // Calculate hash object size
    status = BCryptGetProperty(
        hAlg,
        BCRYPT_OBJECT_LENGTH,
        (PBYTE)&cbHashObject,
        sizeof(DWORD),
        &cbData,
        0
    );
    if (!BCRYPT_SUCCESS(status)) {
        std::cerr << "BCryptGetProperty (object length) failed: " << status << std::endl;
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return "";
    }

    // Allocate hash object
    pbHashObject = (PBYTE)malloc(cbHashObject);
    if (pbHashObject == NULL) {
        std::cerr << "Failed to allocate hash object" << std::endl;
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return "";
    }

    // Calculate hash length
    status = BCryptGetProperty(
        hAlg,
        BCRYPT_HASH_LENGTH,
        (PBYTE)&cbHash,
        sizeof(DWORD),
        &cbData,
        0
    );
    if (!BCRYPT_SUCCESS(status)) {
        std::cerr << "BCryptGetProperty (hash length) failed: " << status << std::endl;
        free(pbHashObject);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return "";
    }

    // Allocate hash buffer
    pbHash = (PBYTE)malloc(cbHash);
    if (pbHash == NULL) {
        std::cerr << "Failed to allocate hash buffer" << std::endl;
        free(pbHashObject);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return "";
    }

    // Create hash
    status = BCryptCreateHash(
        hAlg,
        &hHash,
        pbHashObject,
        cbHashObject,
        NULL,
        0,
        0
    );
    if (!BCRYPT_SUCCESS(status)) {
        std::cerr << "BCryptCreateHash failed: " << status << std::endl;
        free(pbHashObject);
        free(pbHash);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return "";
    }

    // Read file and hash it
    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        BCryptDestroyHash(hHash);
        free(pbHashObject);
        free(pbHash);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return "";
    }

    const size_t BUFFER_SIZE = 8192;
    char* buffer = new char[BUFFER_SIZE];

    while (file.read(buffer, BUFFER_SIZE)) {
        size_t bytes_read = file.gcount();
        if (bytes_read > 0) {
            status = BCryptHashData(
                hHash,
                (PBYTE)buffer,
                bytes_read,
                0
            );
            if (!BCRYPT_SUCCESS(status)) {
                std::cerr << "BCryptHashData failed: " << status << std::endl;
                delete[] buffer;
                file.close();
                BCryptDestroyHash(hHash);
                free(pbHashObject);
                free(pbHash);
                BCryptCloseAlgorithmProvider(hAlg, 0);
                return "";
            }
        }
    }

    // Handle last partial read
    size_t bytes_read = file.gcount();
    if (bytes_read > 0) {
        status = BCryptHashData(
            hHash,
            (PBYTE)buffer,
            bytes_read,
            0
        );
        if (!BCRYPT_SUCCESS(status)) {
            std::cerr << "BCryptHashData failed: " << status << std::endl;
            delete[] buffer;
            file.close();
            BCryptDestroyHash(hHash);
            free(pbHashObject);
            free(pbHash);
            BCryptCloseAlgorithmProvider(hAlg, 0);
            return "";
        }
    }

    delete[] buffer;
    file.close();

    // Finish hash
    status = BCryptFinishHash(
        hHash,
        pbHash,
        cbHash,
        0
    );
    if (!BCRYPT_SUCCESS(status)) {
        std::cerr << "BCryptFinishHash failed: " << status << std::endl;
        BCryptDestroyHash(hHash);
        free(pbHashObject);
        free(pbHash);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return "";
    }

    // Convert to hex string
    std::stringstream ss;
    for (DWORD i = 0; i < cbHash; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)pbHash[i];
    }

    // Cleanup
    BCryptDestroyHash(hHash);
    free(pbHashObject);
    free(pbHash);
    BCryptCloseAlgorithmProvider(hAlg, 0);

    return ss.str();

#else
    // Linux/Unix implementation using OpenSSL
    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return "";
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    const size_t BUFFER_SIZE = 8192;
    char* buffer = new char[BUFFER_SIZE];

    while (file.read(buffer, BUFFER_SIZE)) {
        size_t bytes_read = file.gcount();
        if (bytes_read > 0) {
            SHA256_Update(&sha256, buffer, bytes_read);
        }
    }

    // Handle last partial read
    size_t bytes_read = file.gcount();
    if (bytes_read > 0) {
        SHA256_Update(&sha256, buffer, bytes_read);
    }

    delete[] buffer;
    file.close();

    SHA256_Final(hash, &sha256);

    // Convert to hex string
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return ss.str();
#endif
}

bool Validator::verify_sha256(const std::string& file_path, const std::string& expected_hash) const {
    std::string calculated_hash = calculate_sha256(file_path);
    if (calculated_hash.empty()) {
        return false;
    }

    // Case-insensitive comparison
    if (calculated_hash.length() != expected_hash.length()) {
        return false;
    }

    for (size_t i = 0; i < calculated_hash.length(); i++) {
        if (tolower(calculated_hash[i]) != tolower(expected_hash[i])) {
            return false;
        }
    }

    return true;
}
