#ifndef TEMPLATE_MANAGER_H
#define TEMPLATE_MANAGER_H
#include "Common.h"
#include "ConfigManager.h"
class TemplateManager
{
public:
    static void check_if_template_exists(const std::string& template_name)
    {
        std::string template_path = "resources/actor_templates/" + template_name + ".template";
        if (std::filesystem::exists(template_path))
        {
            return;
        }
        else
        {
            std::cout << "error: template " << template_name << " is missing";
            exit(0);
        }
    }
};

#endif // TEMPLATE_MANAGER_H