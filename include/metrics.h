// template_data.h
#pragma once
#include <vector>
#include <string>

struct DataItem {
    std::string name;
    std::string value1;
    std::string value2;
    std::string status;
};

std::vector<DataItem> process_folder(const std::string& folder);
