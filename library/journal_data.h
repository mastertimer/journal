#pragma once

#include <chrono>
#include <string>
#include <vector>

struct journal_type
{
	std::wstring name;
};

struct journal_entry
{
	std::chrono::system_clock::time_point date_time;
	size_t type_index = 0;
	double value = 0.0;
};

struct journal_data
{
	std::vector<journal_entry> entries;
	std::vector<journal_type> types;
};
