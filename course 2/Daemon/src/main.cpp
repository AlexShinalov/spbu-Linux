#include <mini/ini.h>
#include <filesystem>
#include <iostream>
#include <signal.h>
#include <syslog.h>
#include <thread>
#include <ctime>

bool is_running = true;

const std::string currentDatetime()
{
    time_t now = time(0);
    tm* data = localtime(&now);

    std::ostringstream stream;
    stream << std::put_time(data, "%d.%m.%Y %H-%M-%S");
    return stream.str();
}

const mINI::INIStructure readINI()
{
    mINI::INIFile file("/etc/backup.ini");
    mINI::INIStructure ini;
    file.read(ini);
    if (ini.size() == 0)
    {
        throw std::runtime_error("Failed to read backup.ini file");
    }

    return ini;
}

void backup(const mINI::INIStructure& ini)
{
    const std::string& src = ini.get("src").get("path");
    const std::string& dst = ini.get("dst").get("path"); 

    // Create backup directory
    if (!std::filesystem::exists(dst))
    {
        std::filesystem::create_directory(dst);
    }

    // Copy files
    std::string dateTime = currentDatetime();
    std::string outputPath = dst + dateTime;
    std::filesystem::copy(src, outputPath, std::filesystem::copy_options::recursive);

    std::string log = "Copied " + src + " to " + outputPath;
    syslog(LOG_INFO, log.c_str());
}

void pause_handler(int sig_num)
{
    is_running = false;
    syslog(LOG_INFO, "Pause");
}

void continue_handler(int sig_num)
{
    is_running = true;
    syslog(LOG_INFO, "Continue");
}

void terminate_handler(int sig_num)
{
    syslog(LOG_INFO, "Terminate");
    closelog();
    exit(EXIT_SUCCESS);
}

int main()
{
    // Add signal handlers
    signal(SIGTSTP, pause_handler);
    signal(SIGCONT, continue_handler);
    signal(SIGTERM, terminate_handler);

    // Read configuration file
    const mINI::INIStructure ini = readINI();
    int deltaTime = stoi(ini.get("frequency").get("sec"));

    // Open log file
    openlog("Backup daemon", LOG_PID | LOG_NDELAY, LOG_USER);
    syslog(LOG_INFO, "Start");

    while (true)
    {
        if (is_running)
        {
            backup(ini);
            std::this_thread::sleep_for(std::chrono::seconds(deltaTime));
        }
    }

    return EXIT_SUCCESS;
}
