#include "ui_scene.h"

#include <map>
#include <unordered_map>
#include <vector>
#include <random>
#include <string>

struct Cipher
{
    std::map<wchar_t, wchar_t> map;

    Cipher()
    {
        std::vector<wchar_t> chars;

        // English
        for (wchar_t c = L'A'; c <= L'Z'; ++c) chars.push_back(c);
        for (wchar_t c = L'a'; c <= L'z'; ++c) chars.push_back(c);

        // Russian
        for (wchar_t c = L'А'; c <= L'Я'; ++c) chars.push_back(c);
        for (wchar_t c = L'а'; c <= L'я'; ++c) chars.push_back(c);

        // digits
        for (wchar_t c = L'0'; c <= L'9'; ++c) chars.push_back(c);

        // some symbols
        std::wstring symbols = L" `~@$%^&*()-_=+{}[]:;|/?Ёё";
        for (wchar_t c : symbols) chars.push_back(c);

        // shuffle
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(chars.begin(), chars.end(), gen);

        // pair mapping (involution)
        for (size_t i = 0; i + 1 < chars.size(); i += 2)
        {
            wchar_t a = chars[i];
            wchar_t b = chars[i + 1];

            map[a] = b;
            map[b] = a;
        }

        // если нечетное количество — последний сам себе
        if (chars.size() % 2 == 1)
        {
            wchar_t last = chars.back();
            map[last] = last;
        }
    }

    std::wstring crypt(std::wstring s)
    {
        for (wchar_t& c : s)
        {
            auto it = map.find(c);
            if (it != map.end())
                c = it->second;
        }
        return s;
    }
};

std::wstring crypt2(std::wstring s)
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



    for (wchar_t& c : s)
    {
        auto it = map.find(c);
        if (it != map.end())
            c = it->second;
    }
    return s;
}


#include <fstream>
#include <map>
#include <string>
#include <locale>
#include <codecvt>

std::string wch_to_utf8(wchar_t ch)
{
    char buffer[8];

    int len = WideCharToMultiByte(
        CP_UTF8,
        0,
        &ch,
        1,
        buffer,
        sizeof(buffer),
        nullptr,
        nullptr);

    return std::string(buffer, len);
}

void save_map_as_cpp_utf8(
    const std::map<wchar_t, wchar_t>& map,
    const std::string& filename)
{
    std::ofstream file(filename, std::ios::binary);

    file << "static const std::map<wchar_t, wchar_t> map =\n";
    file << "{\n";

    for (const auto& [src, dst] : map)
    {
        file << "    {L'"
            << wch_to_utf8(src)
            << "', L'"
            << wch_to_utf8(dst)
            << "'},\n";
    }

    file << "};\n";
}

recti ui_scene::draw(size2i s)
{
    //Cipher aa;
    //save_map_as_cpp_utf8(
    //    aa.map,
    //    "e:/text.txt");

	if (canvas.resize(s)) changed_rect = s;
	if (changed_rect.empty()) return {};
	canvas.set_drawing_rect(changed_rect);
	changed_rect &= canvas.get_size();
	canvas.clear(background_color);
	if (root) root->draw({});
	recti result = changed_rect;
	changed_rect = rect();
	return result;
}
