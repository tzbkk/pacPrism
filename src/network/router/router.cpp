#include <memory>
#include <format>
#include <variant>
#include <sstream>

#include <node/dht/dht_operation.hpp>
#include <node/validator/validator.hpp>
#include <network/router/router.hpp>
#include <console/io/io.hpp>
#include <pacPrism/version.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Router::Router(DHT_operation& dht, Validator& validator, FileCache& cache)
    : m_dht(dht), m_validator(validator), m_cache(cache) {};

router_response Router::global_router(const http::request<http::string_body>& request) {
    // Delegate validation to Validator
    auto request_type = m_validator.validate_request(request);

    // Route based on validated request type
    switch (request_type) {
        case RequestType::PlainClient:
            return plain_response_router(request);
        case RequestType::Node:
            return node_response_router(request);
        case RequestType::Invalid:
            return default_response_builder("Invalid request.", request.version(), http::status::bad_request);
        default:
            return default_response_builder("Internal server error.", request.version(), http::status::internal_server_error);
    }
}

router_response Router::node_response_router(const http::request<http::string_body>& request) {
    // Parse the request target for API path
    std::string target = std::string(request.target());
    json response_json;
    http::status status_code = http::status::ok;

    try {
        // API routes for DHT operations
        // Format: /api/dht/{operation}

        // Extract path components
        if (target.find("/api/dht/") == 0) {
            std::string path = target.substr(9); // Remove "/api/dht/"
            std::string operation;
            std::string params;

            // Split operation and parameters
            size_t query_pos = path.find('?');
            if (query_pos != std::string::npos) {
                operation = path.substr(0, query_pos);
                params = path.substr(query_pos + 1);
            } else {
                // Check if this is a "clean/" operation (which contains a slash)
                if (path.find("clean/") == 0) {
                    operation = path; // Keep the whole path as operation
                    params = "";
                } else {
                    size_t slash_pos = path.find('/');
                    if (slash_pos != std::string::npos) {
                        operation = path.substr(0, slash_pos);
                        params = path.substr(slash_pos + 1);
                    } else {
                        operation = path;
                    }
                }
            }

            // Handle different DHT operations
            if (operation == "verify" && !params.empty()) {
                // GET /api/dht/verify/{node_id}
                bool exists = m_dht.verify_entry(params);
                response_json = {
                    {"operation", "verify"},
                    {"node_id", params},
                    {"exists", exists}
                };
                status_code = http::status::ok;
            }
            else if (operation == "store" && request.method() == http::verb::post) {
                // POST /api/dht/store
                // Expected JSON body with dht_entry
                try {
                    json request_json = json::parse(request.body());
                    dht_entry entry = request_json.get<dht_entry>();
                    m_dht.store_entry(entry);
                    response_json = {
                        {"operation", "store"},
                        {"status", "success"},
                        {"node_id", entry.node_id}
                    };
                    status_code = http::status::created;
                } catch (const json::exception& e) {
                    response_json = {
                        {"operation", "store"},
                        {"status", "error"},
                        {"message", "Invalid JSON body"}
                    };
                    status_code = http::status::bad_request;
                }
            }
            else if (operation == "query") {
                // GET /api/dht/query?shard_id={id}
                // Parse shard_id from query parameters
                std::string shard_id;
                size_t shard_pos = params.find("shard_id=");
                if (shard_pos != std::string::npos) {
                    shard_pos += 9; // Skip "shard_id="
                    size_t shard_end = params.find('&', shard_pos);
                    if (shard_end == std::string::npos) {
                        shard_id = params.substr(shard_pos);
                    } else {
                        shard_id = params.substr(shard_pos, shard_end - shard_pos);
                    }
                }

                if (!shard_id.empty()) {
                    const std::set<std::string>* node_ids = m_dht.query_node_ids_by_shard_id(shard_id);
                    if (node_ids != nullptr) {
                        response_json = {
                            {"operation", "query"},
                            {"shard_id", shard_id},
                            {"node_ids", *node_ids}
                        };
                    } else {
                        response_json = {
                            {"operation", "query"},
                            {"shard_id", shard_id},
                            {"node_ids", std::set<std::string>()}
                        };
                    }
                    status_code = http::status::ok;
                } else {
                    response_json = {
                        {"operation", "query"},
                        {"status", "error"},
                        {"message", "Missing shard_id parameter"}
                    };
                    status_code = http::status::bad_request;
                }
            }
            else if (operation == "clean/expiry" && request.method() == http::verb::post) {
                // POST /api/dht/clean/expiry
                m_dht.clean_by_expiry_time();
                response_json = {
                    {"operation", "clean/expiry"},
                    {"status", "success"},
                    {"message", "Expired entries cleaned"}
                };
                status_code = http::status::ok;
            }
            else if (operation == "clean/liveness" && request.method() == http::verb::post) {
                // POST /api/dht/clean/liveness
                m_dht.clean_by_liveness();
                response_json = {
                    {"operation", "clean/liveness"},
                    {"status", "success"},
                    {"message", "Unhealthy entries cleaned"}
                };
                status_code = http::status::ok;
            }
            else {
                response_json = {
                    {"status", "error"},
                    {"message", "Unknown DHT operation"}
                };
                status_code = http::status::not_found;
            }
        } else {
            response_json = {
                {"status", "error"},
                {"message", "Invalid API path"}
            };
            status_code = http::status::bad_request;
        }
    } catch (const std::exception& e) {
        response_json = {
            {"status", "error"},
            {"message", e.what()}
        };
        status_code = http::status::internal_server_error;
    }

    // Build JSON response
    auto response = std::make_shared<http::response<http::string_body>>(status_code, request.version());
    response->set(http::field::content_type, "application/json");
    response->set("server", std::format("pacPrism/{}", pacprism::getVersionFull()));
    response->body() = response_json.dump(4); // Pretty print with 4-space indent
    response->prepare_payload();

    return response;
}

