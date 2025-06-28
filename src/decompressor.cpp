#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <iomanip>

// Check if a value is divisible by 2 (even)
inline static bool is_divisible_by_two(uint32_t value) {
    return (value & 1) == 0;
}

// Lê um número de bits de um buffer de bits
class BitReader {
    std::ifstream* input;
    uint32_t m_buffer;
    uint32_t m_available_bits;
public:
    BitReader() : input(nullptr), m_buffer(0), m_available_bits(0) {}
    BitReader(std::ifstream& in) : input(&in), m_buffer(0), m_available_bits(0) {}
private:
    void private_read(void* data, uint32_t count) {
        if(!input->read(reinterpret_cast<char*>(data), count)) {
            throw std::runtime_error("Failed to read from input stream.");
        }
        m_available_bits = count * 8;
    }
    // 080D8DCC
    uint32_t private_read_bits(uint32_t count) {
        const uint32_t buffer_size = sizeof(m_buffer) * 8;
        uint32_t bytes_to_shift_right = buffer_size - count;
        uint32_t result = (m_buffer >> bytes_to_shift_right);
        result &= (1 << count) - 1; // Limit the result to the requested number of bits
        m_available_bits -= count;
        m_buffer <<= count; // Shift the buffer to the right by the number of bits read
        return result;
    }
public:
    uint32_t read_bits(uint32_t count) {
        if(!m_available_bits) {
            private_read(&m_buffer, sizeof(m_buffer));
        }
        
        if(m_available_bits >= count) {
            return private_read_bits(count);
        }

        uint32_t more_bits_to_read = count - m_available_bits;
        uint32_t all_available_bits = private_read_bits(m_available_bits);

        uint32_t more_bits = read_bits(more_bits_to_read);

        uint32_t result = all_available_bits << more_bits_to_read;
        result |= more_bits;

        return result;
    }
    uint32_t read_all_available_bits() {
        return read_bits(m_available_bits);
    }
    uint8_t read_byte() {
        return read_bits(8);
    }
    uint16_t read_uint16() {
        return read_bits(16);
    }
    bool has_available_bits() const {
        return m_available_bits > 0;
    }
    uint32_t available_bits() const {
        return m_available_bits;
    }
    uint32_t buffer() const {
        return m_buffer;
    }
    auto address() const {
        return input->tellg();
    }
};

std::vector<uint16_t> output_buffer;
uint8_t* current_write_ptr = nullptr;
uint8_t* end_write_ptr = nullptr;
BitReader bit_reader;
std::vector<uint8_t> stack;

void write_u8_to_output(uint8_t value) {
    *current_write_ptr = value;
}

void write_u16_to_output(uint16_t value) {
    *((uint16_t*)(current_write_ptr)) = value;
}

// FUN_080d8dae
static uint16_t find_negative() {
    int16_t var4 = 0x20;

    while(1) {
        // LAB_080D8DB4
        //uint32_t var5 = buffer >> 0x1F;
        uint32_t var5 = bit_reader.read_bits(1);
        //std::cout << "LAB_080D8DB6 R2: " << std::hex << (int)bit_reader.buffer() << std::endl;
        //std::cout << "LAB_080D8DB4 R5: " << std::hex << (int)var5 << std::endl;
        // LAB_080D8DB8
        var5 <<= 1;
        //std::cout << "LAB_080D8DB8 R5: " << std::hex << (int)var5 << std::endl;
        // LAB_080D8DBA
        var5 = var5 + var4;
        //std::cout << "LAB_080D8DB6 R5: " << std::hex << (int)var5 << std::endl;
        // LAB_080D8DBC
        var4 = *(int16_t*)(stack.data() + var5);
        //std::cout << "LAB_080D8DBC R4: " << std::hex << (int)var4 << std::endl;

        // 080D8DBE
        if(var4 >= 0) {
            continue;
        }

        // LAB_080D8DC2
        var4 = ~var4;
        //std::cout << "LAB_080D8DC2 R4: " << std::hex << (int)var4 << std::endl;

        return var4;
    }

    // 080D8DC6
    throw std::runtime_error("Not implemented yet.");
};

