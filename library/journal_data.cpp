#include "journal_data.h"

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
