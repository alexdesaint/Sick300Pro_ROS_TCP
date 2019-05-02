#include "TCPClient.h"


#include <TCPClient.h>
#include <thread>


bool TCPClient::connect(const char *address, unsigned int port) {
    if (sock == -1) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            ROS_ERROR("Could not create socket");
        }
    }

    if (inet_addr(address) == -1) {
        perror("Unknow adresse. Error");
        return false;
    } else
        server.sin_addr.s_addr = inet_addr(address);

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (::connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        ROS_ERROR("connect failed. Error");
        return false;
    }
    return true;
}

bool TCPClient::send(vector<char> data) {
    if (sock != -1) {
        ROS_ERROR("TCP not connected");
        return false;
    }

    if (::send(sock, data.data(), data.size(), 0) < 0) {
        ROS_ERROR("Send failed");
        return false;
    }

    return true;
}

uint8_t TCPClient::read_byte() {
    uint8_t bit;

    while (true) {
        ssize_t size = recv(sock, &bit, 1, 0);

        if (size < 0)
            throw ros::Exception("Cannot read TCP");

        if (size == 1)
            return bit;
        else if (size > 1)
            throw ros::Exception("Out of bound reading TCP");

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

std::vector<uint8_t> TCPClient::read_bytes(size_t toread) {
    std::vector<uint8_t> buff(toread);
    ssize_t readen = 0;

    while (true) {
        toread =  buff.size() - readen;

        ssize_t size = recv(sock, buff.data() + readen, buff.size() - readen, 0);

        if (size < 0)
            throw ros::Exception("Cannot read TCP");

        readen += size;

        if (readen == buff.size())
            return buff;
        else if (readen > buff.size())
            throw ros::Exception("Out of bound reading TCP");

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

static const unsigned short crc_table[256] = {0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108,
                                              0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x0210,
                                              0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 0x9339, 0x8318, 0xb37b,
                                              0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401,
                                              0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee,
                                              0xf5cf, 0xc5ac, 0xd58d, 0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6,
                                              0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d,
                                              0xc7bc, 0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
                                              0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 0x5af5,
                                              0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc,
                                              0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, 0x6ca6, 0x7c87, 0x4ce4,
                                              0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd,
                                              0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13,
                                              0x2e32, 0x1e51, 0x0e70, 0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a,
                                              0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e,
                                              0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
                                              0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x02b1,
                                              0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb,
                                              0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 0x34e2, 0x24c3, 0x14a0,
                                              0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8,
                                              0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657,
                                              0x7676, 0x4615, 0x5634, 0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9,
                                              0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882,
                                              0x28a3, 0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
                                              0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 0xfd2e,
                                              0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07,
                                              0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1, 0xef1f, 0xff3e, 0xcf5d,
                                              0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74,
                                              0x2e93, 0x3eb2, 0x0ed1, 0x1ef0};

unsigned short TCPClient::createCRC(uint8_t *data, ssize_t length, unsigned short CRC_16) {

    for (ssize_t i = 0; i < length; i++) {
        CRC_16 = (CRC_16 << 8) ^ (crc_table[(CRC_16 >> 8) ^ (data[i])]);
    }

    return CRC_16;
}

//
// Data packet structure, as documented in
// https://www.sick.com/media/dox/2/92/892/Telegram_listing_S3000_Standard_Advanced_Professional_Remote_S300_Standard_Advanced_Professional_de_en_IM0022892.PDF
//
// Addr(dec)  Description
// <<<<       the packet begins here
// 0..3       0x00 00 00 00
// <<<<       the telegram size is measured from here when the version of the protocol is 1.02
// 4..5       0x00 00
// 6..7       telegram size, expressed in 2-byte words
// 8..9       0xff07 "cooordination flag, device address"
// 10..11     protocol version, 0x0301
// 12..13     status - 0x0000 normal, 0x0001 lockout
// <<<<       the telegram size is measured from here when the version of the protocol is 1.03
// 14..17     scan number
// 18..19     telegram number
// 20..21     type id of the block, twice (0xbb for continous data streaming block)
// 22..23     0x1111 - complete scan range
// <<<<       end of header
// 24..1105   541 x 2-word measurements
// 1106..1107 CRC-16
// <<<<       packet ends here

// therefore:
const unsigned int telegram_size_location = 6;
const unsigned int protocol_version_number_location = 10;
const unsigned int packetBegin = 4;
const unsigned int header_size = 24;

const unsigned int scan_number_location = 14;
const unsigned int telegram_number_location = 18;
const unsigned int block_type_id_location = 20;

// uncounted_header_part + telegram size = total packet size (1108)
const unsigned int uncounted_header_part_0103 = 14;
const unsigned int uncounted_header_part_0102 = 4;
#define PROTOCOL_1_02 0x0102
#define PROTOCOL_1_03 0x0103

// returns -1 if a packet has not been successfully read (soft failure, i.e. we need to try again
// when the rest of the bytes of the sensor message arrive); returns -2 on serious failure e.g.
// the interface handle is invalid and we should try reconnecting
bool TCPClient::readData() {

    // find our continuous data header
    unsigned int byteSkiped = 0, zeros = 0;
    bool found_header = false;

    // stop search if there is less then a complete header in the buffer left
    while (!found_header) {

        if (read_byte() == 0)
            zeros++;
        else {
            byteSkiped++;
            zeros = 0;
        }

        if (zeros == packetBegin) {
            if(byteSkiped != 0)
                ROS_WARN("SerialCommS300: Warning: skipping %u byte but header found.", byteSkiped);
            found_header = true;
        }
    }

    auto header = read_bytes(header_size - packetBegin);

    // get the relevant bits of the header
    // size includes all data from the data block number
    // through to the end of the packet including the checksum
    unsigned short size = short(2) * htons(*reinterpret_cast<unsigned short *> (&header[telegram_size_location - packetBegin]));
    unsigned int total_packet_size;

    // determine which protocol is used
    unsigned short protocol = *reinterpret_cast<unsigned short *> (&header[protocol_version_number_location - packetBegin]);

    // read & calculate the total packet size according to the protocol
    if (protocol == PROTOCOL_1_02) {
        total_packet_size = size - (header_size - uncounted_header_part_0102);
    } else if (protocol == PROTOCOL_1_03) {
        total_packet_size = size - (header_size - uncounted_header_part_0103);
    } else {
        ROS_ERROR_STREAM(
                "SerialCommS300: protocol version " << std::hex << protocol << std::dec << " is unsupported");
        // perhaps we can recover by discarding a byte?
        return false;
    }

    // read the rest of the packet
    auto packet = read_bytes(total_packet_size);


    unsigned short packet_checksum, calc_checksum;

    // the checksum is contained in the last two bytes of the packet
    packet_checksum = *reinterpret_cast<unsigned short *> (&packet[packet.size() - 2]);

    // excluded from checksumming: first four bytes in the header (zeroes), and the checksum bytes themselves
    calc_checksum = createCRC(header.data(), header.size());
    calc_checksum = createCRC(packet.data(), packet.size() - 2, calc_checksum);

    if (packet_checksum != calc_checksum) {
        ROS_ERROR("SerialCommS300: Checksums don't match. size %u, check %u, calc %u ", size, packet_checksum, calc_checksum);
        return false;
    } else { // checksum valid

        if (header[block_type_id_location - packetBegin] != header[block_type_id_location - packetBegin + 1]) {
            ROS_ERROR("SerialCommS300: Bad type header bytes don't match");
        } else { // block type id bytes OK

            if (header[block_type_id_location - packetBegin] == 0xAA) {
                ROS_ERROR("SerialCommS300: We got an I/O data packet, we don't know what to do with it");
            } else if (header[block_type_id_location - packetBegin] == 0xBB) {
                // measurement data length: total packet size - header - checksums (2 bytes)
                // each measurement is 2 bytes
                unsigned int beam_count = (total_packet_size - 2) / 2;

                if (beam_count < 0) {
                    ROS_ERROR_STREAM("SerialCommS300: bad beam count (" << beam_count << ")");
                    return false;
                }

                m_ranges.resize(beam_count);

                for (int i = 0; i < beam_count; i++) {
                    unsigned short distance_cm = (*reinterpret_cast<unsigned short *> (&packet[2 * i]));
                    distance_cm &= 0x1fff; // remove status bits
                    m_ranges[i] = distance_cm / 100.0f;
                }

                // read the scan number (i.e. sensor timestamp - each scan takes 40 ms)
                m_scanNumber = *reinterpret_cast<unsigned int *> (&header[scan_number_location - packetBegin]);
                m_telegramNumber = *reinterpret_cast<unsigned short *> (&header[telegram_number_location - packetBegin]);

            } else if (header[block_type_id_location - packetBegin] == 0xCC) {
                ROS_ERROR("SerialCommS300: We got a reflector data packet, we don't know what to do with it");
            } else {
                ROS_ERROR("SerialCommS300: We got an unknown packet");
            }
        }
    }

    return true;
}


void TCPClient::disconnect() {
    close(sock);
}