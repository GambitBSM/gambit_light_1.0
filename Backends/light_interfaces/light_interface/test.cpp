#include <map>
#include <iostream>

extern "C"
void run(const std::map<std::string,double>& input, std::map<std::string,double>& output);

int main()
{
    std::map<std::string,double> input;
    std::map<std::string,double> output;

    input.insert({"p1", 1});
    input.insert({"p2", 1});
    input.insert({"p3", 1});
    input.insert({"p4", 1});
    input.insert({"p5", 1});
    input.insert({"p6", 1});

    run(input, output);
    std::cout << "output parameters: " << std::endl;
    for (const auto& fn : output){
        std::cout << " - " << fn.first << ": " << fn.second << std::endl;
    }
}