router_response Router::plain_response_router(const http::request<http::string_body>& request) {
    // Get the request target (path + query)
    std::string request_target = request.target();

    // Check if request has 'target' parameter in query string
    std::string target;
    auto target_pos = request_target.find('?');
    if (target_pos != std::string::npos) {
        // Parse query string for target parameter
        std::string query_string = request_target.substr(target_pos + 1);
        size_t target_start = query_string.find("target=");
        if (target_start != std::string::npos) {
            target_start += 7; // Skip "target="
            size_t target_end = query_string.find('&', target_start);
            if (target_end == std::string::npos) {
                target = query_string.substr(target_start);
            } else {
                target = query_string.substr(target_start, target_end - target_start);
            }
        }
    }

    // Check for Range header
    std::string range_header;
    auto range_it = request.find(http::field::range);
    if (range_it != request.end()) {
        range_header = std::string(range_it->value());
    }

    // Check for conditional request headers
    std::string if_modified_since, if_none_match;
    auto ims_it = request.find(http::field::if_modified_since);
    if (ims_it != request.end()) {
        if_modified_since = std::string(ims_it->value());
    }
    auto inm_it = request.find(http::field::if_none_match);
    if (inm_it != request.end()) {
        if_none_match = std::string(inm_it->value());
    }

    // Determine if we have conditional headers
    bool has_conditional = !if_modified_since.empty() || !if_none_match.empty();

    // If target parameter exists, try to serve from cache
    if (!target.empty()) {
        // Request has target parameter (e.g., /?target=/debian/pool/...)
        std::string target_path = (target[0] == '/') ? target : "/" + target;

        std::shared_ptr<http::response<http::file_body>> file_response;

        // Priority: Range > Conditional > Normal
        if (!range_header.empty()) {
            // Range request takes priority
            file_response = m_cache.get_or_fetch_with_range(target_path, request.version(), range_header);
        } else if (has_conditional) {
            // Conditional request
            file_response = m_cache.get_or_fetch_with_conditional(target_path, request.version(), if_modified_since, if_none_match);
        } else {
            // Normal request
            file_response = m_cache.get_or_fetch(target_path, request.version());
        }

        if (file_response) {
            return file_response;
        }
        return default_response_builder("Failed to fetch file from upstream.", request.version(), http::status::bad_gateway);
    }

    // No target parameter in query string
    // Check if the request path is not just "/" (i.e., it's a file request)
    std::string path = (target_pos != std::string::npos) ? request_target.substr(0, target_pos) : request_target;

    if (path != "/" && !path.empty()) {
        // This is a direct file path request (e.g., /debian/pool/main/...)
        // Try to serve from cache

        std::shared_ptr<http::response<http::file_body>> file_response;

        // Priority: Range > Conditional > Normal
        if (!range_header.empty()) {
            // Range request takes priority
            file_response = m_cache.get_or_fetch_with_range(path, request.version(), range_header);
        } else if (has_conditional) {
            // Conditional request
            file_response = m_cache.get_or_fetch_with_conditional(path, request.version(), if_modified_since, if_none_match);
        } else {
            // Normal request
            file_response = m_cache.get_or_fetch(path, request.version());
        }

        if (file_response) {
            return file_response;
        }
        return default_response_builder("Failed to fetch file from upstream.", request.version(), http::status::bad_gateway);
    }

    // Request is just "/" with no target parameter - return hello message
    return default_response_builder("Hello from pacPrism!", request.version(), http::status::ok);
}

router_response Router::default_response_builder(const std::string& body_string, size_t version, http::status status) {
    auto response = std::make_shared<http::response<http::string_body>>(status, version);
    response->body() = body_string;
    response->set("server", std::format("pacPrism/{}", pacprism::getVersionFull()));
    response->prepare_payload();
    return response;
}
