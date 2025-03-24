#include <woody-woodpacker.h>

woody_status generate_payload(payload_t * payload)
{
    uint8_t *original_payload = (unsigned char *)payload_address;
    payload->size = 0;
    while (original_payload[payload->size] != NOP_64)   // Search for nop in assembly code
        ++payload->size;
    if ((payload->value = malloc(payload->size * sizeof(char))) == NULL)
        return WOODY_ERR;
    ft_memcpy(payload->value, original_payload, payload->size);
    return WOODY_OK;
}
