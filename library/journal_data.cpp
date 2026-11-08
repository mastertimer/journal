#include "journal_data.h"

#include <ctime>
#include <iomanip>
#include <sstream>

std::wstring journal_entry::to_string() const
{
	auto time = std::chrono::system_clock::to_time_t(date_time);
	std::tm local_time{};
	localtime_s(&local_time, &time);

	std::wostringstream text;
	text << std::put_time(&local_time, L"%d.%m.%Y %H:%M")
		<< L" - " << std::fixed << std::setprecision(1) << value;
	return text.str();
}

void journal_data::add_entry(size_t type_index, double value)
{
	if (type_index >= types.size()) return;
	entries.push_back({ std::chrono::system_clock::now(), type_index, value });
	++types[type_index].version;
}

const std::vector<journal_entry>& journal_data::entries_by_type(size_t type_index)
{
	if (type_index >= types.size()) return empty_journal_entries;

	if (cached_entries.size() < types.size())
		cached_entries.resize(types.size());

	auto& cache = cached_entries[type_index];
	const auto& type = types[type_index];
	if (cache.version != type.version)
	{
		cache.entries.clear();
		for (const auto& entry : entries)
			if (entry.type_index == type_index)
				cache.entries.push_back(entry);

		cache.version = type.version;
	}

	return cache.entries;
}
