#include <algorithm>
#include <string>
#include <vector>



// argument parsing (https://stackoverflow.com/a/868894/10180569)
class InputParser
{
  public:
    InputParser(int & argc, char ** argv)
    {
        for (int i = 1; i < argc; ++i)
        {
            this->tokens.push_back(std::string(argv[i]));
        }
    }

    /// @author iain
    std::string const & getCmdOption(std::string const & option) const
    {
        std::vector<std::string>::const_iterator itr;
        itr = std::find(this->tokens.begin(), this->tokens.end(), option);
        if (itr != this->tokens.end() && ++itr != this->tokens.end())
        {
            return *itr;
        }
        static const std::string empty_string("");
        return empty_string;
    }

    /// @author iain
    bool cmdOptionExists(std::string const & option) const
    {
        return std::find(this->tokens.begin(), this->tokens.end(), option) != this->tokens.end();
    }

  private:
    std::vector<std::string> tokens;
};
