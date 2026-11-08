#include "ui_layout.h"
#include "ui_elements.h"

void initialize_ui_scene(ui_scene& scene)
{
    scene.data.types.push_back({ L"Вес", 1 });

    auto list = std::make_shared<ui_list>();
    list->set_transform({ 1.0, { 100, 150 } });
    list->update = [&scene, weak = std::weak_ptr<ui_list>(list)]
    {
        auto list = weak.lock();
        if (!list || scene.data.types.empty()) return;
        const auto& type = scene.data.types[0];
        if (list->data_version == type.version) return;

        std::vector<std::wstring> items;
        for (const auto& entry : scene.data.entries_by_type(0))
            items.push_back(entry.to_string());
        list->set_items(std::move(items));
        list->data_version = type.version;
    };
    scene.root->add_child(list);

    const auto& weight_entries = scene.data.entries_by_type(0);
    double last_weight = weight_entries.empty() ? 0.0 : weight_entries.back().value;
    auto number_input = std::make_shared<ui_number_input>(0.1, 0.0, 500.0, 1, last_weight);
    number_input->set_transform({ 1.0, { 100, 50 } });
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
                if (input->scene)
                {
                    input->scene->data.add_entry(0, input->get_value());
                    input->scene->all_update();
                }
    };
    number_input->add_child(accept);

    auto save = std::make_shared<ui_button>();
    save->set_caption(L"Сохранить данные");
    save->set_transform({ 1.0, { 100, 500 } });
    save->on_click = [&scene]
    {
        scene.data.write_to_file(L"e:/test.txt");
    };
    scene.root->add_child(save);
}