static void display_mem(std::string_view title, uint8_t* buffer, size_t size)
{
    std::cout << std::endl;
    std::cout << title;
    std::cout << std::endl;
    int i = 0;
    for (int y = 0; y < 8; ++y) { 
        for(int x = 0; x < 16; ++x) {
            if(i >= size) {
                break;
            }
            if(x) {
                std::cout << " ";
            }
            if(x == 8) {
                std::cout << "   "; // Extra space for better alignment
            }
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)buffer[i] << " ";
            i++;
        }
        std::cout << std::endl;
        if(i >= size) {
            break;
        }
    }

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
}

static void display_stack()
{
    display_mem("Stack", stack.data(), stack.size());
}

static void display_output_buffer()
{
    display_mem("Output Buffer", (uint8_t*)output_buffer.data(), output_buffer.size() * sizeof(uint16_t));
}

static void FUN_080D9084(uint32_t& bytes_written, uint8_t* output_buffer_byte_ptr, uint32_t& var4, uint32_t& var5, uint32_t& var6, uint32_t& var10)
{
    // 080D9088
    uint32_t var2 = var10;

    // 080D908A
    if(var5 != 1) {
        // 080D908E
        if(is_divisible_by_two(var5)) {
            // 080D9092
            var5 = bytes_written - var5;
            // 080D9094
            if(!is_divisible_by_two(var5)) {
                var5++;
                // 080D9098
                uint32_t var4 = *((uint16_t*)(output_buffer_byte_ptr + var5));
                // 080D909C
                var4 >>= 8;
                // 080D909E
                var4 <<= 8;
                // 080D90A0
                var4 |= var2;
                // 080D90A2
                bytes_written++;
                // 080D90A4
                *((uint16_t*)(output_buffer_byte_ptr + bytes_written)) = var4;

                // 080D90A6
                var6 -= 1;

                // 080D90A8
                if(var6 != 0) {
                    while((int16_t)var6 > 0) {
                        // 080D90AA
                        var5 += 2;
                        // 080D90AC
                        var4 = *((uint16_t*)(output_buffer_byte_ptr + var5));
                        // 080D90AE
                        bytes_written+=2;
                        // 080D90B0
                        *((uint16_t*)(output_buffer_byte_ptr + bytes_written)) = var4;
                        // 080D90B2
                        var6 -= 2;
                    }

                    // 080D90B6
                    var4 <<= 0x18;
                    // 080D90B8
                    var2 = var4 >> 0x18;
                    // 080D90BA
                    bytes_written += var6;
                    // 080D90BC
                    var10 = var2;
                } else {
                    throw std::runtime_error("Not implemented yet.");
                }
            } else {
                while((int16_t)var6 > 0) {
                    // 080D90AA
                    var5 += 2;
                    // 080D90AC
                    var4 = *((uint16_t*)(output_buffer_byte_ptr + var5));
                    // 080D90AE
                    bytes_written += 2;
                    // 080D90B0
                    *((uint16_t*)(output_buffer_byte_ptr + bytes_written)) = var4;
                    // 080D90B2
                    var6 -= 2;
                }

                // 080D90B6
                var4 <<= 0x18;
                // 080D90B8
                var2 = var4 >> 0x18;
                // 080D90BA
                bytes_written += var6;
                // 080D90BC
                var10 = var2;
            }
        } else {
            // 080D90C0
            var5 = bytes_written - var5;
            // 080D90C2
            if(!is_divisible_by_two(var5)) {
                // 080D90C6
                var5++;
                // 080D90C8
                var2 = *((uint16_t*)(output_buffer_byte_ptr + var5));
                // 080D90CA
                var2 >>= 8;
                // 080D90CC
                bytes_written++;
                // 080D90CE
                var6--;
                if(var6 == 0) {
                    throw std::runtime_error("Not implemented yet.");   
                }
            }
            // 080D90D2
            bytes_written--;
            while((int16_t)var6 > 0) {
                // 080D90D4
                var5 += 2;
                // 080D90D6
                var4 = *((uint16_t*)(output_buffer_byte_ptr + var5));
                // 080D90D8
                var4 <<= 8;
                // 080D90DA
                var4 |= var2;
                // 080D90DC
                bytes_written += 2;
                // 080D90DE
                *((uint16_t*)(output_buffer_byte_ptr + bytes_written)) = var4;

                // 080D90E0
                var2 = var4 >> 0x10;
                // 080D90E2
                var6 -= 2;
            }
            // 080D90E6
            var6++;
            // 080D90E8
            bytes_written += var6;
            // 080D90EA
            var10 = var2;
        }
    } else {
        bool should_increment = true;
        // 080D90EE
        if(!is_divisible_by_two(bytes_written)) {
            // 080D90F2
            var4 = var2 << 8;
            // 080D90F4
            var4 |= var2;
            // 080D90F6
            bytes_written++;
            // 080D90F8
            var6++;
            should_increment = false;
        } else {
            // 080D90FC
            var4 = *((uint16_t*)(output_buffer_byte_ptr + bytes_written));
            // 080D90FE
            var4 >>= 8;
            // 080D9100
            uint32_t var2 = var4 << 8;
            // 080D9102
            var4 |= var2;
        }
        while((int16_t)var6 > 0) {
            if(should_increment) {
                // 080D9104
                bytes_written+=2;
            }
            should_increment = true;
            // 080D9106
            *((uint16_t*)(output_buffer_byte_ptr + bytes_written)) = var4;
            // 080D9108
            var6 -= 2;
        }
        // 080D910C
        var2 = var4 >> 8;
        // 080D910E
        bytes_written += var6;
        // 080D9110
        var10 = var2;
    }
}

