#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <pwd.h>
#include <time.h>


#define FILE_ERR  1
#define PARAM_ERR 2

#define RESET   std::string("\033[0m")
#define DEFAULT std::string("\033[39m")
#define BLACK   std::string("\033[30m")
#define RED     std::string("\033[31m")
#define GREEN   std::string("\033[32m")
#define YELLOW  std::string("\033[33m")
#define BLUE    std::string("\033[34m")
#define MAGENTA std::string("\033[35m")
#define CYAN    std::string("\033[36m")
#define WHITE   std::string("\033[37m")


int _kbhit() {
  static const int STDIN = 0;
  static bool initialized = false;

  if (! initialized) {
    termios term;
    tcgetattr(STDIN, &term);
    term.c_lflag &= ~ICANON;
    tcsetattr(STDIN, TCSANOW, &term);
    setbuf(stdin, NULL);
    initialized = true;
  }

  int bytesWaiting;
  ioctl(STDIN, FIONREAD, &bytesWaiting);
  return bytesWaiting;
}


std::string getConfigPath() {
  struct passwd *pw = getpwuid(getuid());
  return std::string(pw->pw_dir) + "/.config";
}

void assign(std::vector<std::string>& lines, const std::string& filePath) {
  std::fstream file(filePath);
  std::string buffer;
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << filePath << std::endl;
    exit(FILE_ERR);
  }
  while (std::getline(file, buffer)) {
    lines.push_back(buffer);
  }
}

std::string parseColor(const std::string& colorName) {
  if (colorName == "default") {
    return DEFAULT;
  } else if (colorName == "black") {
    return BLACK;
  } else if (colorName == "red") {
    return RED;
  } else if (colorName == "green") {
    return GREEN;
  } else if (colorName == "blue") {
    return BLUE;
  } else if (colorName == "cyan") {
    return CYAN;
  } else if (colorName == "yellow") {
    return YELLOW;
  } else if (colorName == "magenta") {
    return MAGENTA;
  } else if (colorName == "white") {
    return WHITE;
  }
  return RESET;
}

bool randomBool() {
  srand((unsigned int)time(NULL) + (unsigned int)clock());
  int n = rand();
  return n % 8;
}

int getRandom(int start, int end) {
  srand((unsigned int)time(NULL) + (unsigned int)clock());
  int range = end - start;
  return rand() % range + start;
}

void sleep_ns(unsigned long long ns) {
  struct timespec req, rem;

  req.tv_sec = 0;
  req.tv_nsec = ns;

  while (nanosleep(&req, &rem) == -1) {
    req = rem;
  }
}

int main(int argc, char* argv[]) {
  srand((unsigned int)time(NULL) + (unsigned int)clock());

  std::vector<std::string> goodLines;
  std::vector<std::string> badLines;
  std::string color = DEFAULT;
  const std::string goodConfigPath = getConfigPath() + "/randLog/good";
  const std::string badConfigPath = getConfigPath() + "/randLog/bad";
  unsigned long long microSecs = 500000000;
  bool limited = false;
  long long logCount = 1;

  for (int i = 1; i < argc; i++) {
    std::string param = argv[i];
    if (param == "-c" || param == "--color") {
      if (i != argc - 1)
        color = parseColor(argv[i + 1]);
      else {
        std::cerr << "-c(--color) option requires a parameter" << std::endl;
        return PARAM_ERR;
      } 
      i++;
    } else if (param == "-h" || param == "--help") {
      std::cout << "randLog: a program to output random/fake log output" << std::endl;
      std::cout << "OPTIONS: " << std::endl;
      std::cout << "\t-h, --help: Output this text" << std::endl;
      std::cout << "\t-c, --color: Color of output" << std::endl;
      std::cout << "\t-v, --version: Print version" << std::endl;
      std::cout << "\t-n, --count: Number of logs" << std::endl;
      return 0;
    } else if (param == "-v" || param == "--version") {
      std::cout << "randLog v1.0" << std::endl;
      return 0;
    } else if (param == "-n" || param == "--count") {
      if (i != argc - 1) {
        limited = true;
        logCount = std::stoi(std::string(argv[i + 1]));
      } else {
        std::cerr << "-n(--count) option requires a parameter" << std::endl;
        return PARAM_ERR;
      } 
      i++;
    } else {
      std::cerr << "Unknown option: " << param << std::endl;
      return PARAM_ERR;
    }
  }


  assign(goodLines, goodConfigPath);
  assign(badLines, badConfigPath);

  while (!_kbhit() && (limited ? logCount-- : true)) {
    bool current = randomBool();
    std::cout << color << "[ " << (current ? GREEN +  " OK " + color: RED + "FAIL" + color) << " ] " << (current ? goodLines[getRandom(0, goodLines.size())] : badLines[getRandom(0, badLines.size())]) << std::endl;
    sleep_ns(microSecs);
  }

  return 0;
}

