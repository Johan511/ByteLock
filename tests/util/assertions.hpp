#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

namespace util::assertions {

bool assert_eq(std::vector<std::size_t> const &expected,
                     std::vector<std::size_t> const &actual,
                     std::string logFileName = "log.txt") {
  if (expected == actual)
    return true;

  std::ofstream logFile(logFileName);

  if (expected.size() != actual.size()) {
    logFile << "Expected vector size: " << expected.size() << std::endl;
    logFile << "Actual vector size: " << actual.size() << std::endl;
    return false;
  }

  std::size_t n = expected.size();

  logFile << "Expected" << std::setw(10) << std::right << std::setw(10)
          << "Actual\n";

  for (std::size_t i = 0; i != n; i++) {
    char fillChar = ' ';

    if (expected[i] != actual[i]) {
      fillChar = '-';
    }

    logFile << expected[i] << std::setfill(fillChar) << std::setw(10)
            << std::right << std::setfill(fillChar) << std::setw(10)
            << actual[i] << std::endl;
  }
  return false;
}

} // namespace util::assertions