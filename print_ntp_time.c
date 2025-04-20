#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define NTP_SERVER "188.156.114.7"
#define NTP_PORT 123
#define NTP_PACKET_SIZE 48

void print_binary(uint32_t num) {
    printf("Binary: ");
    for (int i = 31; i >= 0; i--) {
        printf("%d", (num >> i) & 1);
    }

    printf("\n");
}

typedef struct {
    uint8_t li_vn_mode;
    uint8_t stratum;
    uint8_t poll;
    uint8_t precision;
    uint32_t rootDelay;
    uint32_t rootDispersion;
    uint32_t refId;
    uint32_t refTimeStamp[2];
    uint32_t origTimeStamp[2];
    uint32_t recvTimeStamp[2];
    uint32_t txTimeStamp[2];
    // uint32_t ext_field_1;
    uint32_t digest[4];
} ntp_packet;

void print_ntp_time() {
    int sockfd;
    struct sockaddr_in server_addr;
    ntp_packet packet;
    uint32_t tx_time;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(NTP_PORT);
    server_addr.sin_addr.s_addr = inet_addr(NTP_SERVER);

    memset(&packet, 0, sizeof(ntp_packet));
    packet.li_vn_mode = (0 << 6) | (4 << 3) | 3;

    if (sendto(sockfd, &packet, sizeof(ntp_packet), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Send failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    socklen_t addr_len = sizeof(server_addr);
    if (recvfrom(sockfd, &packet, sizeof(ntp_packet), 0, (struct sockaddr*)&server_addr, &addr_len) < 0) {
        perror("Receive failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    close(sockfd);

    print_binary(packet.txTimeStamp[0]);
    printf("Decimal: %u\n", packet.txTimeStamp[0]);
    print_binary(packet.txTimeStamp[1]);
    printf("Decimal: %u\n", packet.txTimeStamp[1]);

    // https://www.ntp.org/reflib/y2k/
    tx_time = ntohl(packet.txTimeStamp[0]) - 2208988800U;
    printf("NTP Time: %s", ctime((const time_t*)&tx_time));

    printf("%u\n", ntohl(packet.refId));
    printf("%u\n", packet.stratum);
}


int main() {
    print_ntp_time();

    return 0;
}
