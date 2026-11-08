#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

struct journal_type
{
	std::wstring name;
	unsigned int decimal_places = 0;
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
	bool write_to_file(const std::filesystem::path& file_path) const;
	bool read_from_file(const std::filesystem::path& file_path);
	void add_entry(size_t type_index, double value);

private:
	struct type_cache
	{
		std::uint64_t version = 0;
		std::vector<journal_entry> entries;
	};

	std::vector<type_cache> cached_entries;
};
