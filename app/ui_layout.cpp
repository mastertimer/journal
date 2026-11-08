#include "ui_layout.h"
#include "ui_elements.h"

void initialize_ui_scene(ui_scene& scene)
{
    auto text = std::make_shared<ui_text>();
    text->set_text(L"Привет");
    text->set_transform({ 1.0, { 100, 50 } });
    scene.root->add_child(text);

    auto text_edit = std::make_shared<ui_text_edit>();
    text_edit->set_transform({ 1.0, { 100, 100 } });
    text_edit->set_text(L"Ввод Текста");
    scene.root->add_child(text_edit);
    scene.keyboard_target = text_edit;

    auto list = std::make_shared<ui_list>();
    list->set_transform({ 1.0, { 100, 150 } });
    scene.root->add_child(list);

    auto button = std::make_shared<ui_button>();
    button->set_transform({ 1.0, { 100, 350 } });
    button->set_caption(L"Кнопка");
    scene.root->add_child(button);
}
