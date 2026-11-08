#include "ui_layout.h"
#include "ui_elements.h"

void initialize_ui_scene(ui_scene& scene)
{
    scene.data.types.push_back({ L"Вес", 0 });

    auto list = std::make_shared<ui_list>();
    list->set_transform({ 1.0, { 100, 150 } });
    scene.root->add_child(list);

    auto number_input = std::make_shared<ui_number_input>(0.1, 0.0, 500.0, 1);
    number_input->set_transform({ 1.0, { 100, 50 } });
    number_input->set_value(0.0);
    scene.root->add_child(number_input);

    auto label = std::make_shared<ui_text>();
    label->set_text(L"Вес");
    label->set_transform({ 1.0, { -55, 5 } });
    number_input->add_child(label);

    auto accept = std::make_shared<ui_button>();
    accept->set_caption(L"Принять");
    accept->set_transform({ 1.0, { 200, 0 } });
    accept->on_click = [weak = std::weak_ptr<ui_element>(accept)]
    {
        if (auto button = weak.lock())
            if (auto input = dynamic_cast<ui_number_input*>(button->parent))
                if (input->scene) input->scene->data.add_entry(0, input->get_value());
    };
    number_input->add_child(accept);
}
