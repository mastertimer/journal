#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

struct journal_type
{
	std::wstring name;
	std::uint64_t version = 0;
};

struct journal_entry
{
	std::chrono::system_clock::time_point date_time;
	size_t type_index = 0;
	double value = 0.0;
	std::wstring to_string() const;
};

inline const std::vector<journal_entry> empty_journal_entries;

struct journal_data
{
	std::vector<journal_entry> entries;
	std::vector<journal_type> types;

	const std::vector<journal_entry>& entries_by_type(size_t type_index);
	void add_entry(size_t type_index, double value);

private:
	struct type_cache
	{
		std::uint64_t version = 0;
		std::vector<journal_entry> entries;
	};

	std::vector<type_cache> cached_entries;
};
