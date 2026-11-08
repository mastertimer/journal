#include "utils.h"
#include <unordered_map>

uint64_t next_id()
{
    static std::atomic<uint64_t> id = 0;
    return ++id;
}

std::wstring crypt(std::wstring s)
{
    static const std::unordered_map<wchar_t, wchar_t> map =
    {
        {L' ', L'Я'}, {L'$', L')'}, {L'%', L'л'}, {L'&', L'В'}, {L'(', L'и'}, {L')', L'$'}, {L'*', L'0'}, {L'+', L'П'},
        {L'-', L'б'}, {L'/', L'х'}, {L'0', L'*'}, {L'1', L'`'}, {L'2', L'А'}, {L'3', L'u'}, {L'4', L'g'}, {L'5', L'Ы'},
        {L'6', L'д'}, {L'7', L'ь'}, {L'8', L'ы'}, {L'9', L'N'}, {L':', L'Т'}, {L';', L'з'}, {L'=', L'а'}, {L'?', L'Е'},
        {L'@', L'м'}, {L'A', L'Э'}, {L'B', L'ю'}, {L'C', L'ж'}, {L'D', L'Б'}, {L'E', L'Ш'}, {L'F', L'о'}, {L'G', L'v'},
        {L'H', L'e'}, {L'I', L'S'}, {L'J', L'r'}, {L'K', L'ц'}, {L'L', L'щ'}, {L'M', L'}'}, {L'N', L'9'}, {L'O', L'З'},
        {L'P', L'Щ'}, {L'Q', L'Ь'}, {L'R', L'X'}, {L'S', L'I'}, {L'T', L'|'}, {L'U', L'Ю'}, {L'V', L'ш'}, {L'W', L'h'},
        {L'X', L'R'}, {L'Y', L'У'}, {L'Z', L'Р'}, {L'[', L'с'}, {L']', L'н'}, {L'^', L'_'}, {L'_', L'^'}, {L'`', L'1'},
        {L'a', L'z'}, {L'b', L'С'}, {L'c', L'М'}, {L'd', L'Д'}, {L'e', L'H'}, {L'f', L'm'}, {L'g', L'4'}, {L'h', L'W'},
        {L'i', L'Г'}, {L'j', L't'}, {L'k', L'Ё'}, {L'l', L'О'}, {L'm', L'f'}, {L'n', L'ф'}, {L'o', L'Ф'}, {L'p', L'Ж'},
        {L'q', L'К'}, {L'r', L'J'}, {L's', L'п'}, {L't', L'j'}, {L'u', L'3'}, {L'v', L'G'}, {L'w', L'Н'}, {L'x', L'у'},
        {L'y', L'г'}, {L'z', L'a'}, {L'{', L'я'}, {L'|', L'T'}, {L'}', L'M'}, {L'~', L'И'}, {L'Ё', L'k'}, {L'А', L'2'},
        {L'Б', L'D'}, {L'В', L'&'}, {L'Г', L'i'}, {L'Д', L'd'}, {L'Е', L'?'}, {L'Ж', L'p'}, {L'З', L'O'}, {L'И', L'~'},
        {L'Й', L'Х'}, {L'К', L'q'}, {L'Л', L'ё'}, {L'М', L'c'}, {L'Н', L'w'}, {L'О', L'l'}, {L'П', L'+'}, {L'Р', L'Z'},
        {L'С', L'b'}, {L'Т', L':'}, {L'У', L'Y'}, {L'Ф', L'o'}, {L'Х', L'Й'}, {L'Ц', L'р'}, {L'Ч', L'ч'}, {L'Ш', L'E'},
        {L'Щ', L'P'}, {L'Ъ', L'э'}, {L'Ы', L'5'}, {L'Ь', L'Q'}, {L'Э', L'A'}, {L'Ю', L'U'}, {L'Я', L' '}, {L'а', L'='},
        {L'б', L'-'}, {L'в', L'т'}, {L'г', L'y'}, {L'д', L'6'}, {L'е', L'ъ'}, {L'ж', L'C'}, {L'з', L';'}, {L'и', L'('},
        {L'й', L'к'}, {L'к', L'й'}, {L'л', L'%'}, {L'м', L'@'}, {L'н', L']'}, {L'о', L'F'}, {L'п', L's'}, {L'р', L'Ц'},
        {L'с', L'['}, {L'т', L'в'}, {L'у', L'x'}, {L'ф', L'n'}, {L'х', L'/'}, {L'ц', L'K'}, {L'ч', L'Ч'}, {L'ш', L'V'},
        {L'щ', L'L'}, {L'ъ', L'е'}, {L'ы', L'8'}, {L'ь', L'7'}, {L'э', L'Ъ'}, {L'ю', L'B'}, {L'я', L'{'}, {L'ё', L'Л'},
    };

    for (wchar_t& c : s) if (auto it = map.find(c); it != map.end()) c = it->second;
    return s;
}

