#include "journal_data.h"
#include "utils.h"

#include <ctime>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <sstream>
#include <utility>

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

bool journal_data::write_to_file(const std::filesystem::path& file_path) const
{
	std::ofstream file(file_path, std::ios::binary | std::ios::trunc);
	if (!file) return false;

	std::wstring text;
	for (const auto& type : types)
	{
		std::wostringstream line;
		line << std::quoted(crypt(type.name)) << L' ' << type.decimal_places << L'\n';
		text += line.str();
	}
	text += L"---\n";

	for (const auto& entry : entries)
	{
		if (entry.type_index >= types.size()) return false;
		const std::time_t time = std::chrono::system_clock::to_time_t(entry.date_time);
		std::tm local_time{};
		if (localtime_s(&local_time, &time) != 0) return false;

		std::wostringstream line;
		line << std::put_time(&local_time, L"%d.%m.%Y %H:%M") << L' '
			<< entry.type_index << L' ' << std::fixed
			<< std::setprecision(types[entry.type_index].decimal_places) << entry.value << L'\n';
		text += line.str();
	}

	const std::string bytes = wstring_to_string(text);
	file.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
	return file.good();
}

bool journal_data::read_from_file(const std::filesystem::path& file_path)
{
	std::ifstream file(file_path, std::ios::binary);
	if (!file) return false;
	const std::string bytes((std::istreambuf_iterator<char>(file)), {});

	const std::wstring text = string_to_wstring(bytes);

	std::wistringstream input(text);
	std::vector<journal_type> loaded_types;
	std::vector<journal_entry> loaded_entries;
	std::wstring line;
	bool separator_found = false;
	while (std::getline(input, line))
	{
		if (!line.empty() && line.back() == L'\r') line.pop_back();
		if (line == L"---")
		{
			separator_found = true;
			break;
		}

		std::wistringstream type_line(line);
		std::wstring encoded_name;
		journal_type type;
		if (!(type_line >> std::quoted(encoded_name) >> type.decimal_places)) return false;
		type.name = crypt(std::move(encoded_name));
		type.version = 1;
		loaded_types.push_back(std::move(type));
	}
	if (!separator_found) return false;

	while (std::getline(input, line))
	{
		if (!line.empty() && line.back() == L'\r') line.pop_back();
		if (line.empty()) continue;

		std::wistringstream entry_line(line);
		std::tm local_time{};
		size_t type_index = 0;
		double value = 0.0;
		if (!(entry_line >> std::get_time(&local_time, L"%d.%m.%Y %H:%M") >> type_index >> value)
			|| type_index >= loaded_types.size()) return false;

		const std::time_t time = std::mktime(&local_time);
		if (time == static_cast<std::time_t>(-1)) return false;
		loaded_entries.push_back({ std::chrono::system_clock::from_time_t(time), type_index, value });
	}

	types = std::move(loaded_types);
	entries = std::move(loaded_entries);
	cached_entries.clear();
	return true;
}
