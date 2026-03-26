#include <vtr_db_extractor/utils.hpp>
// #pragma once


namespace fs = std::filesystem;


BagExtractor::BagExtractor(const std::string& bag_path, const std::string& bag_name) : bag_path_(bag_path) {
    std::string db_path = bag_path +"/"+ bag_name;
    std::string metadata_path = bag_path + "/metadata.yaml";
    
    std::cout << "DB Path: " << db_path << std::endl;
    
    // Check if paths exist
    if (!fs::exists(db_path)) {
        std::cerr << "Error: Database file not found at " << db_path << std::endl;
        exit(1);
    }
    if (!fs::exists(metadata_path)) {
        std::cerr << "Error: Metadata file not found at " << metadata_path << std::endl;
        // exit(1);
    }
    // Load metadata
    try {
        metadata_ = YAML::LoadFile(metadata_path);
    } catch (const YAML::Exception& e) {
        std::cerr << "Error loading YAML: " << e.what() << std::endl;
        // exit(1);
    }
    // Connect to the database
    int rc = sqlite3_open(db_path.c_str(), &db_);
    if (rc) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db_) << std::endl;
        exit(1);
    }
    // Get available topics
    get_topics();
}

BagExtractor::~BagExtractor() {
    if (db_) {
        sqlite3_close(db_);
    }
}





void BagExtractor::get_topics() {
    const char* query = "SELECT id, name, type FROM topics";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db_, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(db_) << std::endl;
        return;
    }
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int topic_id = sqlite3_column_int(stmt, 0);
        std::string topic_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string topic_type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        
        topics_[topic_id] = {topic_name, topic_type};
    }
    
    sqlite3_finalize(stmt);
}


void clearOutputFolder(const std::string& folderPath) {
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        std::filesystem::remove(entry.path());
    }
}