static void FUN_080D89114(uint8_t* stack_ptr, BitReader& bit_reader, uint32_t var5)
{
    // FUN_080D89114
    uint32_t var6 = 1;

    for(size_t i = 0; i < var5; ++i) {
        uint32_t var4 = 4;

        uint32_t bit = bit_reader.read_bits(var4);
        // std::cout << "LAB_080D911A: " << "Bit: " << std::hex << (int)bit << std::endl;

        ++bit;

        *(uint16_t*)(stack_ptr + 4) = var6;
        stack_ptr[6] = bit;

        // LAB_080D9124
        stack_ptr += 4;

        uint32_t var1 = 1 << bit;

        var6 += var1;

        // display_stack();
    }
}

void decompress(std::ifstream& rom, size_t offset) {
    rom.seekg(offset);

    uint32_t header;
    if(!rom.read((char*)&header, sizeof(header))) {
        throw std::runtime_error("Failed to read header.");
    }

    uint8_t magic_number = header & 0xFF;
    if(magic_number != 0x70) {
        // throw std::runtime_error("Invalid magic number: " + std::to_string(magic_number));
    }

    uint32_t target_size = (header >> 8) / 2;

    bit_reader = BitReader(rom);
    uint8_t byte = bit_reader.read_byte();

    stack.resize(0x20, 0x00);
    *((uint32_t*)stack.data()) = byte;

    output_buffer.resize(target_size);

    uint32_t type_of_something = byte;
    type_of_something <<=27;
    type_of_something >>= 30;

    std::cout << "Offset: 0x" << std::hex << offset << std::endl;
    std::cout << "Magic Number: 0x" << std::hex << (int)magic_number << std::endl;
    std::cout << "Target Size: 0x" << std::hex << (int)target_size << std::endl;

    uint32_t some_callback;

    switch(type_of_something) {
        case 2:
            {
                some_callback = 0x080D8DA7;
                int var3 = 8;
                //FUN_080D8D18(rom, dst-2, bit buffer, remaining bytes, 8, DAT_0X0D8DFC: 080D8DA7)
                int size_of_something = 1 << var3;
                size_t old_size = stack.size();
                size_t bytes_to_alloc = (sizeof(uint32_t) * (size_of_something));
                stack.resize(old_size + bytes_to_alloc, -1);
                uint8_t* stack_ptr = stack.data();

                // LAB_080D8D2E
                uint32_t current_table_write = 0x20;

                // LAB_080d8D32
                int r5_something = 0;
                // LAB_080d8D34
                // Stored at R12 and working in R6
                int count = 0;
                // LAB_080d8D38
                int r6_something = var3 << 1;

                // LAB_080d8D8E
                while(r6_something) {
                    // Enough time to not flood the console
                    // std::this_thread::sleep_for(std::chrono::milliseconds(500));

                    // display_stack();

                    //std::cout << "LAB_080d8D8E: " << "R6: " << std::hex << (int)r6_something << std::endl;

                    // LAB_080d8D3A
                    r5_something <<= 1;
                    //std::cout << "LAB_080d8D3A: " << "R5: " << std::hex << (int)r5_something << std::endl;

                    // LAB_080d8D3E
                    uint8_t byte = bit_reader.read_bits(var3);
                    //std::cout << "LAB_080d8DCC (read_bits): " << "Byte: " << std::hex << (int)byte << std::endl;

                    // LAB_080d8D42
                    if(byte) {
                        // LAB_080d8D46
                        uint32_t previous_r6_something = r6_something;
                        for(int iterator = 0; iterator < byte; ++iterator) {
                            // LAB_080d8D4A
                            int32_t value_to_sum_to_r4 = 0x20;
                           // std::cout << "LAB_080d8D4A: " << "Current Table Write reset to: " << std::hex << (int)current_table_write << std::endl;   

                            // LAB_080d8D4C
                            uint32_t local_count = count;
                           // std::cout << "LAB_080d8D4C: " << "Local Count: " << std::hex << (int)local_count << std::endl;
                            // LAB_080d8D4E
                            while(local_count) {
                                // LAB_080D8D52
                                uint32_t r4_something = r5_something >> local_count;
                                r4_something <<= 0x1F;
                                r4_something >>= 0x1E;
                                r4_something += value_to_sum_to_r4;

                               // std::cout << "LAB_080D8D52: " << "R4: " << std::hex << (int)r4_something << std::endl;

                                // LAB_080D8D5C
                                int16_t* value_to_sum_to_r4_ptr = (int16_t*)(stack_ptr + r4_something);
                                value_to_sum_to_r4 = *value_to_sum_to_r4_ptr;
                                //std::cout << "LAB_080D8D5C: " << "R1: " << std::hex << (int)value_to_sum_to_r4 << std::endl;

                                // LAB_080D8D5E - LAB_080D8D60
                                if(value_to_sum_to_r4 <= 0) {
                                    // LAB_080D8D62
                                    current_table_write+=4;
                                    *value_to_sum_to_r4_ptr = current_table_write;
                                    value_to_sum_to_r4 = current_table_write;
                                    //std::cout << "LAB_080D8D62: " << "R1: " << std::hex << (int)value_to_sum_to_r4 << std::endl;
                                   // std::cout << "LAB_080D8D68: strh " << std::hex << current_table_write << ", " << r4_something << std::endl;
                                }

                                local_count--;
                            }

                            // LAB_080D8D6e
                            uint32_t var6 = r5_something << 0x1F;
                            // LAB_080D8D70
                            var6 >>= 0x1E;

                            // LAB_080D8D72
                            var6 += current_table_write;

                            // LAB_080D8D76
                            int32_t next_to_negate = bit_reader.read_bits(var3);

                            // LAB_080D8D7C
                            next_to_negate = ~next_to_negate;

                            *((uint16_t*)(stack_ptr + var6)) = next_to_negate;

                            //std::cout << "LAB_080D8D7C: " << "strh " << std::hex << (next_to_negate) << ", " << var6 << std::endl;

                            ++r5_something;
                        }
                        r6_something = previous_r6_something;
                    }

                    //std::cout << "LAB_080d8D8A: incrementing count and decrementing r6_something" << std::endl;

                    // LAB_080d8D8A
                    ++count;

                    // LAB_080D8D8C
                    r6_something--;

                    //std::cout << "LAB_080d8D8A-8C: " << "count: " << std::hex << count << std::endl;
                    //std::cout << "LAB_080d8D8A-8C: " << "R6: " << std::hex << r6_something << std::endl;
                }
            }
        break;
        default:
            throw std::runtime_error("Unknown type of something: " + std::to_string(type_of_something));
        break;
    }

    // 0x0D8CA2
    uint32_t second_type_of_something = byte;
    second_type_of_something <<= 0x1D;
    second_type_of_something >>= 0x1D;

    std::cout << std::endl << std::endl;
    std::cout << "Step 2, type: " << std::hex << (int)second_type_of_something << std::endl;

    uint32_t bytes_written = 0;
    uint8_t* output_buffer_byte_ptr = (uint8_t*)output_buffer.data();

    output_buffer[0] = 0x0200;
    switch (second_type_of_something)
    {
    case 2:
        // 0X080D8CAC
        {
            // FUN_080D8F14()
            // 080D8F14
            {
                uint32_t var5 = 7;
                FUN_080D89114((uint8_t*)stack.data(), bit_reader, var5);
            }
            // std::cout << "LAB_080D8F1C: " << std::hex << (int)bit_reader.buffer() << std::endl;
            // std::cout << "LAB_080D8F1C: " << std::hex << (int)bit_reader.available_bits() << std::endl;
            // 0x0D8F22
            // TODO: find where this 8 comes from
            uint32_t var10 = 8;
            // TODO: find initial value of var10
            uint32_t var6 = 0;
            // LAB_080D8F20
            while(1) {
                // FUN_080D8F9E()
                // FUN_080D8DA7()
                if(bytes_written >= 562) {
                    // display_output_buffer();
                    //std::cout << "bytes_written: " << std::hex << bytes_written << std::endl;
                }
                {
                    // 080D8F26
                    if(bit_reader.read_bits(1)) {
                        // 080D8F28
                        uint32_t var4 = bit_reader.read_bits(3);
                        if(var4 != 7) {
                            // testado
                            // 080D8F32
                            uint32_t var5 = var4 << 2;
                            // 080D8F36
                            var4 = stack[var5 + 6];
                            // 080D8F38
                            var4 = bit_reader.read_bits(var4);
                            // 080D8F3C
                            var5 = *(uint16_t*)(stack.data() + var5 + 4);
                            // 080D8F3E
                            var5 += var4;
                            // 080D8F40
                            var4 = bit_reader.read_bits(4);
                            // 080D8F46
                            var6 = var4 + 3;

                            FUN_080D9084(bytes_written, output_buffer_byte_ptr, var4, var5, var6, var10);
                        } else {
                            // 080D8F4A
                            var6 = 0;
                            bool condition;
                            do {
                                // 080D8F4E / 080D8F4C
                                var4 = bit_reader.read_bits(4);
                                // 080D8F52
                                var6 <<= 4;
                                // 080D8F54
                                var6 += var4;
                                condition = var6 & 1;
                                // 080D8F56
                                var6 >>= 1;
                            } while(condition);
                            // 080D8F5C 080D8F5E / 080D8F60
                            if(bit_reader.read_bits(1)) {
                                // 080D8F62
                                var4 = bit_reader.read_bits(3);
                                // 080D8F68
                                uint32_t var5 = var4 << 2;
                                // 080D8F6A
                                var5 += 0;
                                // 080D8F6C
                                var4 = *(stack.data() + var5 + 6);
                                // 080D8F6E
                                var4 = bit_reader.read_bits(var4);
                                // 080D8F72
                                var5 = *(uint16_t*)(stack.data() + var5 + 4);
                                // 080D8F74
                                var5 += var4;
                                // 080D8F76
                                var4 = bit_reader.read_bits(4);
                                // 080D8F7C
                                var6 <<= 4;
                                // 080D8F7E
                                var6 += var4;
                                // 080D8F80
                                var6 += 3;
                                // 080D8F82
                                FUN_080D9084(bytes_written, output_buffer_byte_ptr, var4, var5, var6, var10);
                            } else {
                                // 080D8F84
                                var6++;
                                do {
                                    // 080D8F86 / 080D8F88
                                    var4 = find_negative();
                                    // 080D8F8A
                                    bytes_written++;
                                    // 080D8F8C / 080D8F8E
                                    if(is_divisible_by_two(bytes_written)) {
                                        // 080D8F90
                                        var4 <<= 8;
                                        // 080D8F92
                                        var4 += var10;
                                        // 080D8F94
                                        *((uint16_t*)(output_buffer_byte_ptr + bytes_written)) = var4;
                                    }
                                    // 080D8F96
                                    var10 = var4;
                                    // 080D8F98
                                    var6--;
                                } while((int32_t)var6 > 0);
                            }
                        }
                    } else {
                        // 0x080D8DA8
                        uint16_t var4 = find_negative();
                        // 080D8FA2
                        ++bytes_written;
                        // 080D8FA26
                        if(is_divisible_by_two(bytes_written)) {
                            // 080D8FA8
                            var4 <<= 8;
                            // 080D8FAA
                            var4 += var10;
                            // 080D8FAC
                            *((uint16_t*)(output_buffer_byte_ptr + bytes_written)) = var4;
                        }
                        // 080D8FAE
                        var10 = var4;
                    }
                }
                // 080D8FB2
                if((bytes_written >= target_size)) {
                    break;
                }
            }
        }
    break;
    case 3:
        {
            // FUN_080D8FC4(rom, dst-2, bit buffer, remaining bytes, 0)
            {
                uint32_t var5 = 3;
                FUN_080D89114((uint8_t*)stack.data(), bit_reader, var5);
            }

            //display_stack();

            // LAB_080D8FCE
            while(bytes_written < target_size) {
                // LAB_080D8FD0
                // discard one bit
                uint32_t carry = bit_reader.read_bits(1);
                if(!carry) {
                    // LAB_080D8FD6
                    int16_t var5 = find_negative();
                    // LAB_080D8FDE
                    int16_t var4 = find_negative();

                    // LAB_080D8FDE
                    var4 <<= 8;

                    // LAB_080D8FE0
                    var4 |= var5;

                    output_buffer[bytes_written] = var4;
                    bytes_written++;

                    if(bytes_written >= target_size) {
                        break;
                    }
                } else {
                    // 080D8FE8
                    uint32_t var4 = bit_reader.read_bits(2);

                    uint32_t var5;
                    uint32_t var6;

                    // 080D8FEE
                    if(var4 != 3) {
                        // 080D8FF2
                        var5 = var4 << 2;
                        // 080D8FF4/080D8FF6
                        var4 = stack[var5 + 6];

                        // 080D8FF8
                        var4 = bit_reader.read_bits(var4);
                        //std::cout << "080D8FF8 R4: " << std::hex << (int)var4 << std::endl;

                        // 080D8FFC
                        var5 = *(uint16_t*)(stack.data() + var5 + 4);
                        //std::cout << "080D8FFC R5: " << std::hex << (int)var5 << std::endl;

                        // 080D8FFE
                        var5 += var4;
                        //std::cout << "080D8FFE R5: " << std::hex << (int)var5 << std::endl;

                        // 080D9000
                        var5 <<= 1;
                        //std::cout << "080D9000 R5: " << std::hex << (int)var5 << std::endl;

                        // 080D9004
                        var4 = bit_reader.read_bits(3);
                        //std::cout << "080D9004 R4: " << std::hex << (int)var4 << std::endl;

                        // 080D9008
                        var6 = var4 + 2;
                        //std::cout << "080D9008 R6: " << std::hex << (int)var6 << std::endl;

                    } else {
                        // 080D900C
                        var6 = 0;
                        bool condition = true;
                        //std::cout << "buffer address: " << std::hex << (int)bit_reader.address() << std::endl;
                        while(condition) {
                            // 080D900e
                            uint32_t var4 = bit_reader.read_bits(3);
                            //std::cout << "080D900E R4: " << std::hex << (int)var4 << std::endl;

                            // 080D9014
                            var6 <<= 3;
                            //std::cout << "080D9014 R6: " << std::hex << (int)var6 << std::endl;
                            
                            // 080D9016
                            var6 += var4;
                            //std::cout << "080D9016 R6: " << std::hex << (int)var6 << std::endl;

                            // 080D9018
                            condition = var6 & 0x1;
                            var6 >>= 1;
                            //std::cout << "080D9018 R6: " << std::hex << (int)var6 << std::endl;

                            if(var6 + bytes_written >= target_size) {
                                throw std::logic_error("Error: var6 + bytes_written >= target_size");
                            }
                        }

                        if(bit_reader.read_bits(1)) {
                            // 080D9026
                            uint32_t var4 = bit_reader.read_bits(2);
                            //std::cout << "080D9026 R4: " << std::hex << (int)var4 << std::endl;

                            // 080D902A
                            var5 = var4 << 2;
                            //std::cout << "080D902A R5: " << std::hex << (int)var5 << std::endl;

                            // 080D902E
                            var4 = stack[var5 + 6];
                            //std::cout << "080D902E R4: " << std::hex << (int)var4 << std::endl;

                            // 080D9030
                            var4 = bit_reader.read_bits(var4);
                            //std::cout << "080D9030 R4: " << std::hex << (int)var4 << std::endl;

                            // 080D9034
                            var5 = *(uint16_t*)(stack.data() + var5 + 4);
                            //std::cout << "080D9034 R5: " << std::hex << (int)var5 << std::endl;

                            // 080D9036
                            var5 += var4;
                            //std::cout << "080D9036 R5: " << std::hex << (int)var5 << std::endl;

                            // 080D9038
                            var5 <<= 1;
                            //std::cout << "080D9038 R5: " << std::hex << (int)var5 << std::endl;

                            //std::cout << "080D903C buffer: " << std::hex << (int)bit_reader.buffer() << std::endl;

                            var4 = bit_reader.read_bits(3);
                            //std::cout << "080D903C R4: " << std::hex << (int)var4 << std::endl;

                            // 080D9040
                            var6 <<= 3;
                            //std::cout << "080D9040 R6: " << std::hex << (int)var6 << std::endl;

                            // 080D9042
                            var6 += var4;
                            //std::cout << "080D9042 R6: " << std::hex << (int)var6 << std::endl;

                            // 080D9044
                            var6 += 2;
                            //std::cout << "080D9044 R6: " << std::hex << (int)var6 << std::endl;
                        } else {
                            // 080D9048
                            var6 += 1;

                            for(size_t i = 0; i < var6; ++i) {
                                //std::cout << "080D9048 R6: " << std::hex << (int)var6 << std::endl;

                                // 080D904C
                                uint16_t var5 = find_negative();
                                //std::cout << "080D904C R5: " << std::hex << (int)var5 << std::endl;

                                // 080D9052
                                uint16_t var4 = find_negative();
                                //std::cout << "080D9052 R4: " << std::hex << (int)var4 << std::endl;

                                // 080D9054
                                var4 <<= 8;
                                //std::cout << "080D9054 R4: " << std::hex << (int)var4 << std::endl;

                                // 080D9056
                                var4 += var5;
                                //std::cout << "080D9056 R4: " << std::hex << (int)var4 << std::endl;

                                // 080D9058
                                output_buffer[bytes_written] = var4;
                                bytes_written++;
                            }

                            // std::cout << std::endl;
                            // std::cout << std::endl;
                            // std::cout << std::endl;
                            // std::cout << "080D9072 buffer: " << std::hex << (int)bit_reader.buffer() << std::endl;
                            // std::cout << "080D9072 available_bits: " << std::hex << (int)bit_reader.available_bits() << std::endl;
                            // std::cout << "080D9072 bytes written: " << std::hex << bytes_written * 2 << std::endl;
                            // std::cout << std::endl;
                            // std::cout << std::endl;
                            // std::cout << std::endl;

                            if(bytes_written >= target_size) {
                                throw std::runtime_error("Not implemented yet.");
                            }

                            continue;
                        }
                    }

                    // Repeats a previous value from output_buffer var6 times

                    // 080D9066
                    // To allow better visualization and comparison with the original code, I'm using indexes, not pointers.
                    int write_buffer_ptr = 0;
                    int current_write_buffer_ptr = bytes_written * 2;
                    int lookup_buffer_ptr = current_write_buffer_ptr - var5;

                    if(lookup_buffer_ptr < 0) {
                        throw std::runtime_error("Not implemented yet.");
                    }

                    for(size_t i = 0; i < var6; ++i) {
                        uint16_t half_to_write = *(uint16_t*)(((uint8_t*)output_buffer.data()) + lookup_buffer_ptr);
                        //std::cout << "080D9066 R4: " << std::hex << (int)half_to_write << std::endl;
                        output_buffer[bytes_written] = half_to_write;
                        bytes_written++;

                        lookup_buffer_ptr+=2;
                    }

                    if(bytes_written >= target_size) {
                        throw std::runtime_error("Not implemented yet.");
                    }
                }
            }

            break;
        }
        break;
    default:
        throw std::runtime_error("Unknown second type of something: " + std::to_string(second_type_of_something));
        break;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <source_offset_hex> <output_file>" << std::endl;
        std::cerr << "  <source_offset_hex>: Offset in the GBA ROM (e.g., 6F3D14)" << std::endl;
        std::cerr << "(you gave " << argc << " arguments)" << std::endl;
        return 1;
    }

    std::string input_filename = argv[1];
    std::string_view offset_str = argv[2];
    std::string output_filename = argv[3];

    int error = 0;

    try {
        std::ifstream rom(input_filename, std::ios::binary);

        if (!rom) {
            throw std::runtime_error("Failed to open input file: " + input_filename);
        }

        uint32_t offset = 0;
        if(offset_str.starts_with("0x")) {
            offset_str.remove_prefix(2);
            offset = std::stoull(std::string(offset_str), nullptr, 16);
        } else if(isdigit(offset_str[0])) {
            size_t pos = 0;
            offset = std::stoull(std::string(offset_str), &pos, 10);

            if(pos != offset_str.size()) {
                throw std::runtime_error("Invalid offset format. Use 0xXXXXXXXX or decimal.");
            }
        } else {
            throw std::runtime_error("Invalid offset format. Use 0xXXXXXXXX or decimal.");
        }

        if(offset >= 0x08000000) {
            offset -= 0x08000000;
        }

        decompress(rom, offset);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        error = 1;
        return error;
    }
    std::ofstream output(output_filename, std::ios::binary);
    if (!output) {
        throw std::runtime_error("Failed to open output file: " + output_filename);
    }
    output.write(reinterpret_cast<const char*>(output_buffer.data()), output_buffer.size() * sizeof(uint16_t));
    if (!output) {
        throw std::runtime_error("Failed to write to output file: " + output_filename);
    }
    output.close();
    std::cout << "Terminated successfully." << std::endl;
    return error;
}