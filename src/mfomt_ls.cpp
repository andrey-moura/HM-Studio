#include <fstream>
#include <iostream>
#include <iomanip>

uint32_t calls_to_decompressor[] = {
    0x08000cc8,
    0x08000ffc,
    0x08001004,
    0x08001dd8,
    0x0805763c,
    0x08057644,
    0x08057648,
    0x0805764c,
    0x08057654,
    0x0805d8e0,
    0x0805d8e8,
    0x0805d8f0,
    0x0805d8f8,
    0x0805d900,
    0x08062a30,
    0x0807c750,
    0x0807c75c,
    0x080a81dc,
    0x080a81e0,
    0x080a81e4,
    0x080a81e8,
    0x080ae6d0,
    0x080ae6d4,
    0x080ae6d8,
    0x080ae6d8,
    0x080ae6dc,
    0x080ae6e0,
    0x080ae6e4,
    0x080ae6dc,
    0x080ae6dc,
    0x080ae6e0,
    0x080ae6dc,
    0x080ae6e8,
    0x080ae6d8,
    0x080b3ac8,
    0x080b3acc,
    0x080b3ad0,
    0x080ba978,
    0x080ba988,
    0x080ba98c,
    0x080ba990,
    0x080ba998,
    0x080ba9a0,
    0x080bc524,
    0x080bc52c,
    0x080bc534,
    0x080c2f84,
    0x080c2f88,
    0x080c2f8c,
    0x080c75d0,
    0x080c75d4,
    0x080c75d8,
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <rom>" << std::endl;
        return 1;
    }

    std::ifstream input_file(argv[1], std::ios::binary);
    if (!input_file) {
        std::cerr << "Error opening file: " << argv[1] << std::endl;
        return 1;
    }
    for (size_t i = 0; i < sizeof(calls_to_decompressor) / sizeof(calls_to_decompressor[0]); ++i) {
        uint32_t header;
        input_file.seekg(calls_to_decompressor[i] - 0x08000000);
        input_file.read(reinterpret_cast<char*>(&header), sizeof(header));
        //std::cout << "Address 0x" << std::hex << std::setw(8) << std::setfill('0') << calls_to_decompressor[i] << ": " << std::endl;
        if(header > 0x08000000) {
            //std::cout << "Pointer to: 0x" << std::hex << std::setw(8) << std::setfill('0') << (int)header << std::endl;
            input_file.seekg(header - 0x08000000);
            input_file.read(reinterpret_cast<char*>(&header), sizeof(header));
        }
        std::string output;
        std::stringstream ss;
        ss << std::hex << std::setw(8) << std::setfill('0') << (int)header;
        output = ss.str();
        output += ".bin";
        std::string command = "./decompressor '" + std::string(argv[1]) + "' " + std::to_string(calls_to_decompressor[i]) + " " + output;
        //std::cout << "Command: " << command << std::endl;
        system(command.c_str());
        
        // std::cout << "Header 0x" << std::hex << std::setw(8) << std::setfill('0') << (int)header << std::endl;
        // uint32_t target_size = (header >> 8) / 2;
        // std::cout << "Target size: " << std::dec << (int)target_size << " bytes " << std::endl;
        // uint32_t next_word;
        // input_file.read(reinterpret_cast<char*>(&next_word), sizeof(next_word));
        // uint8_t byte = (next_word & 0xFF000000) >> 24;
        // std::cout << "Byte: 0x" << std::hex << std::setw(2) << std::setfill('0') << (int)byte << std::endl;
        // std::cout << std::endl;
    }
    return 0;
}