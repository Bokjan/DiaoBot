#include <unistd.h>
#include <csignal>
#include <thread>
#include "DaemonHeader.hpp"
#include "Log.hpp"
#include "TfcConfigCodec.hpp"

void DoMain(void);

int main(int argc, char *argv[]) {
  if (argc == 1) {
    DiaoBot::PrintUsage(argv[0]);
    exit(EXIT_SUCCESS);
  }
  int ret;
  ret = DiaoBot::MainConf.ParseFile(argv[1]);
  if (ret != DiaoBot::TfcConfigCodec::SUCCESS) {
    LOG("Parse %s failed, retcode: %d", argv[1], ret);
    exit(EXIT_FAILURE);
  }

  // DAEMON?
  string pid_file;
  if (argc > 2) {  // not daemon
    LOG("%s: foreground mode", argv[0]);
    DoMain();
  } else {
    LOG("%s: daemon mode", argv[0]);

    // Same bot is running?
    if (!DiaoBot::MainConf.HasKV("root", "bot_name")) {
      LOG("%s", "Config `root\\bot_name` not found! It's required under daemon mode!");
      exit(EXIT_FAILURE);
    }
    pid_file =
        string("/var/run/").append(DiaoBot::MainConf.GetKV("root", "bot_name")).append(".pid");
    LOG("Assumed pid file is %s", pid_file.c_str());
    do {
      string pid_string = DiaoBot::ReadFile(pid_file.c_str());
      if (pid_string.empty()) {  // OK
        break;
      }
      pid_t prev_pid;
      if (sscanf(pid_string.c_str(), "%d", &prev_pid) != 1) {  // Cannot read a PID integer
        LOG("PID file %s contains no integer!", pid_file.c_str());
        exit(EXIT_FAILURE);
      }
      if (kill(prev_pid, 0) == 0) {  // exist
        LOG("Process with PID %d is running, daemon start failed", prev_pid);
        exit(EXIT_FAILURE);
      }
    } while (false);

    // FORK!
    auto pid = fork();
    if (pid == -1) {
      LOG("%s", "fork() failed!");
      exit(EXIT_FAILURE);
    }
    if (pid != 0) {  // parent
      exit(EXIT_SUCCESS);
    }
    if (setsid() == -1) {
      LOG("%s", "setsid() failed!");
      exit(EXIT_FAILURE);
    }
    FILE *fp = fopen(pid_file.c_str(), "w");
    fprintf(fp, "%d", getpid());
    fclose(fp);

    // Redirect log FP
    if (!DiaoBot::MainConf.HasKV("root\\log", "stderr") ||
        DiaoBot::MainConf.GetKV("root\\log", "stderr").empty() ||
        !DiaoBot::MainConf.HasKV("root\\log", "mode")) {
      LOG("%s", "Config section `root\\log` is ill! It's required under daemon mode!");
      exit(EXIT_FAILURE);
    }
    if (freopen(DiaoBot::MainConf.GetKV("root\\log", "stderr").c_str(),
                DiaoBot::MainConf.GetKV("root\\log", "mode").c_str(), stderr) == nullptr) {
      LOG("%s", "`freopen` failed, check configuration!");
      exit(EXIT_FAILURE);
    }

    // RUN!
    DoMain();
  }
  if (argc == 2) {
    // daemon
    unlink(pid_file.c_str());
  }
  LOG("%s", "Finishing `main`.");
}

void DoMain(void) {
  DiaoBot::LoadSharedObjects();
  DiaoBot::InitializeBotEngine();

  signal(SIGTERM, DiaoBot::GracefulSignalHandler);

  std::thread CronThread(DiaoBot::DoCronThread);
  std::thread HttpThread(DiaoBot::DoHttpThread);
  CronThread.join();
  HttpThread.join();
}