std::wstring string_to_wstring(std::string_view s)
{
	static const wchar_t conv[256] = {
		L'\x0', L'\x1', L'\x2', L'\x3', L'\x4', L'\x5', L'\x6', L'\x7', L'\x8', L'\x9', L'\xa', L'\xb', L'\xc', L'\xd',
		L'\xe', L'\xf', L'\x10', L'\x11', L'\x12', L'\x13', L'\x14', L'\x15', L'\x16', L'\x17', L'\x18', L'\x19',
		L'\x1a', L'\x1b', L'\x1c', L'\x1d', L'\x1e', L'\x1f', L' ', L'!', L'"', L'#', L'$', L'%', L'&', L'\'', L'(',
		L')', L'*', L'+', L',', L'-', L'.', L'/', L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L':',
		L';', L'<', L'=', L'>', L'?', L'@', L'A', L'B', L'C', L'D', L'E', L'F', L'G', L'H', L'I', L'J', L'K', L'L',
		L'M', L'N', L'O', L'P', L'Q', L'R', L'S', L'T', L'U', L'V', L'W', L'X', L'Y', L'Z', L'[', L'\\', L']', L'^',
		L'_', L'`', L'a', L'b', L'c', L'd', L'e', L'f', L'g', L'h', L'i', L'j', L'k', L'l', L'm', L'n', L'o', L'p',
		L'q', L'r', L's', L't', L'u', L'v', L'w', L'x', L'y', L'z', L'{', L'|', L'}', L'~', L' ', L' ', L' ', L' ',
		L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ',
		L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ',
		L' ', L'Ё', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L'ё',
		L' ', L' ', L' ', L' ', L' ', L' ', L' ', L'А', L'Б', L'В', L'Г', L'Д', L'Е', L'Ж', L'З', L'И', L'Й', L'К',
		L'Л', L'М', L'Н', L'О', L'П', L'Р', L'С', L'Т', L'У', L'Ф', L'Х', L'Ц', L'Ч', L'Ш', L'Щ', L'Ъ', L'Ы', L'Ь',
		L'Э', L'Ю', L'Я', L'а', L'б', L'в', L'г', L'д', L'е', L'ж', L'з', L'и', L'й', L'к', L'л', L'м', L'н', L'о',
		L'п', L'р', L'с', L'т', L'у', L'ф', L'х', L'ц', L'ч', L'ш', L'щ', L'ъ', L'ы', L'ь', L'э', L'ю', L'я'
	};
	std::wstring result(s.size(), L' ');
	for (size_t i = 0; i < s.size(); ++i)
		result[i] = conv[static_cast<unsigned char>(s[i])];
	return result;
}

std::string wstring_to_string(std::wstring_view s)
{
	static const std::unordered_map<wchar_t, unsigned char> reverse = []
	{
		std::unordered_map<wchar_t, unsigned char> map;
		for (int i = 0; i < 256; ++i)
		{
			const char byte = static_cast<char>(i);
			const wchar_t character = string_to_wstring(std::string_view(&byte, 1))[0];
			map.emplace(character, static_cast<unsigned char>(i));
		}
		return map;
	}();
	std::string result(s.size(), '?');
	for (size_t i = 0; i < s.size(); ++i)
		if (auto it = reverse.find(s[i]); it != reverse.end())
			result[i] = static_cast<char>(it->second);
	return result;
}
