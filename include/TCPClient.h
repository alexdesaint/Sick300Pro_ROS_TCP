#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <arpa/inet.h>
#include <vector>
#include <array>
#include <ros/ros.h>

using namespace std;

class TCPClient
{
  private:
    int sock = -1;
    struct sockaddr_in server;

    int zerobytesread_counter;

    std::vector<float> m_ranges;

    unsigned int m_scanNumber;
    unsigned int m_telegramNumber;
    ros::Time m_receivedTime;

    std::vector<uint8_t> read_bytes(size_t toread);

    uint8_t read_byte();

    unsigned short createCRC(uint8_t *data, ssize_t length, unsigned short CRC_16 = 0xFFFF);

  public:
    bool connect(const char *address, unsigned int port);
    bool send(vector<char> data);
    bool readData();
    void disconnect();

    inline std::vector<float>& getRanges() {
        return m_ranges;
    }

    inline unsigned int getScanNumber() {
        return m_scanNumber;
    }

    inline unsigned int getTelegramNumber() {
        return m_telegramNumber;
    }

    inline ros::Time getReceivedTime() {
        return m_receivedTime;
    }
};

#endif
