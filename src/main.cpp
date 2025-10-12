#include "TreadmillApp.h"
#include <iostream>

int main()
{
    try
    {
        TreadmillApp app;

        if (!app.initialize())
        {
            std::cerr << "Failed to initialize application" << std::endl;
            return 1;
        }

        app.run();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}
